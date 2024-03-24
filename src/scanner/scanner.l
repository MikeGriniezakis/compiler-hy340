%{
#include "parser.hpp"
#include <vector>
#include <sstream>
#include <string.h>

#define YY_DECL int alpha_yylex (void* ylval)

typedef struct {
  unsigned int numline;
  unsigned int numToken;
  char *content;
  char *type;
  char *repr_type;
} alpha_token_t;

unsigned int token_count = 1;
unsigned int nested_comments_count = 0;

unsigned int block_comment_start = 0;
unsigned int block_comment_end = 0;

std::vector<alpha_token_t> tokens;

const char* get_token_repr(alpha_token_t token) {
    std::stringstream ss{""};

    if (strcmp(token.type, "IDENT") == 0) {
        ss << "\"" << token.content << "\" <- char*";
    } else {
        ss << "\"" << token.content << "\" <- " << token.repr_type;
    }

    return strdup(ss.str().c_str());
}

void print_string(alpha_token_t token) {
    std::vector<std::string> invalid_chars;

    printf("%d: #%d ", token.numline, token.numToken);
    for (int i = 0; i < strlen(token.content); i++) {
        if (token.content[i] == '\\') {
            if (i > 0 && token.content[i - 1] == '\\') {
                continue;
            }

            if (i < strlen(token.content) - 1) {
                if (token.content[i + 1] == 'n') {
                    printf("\n");
                } else if (token.content[i + 1] == 't') {
                    printf("\t");
                } else if (token.content[i + 1] == '\\') {
                    printf("\\");
                } else if (token.content[i + 1] == '\"') {
                    printf("\"");
                } else if (token.content[i + 1] == '\'') {
                    printf("\'");
                } else {
                    invalid_chars.push_back("\\" + std::string(1, token.content[i+1]));
                    printf("\\%c", token.content[i+1]);
                }
            }
        } else if (i > 0) {
            if (token.content[i - 1] != '\\' || (token.content[i - 1] == '\\' && token.content[i - 2] == '\\')) {
                printf("%c", token.content[i]);
            }
        } else if (i == 0) {
            printf("%c", token.content[i]);
        }
    }
    printf(" STRING <- char*\n");

    if (invalid_chars.size() > 0) {
        printf("\nWARNING: invalid escape sequences in string at line %d\n", token.numline);
        printf("Invalid escape sequences: ");
        for (auto c : invalid_chars) {
            printf("%s ", c.c_str());
        }
        printf("\n");
    }
}

void print_tokens() {
    printf("---------- Lexical Analysis ----------\n");

    for (auto token : tokens) {
        if (strcmp(token.type, "STRING") == 0) {
            print_string(token);
        } else {
            printf("%d: #%d \"%s\" %s %s\n", token.numline, token.numToken, token.content, token.type, get_token_repr(token));
        }
    }
}

void add_token(const char *type, const char *content, unsigned int numline, const char *repr_type = "enumerated") {
    alpha_token_t token;

    token.numline = numline;
    token.numToken = token_count++;
    token.content = content != NULL ? strdup(content) : nullptr;
    token.type = strdup(type);
    token.repr_type = strdup(repr_type);

    tokens.push_back(token);
}
%}

%option noyywrap
%option yylineno

%x BLOCK_COMMENT

IF            "if"
ELSE          "else"
WHILE         "while"
FOR           "for"
FUNCTION      "function"
RETURN        "return"
BREAK         "break"
CONTINUE      "continue"
AND           "and"
OR            "or"
NOT           "not"
LOCAL         "local"
TRUE          "true"
FALSE         "false"
NIL           "nil"
INTCONST      [0-9]+
REALCONST     [0-9]+\.[0-9]+
STRING        \"([^"\\]*(\\.[^"\\]*)*)\"
IDENT         [a-zA-Z0-9][a-zA-Z0-9_]*
EQUAL         "=="
DIFF          "!="
INC           "++"
DEC           "--"
ASSIGN        "="
PLUS          "+"
MINUS         "-"
MULT          "*"
DIV           "/"
MOD           "%"
GE            ">="
LE            "<="
GT            ">"
LT            "<"
CURLY_OPEN    "{"
CURLY_CLOSE   "}"
PAREN_OPEN    "("
PAREN_CLOSE   ")"
BRACKET_OPEN  "["
BRACKET_CLOSE "]"
SEMICOLON     ";"
COMMA         ","
DOUBLE_DOT    ".."
DOT           "."
NAMESPACE     "::"
COLON         ":"
LINE_COMMENT  "//".*
START_BLOCK   "/*"

WS          [ \t\n]

%%

{IF}            { add_token("IF", yytext, yylineno); return IF;}
{ELSE}          { add_token("ELSE", yytext, yylineno); return ELSE;}
{WHILE}         { add_token("WHILE", yytext, yylineno); return WHILE;}
{FOR}           { add_token("FOR", yytext, yylineno); return FOR;}
{FUNCTION}      { add_token("FUNCTION", yytext, yylineno); return FUNCTION;}
{RETURN}        { add_token("RETURN", yytext, yylineno); return RETURN;}
{BREAK}         { add_token("BREAK", yytext, yylineno); return BREAK;}
{CONTINUE}      { add_token("CONTINUE", yytext, yylineno); return CONTINUE;}
{AND}           { add_token("AND", yytext, yylineno); return AND;}
{OR}            { add_token("OR", yytext, yylineno); return OR;}
{NOT}           { add_token("NOT", yytext, yylineno); return NOT;}
{LOCAL}         { add_token("LOCAL", yytext, yylineno); return LOCAL;}
{TRUE}          { add_token("TRUE", yytext, yylineno); alpha_yylval.boolValue = false; return TRUE;}
{FALSE}         { add_token("FALSE", yytext, yylineno); alpha_yylval.boolValue = true; return FALSE;}
{NIL}           { add_token("NIL", yytext, yylineno); return NIL;}
{INTCONST}      { add_token("INTCONST", yytext, yylineno, "int"); alpha_yylval.intValue = atoi(yytext); return INTEGER;}
{REALCONST}     { add_token("REALCONST", yytext, yylineno, "float"); alpha_yylval.doubleValue = atof(yytext); return FLOAT;}
{STRING}        { add_token("STRING", yytext, yylineno, "char*"); alpha_yylval.stringValue = strdup(yytext); return STRING;}
{IDENT}         { add_token("IDENT", yytext, yylineno, "char*"); alpha_yylval.stringValue = strdup(yytext); return ID;}
{EQUAL}         { add_token("EQUAL", yytext, yylineno); return EQUAL;}
{DIFF}          { add_token("DIFF", yytext, yylineno); return DIFF;}
{INC}           { add_token("INC", yytext, yylineno); return INC;}
{DEC}           { add_token("DEC", yytext, yylineno); return DEC;}
{ASSIGN}        { add_token("ASSIGN", yytext, yylineno); return ASSIGN;}
{PLUS}          { add_token("PLUS", yytext, yylineno); return PLUS;}
{MINUS}         { add_token("MINUS", yytext, yylineno); return MINUS;}
{MULT}          { add_token("MULT", yytext, yylineno); return MULT;}
{DIV}           { add_token("DIV", yytext, yylineno); return DIV;}
{MOD}           { add_token("MOD", yytext, yylineno); return MOD;}
{GE}            { add_token("GE", yytext, yylineno); return GE;}
{LE}            { add_token("LE", yytext, yylineno); return LE;}
{GT}            { add_token("GT", yytext, yylineno); return GT;}
{LT}            { add_token("LT", yytext, yylineno); return LT;}
{CURLY_OPEN}    { add_token("CURLY_OPEN", yytext, yylineno); return CURLY_OPEN;}
{CURLY_CLOSE}   { add_token("CURLY_CLOSE", yytext, yylineno); return CURLY_CLOSE;}
{PAREN_OPEN}    { add_token("PAREN_OPEN", yytext, yylineno); return PAREN_OPEN;}
{PAREN_CLOSE}   { add_token("PAREN_CLOSE", yytext, yylineno); return PAREN_CLOSE;}
{BRACKET_OPEN}  { add_token("BRACKET_OPEN", yytext, yylineno); return BRACKET_OPEN;}
{BRACKET_CLOSE} { add_token("BRACKET_CLOSE", yytext, yylineno); return BRACKET_CLOSE;}
{SEMICOLON}     { add_token("SEMICOLON", yytext, yylineno); return SEMICOLON; }
{COMMA}         { add_token("COMMA", yytext, yylineno); return COMMA;}
{DOUBLE_DOT}    { add_token("DOUBLE_DOT", yytext, yylineno); return DOUBLE_DOT;}
{DOT}           { add_token("DOT", yytext, yylineno); return DOT;}
{NAMESPACE}     { add_token("NAMESPACE", yytext, yylineno); return NAMESPACE;}
{COLON}         { add_token("COLON", yytext, yylineno); return COLON;}
{LINE_COMMENT}  { add_token("LINE_COMMENT", "", yylineno);}
{START_BLOCK}   { nested_comments_count = 1; block_comment_start = yylineno; BEGIN(BLOCK_COMMENT); }
{WS}            {}

<BLOCK_COMMENT>"/*" {
    ++nested_comments_count;
    BEGIN(BLOCK_COMMENT);
}
<BLOCK_COMMENT>"*/" {
    if (--nested_comments_count == 0) {
        block_comment_end = yylineno;
        std::stringstream ss{""};
        ss << block_comment_start << " - " << block_comment_end;
        add_token("BLOCK_COMMENT", ss.str().c_str(), yylineno);
        BEGIN(INITIAL);
    }
}
<BLOCK_COMMENT>[/*]|[^*/] {}

<BLOCK_COMMENT><<EOF>> {
    if (nested_comments_count > 0) {
        printf("Error: unclosed block comment\n");
        return 1;
    }
}

.               { printf("Error: unexpected character '%s' at line %d\n", yytext, yylineno); }
%%
