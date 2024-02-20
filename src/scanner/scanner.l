%{
#include <vector>
#include <sstream>
#include <string.h>

#define YY_DECL int alpha_yylex (void* ylval)

typedef struct {
  unsigned int numline;
  unsigned int numToken;
  char *content;
  char *type;
} alpha_token_t;

unsigned int token_count = 1;
int nested_comments_count = 0;

std::vector<alpha_token_t> tokens;

const char* get_token_repr(alpha_token_t token) {
    std::stringstream ss{""};

    if (strcmp(token.type, "IDENT") == 0) {
        ss << "\"" << token.content << "\" <- char*";
    } else if (strcmp(token.type, "STRING") == 0) {
        ss << "" << token.content << " <- char*";
    } else if (strcmp(token.type, "INTCONST") == 0) {
        ss << token.content << " <- integer";
    } else if (strcmp(token.type, "REALCONST") == 0) {
        ss << token.content << " <- float";
    }

    return strdup(ss.str().c_str());
}

void print_tokens() {
    printf("---------- Lexical Analysis ----------\n");

    for (auto token : tokens) {
        printf("%d: #%d \"%s\" %s %s\n", token.numline, token.numToken, token.content, token.type, get_token_repr(token));
    }
}

void add_token(const char *type, const char *content, unsigned int numline) {
    alpha_token_t token;

    token.numline = numline;
    token.numToken = token_count++;
    token.content = strdup(content);
    token.type = strdup(type);

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
STRING        \"([^"\\\n]*(\\.[^"\\\n]*)*)\"
IDENT         [a-zA-Z0-9_]*
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

{IF}            { add_token("IF", yytext, yylineno); }
{ELSE}          { add_token("ELSE", yytext, yylineno); }
{WHILE}         { add_token("WHILE", yytext, yylineno); }
{FOR}           { add_token("FOR", yytext, yylineno); }
{FUNCTION}      { add_token("FUNCTION", yytext, yylineno); }
{RETURN}        { add_token("RETURN", yytext, yylineno); }
{BREAK}         { add_token("BREAK", yytext, yylineno); }
{CONTINUE}      { add_token("CONTINUE", yytext, yylineno); }
{AND}           { add_token("AND", yytext, yylineno); }
{OR}            { add_token("OR", yytext, yylineno); }
{NOT}           { add_token("NOT", yytext, yylineno); }
{LOCAL}         { add_token("LOCAL", yytext, yylineno); }
{TRUE}          { add_token("TRUE", yytext, yylineno); }
{FALSE}         { add_token("FALSE", yytext, yylineno); }
{NIL}           { add_token("NIL", yytext, yylineno); }
{INTCONST}      { add_token("INTCONST", yytext, yylineno); }
{REALCONST}     { add_token("REALCONST", yytext, yylineno); }
{STRING}        { add_token("STRING", yytext, yylineno); }
{IDENT}         { add_token("IDENT", yytext, yylineno); }
{EQUAL}         { add_token("EQUAL", yytext, yylineno); }
{DIFF}          { add_token("DIFF", yytext, yylineno); }
{INC}           { add_token("INC", yytext, yylineno); }
{DEC}           { add_token("DEC", yytext, yylineno); }
{ASSIGN}        { add_token("ASSIGN", yytext, yylineno); }
{PLUS}          { add_token("PLUS", yytext, yylineno); }
{MINUS}         { add_token("MINUS", yytext, yylineno); }
{MULT}          { add_token("MULT", yytext, yylineno); }
{DIV}           { add_token("DIV", yytext, yylineno); }
{MOD}           { add_token("MOD", yytext, yylineno); }
{GE}            { add_token("GE", yytext, yylineno); }
{LE}            { add_token("LE", yytext, yylineno); }
{GT}            { add_token("GT", yytext, yylineno); }
{LT}            { add_token("LT", yytext, yylineno); }
{CURLY_OPEN}    { add_token("CURLY_OPEN", yytext, yylineno); }
{CURLY_CLOSE}   { add_token("CURLY_CLOSE", yytext, yylineno); }
{PAREN_OPEN}    { add_token("PAREN_OPEN", yytext, yylineno); }
{PAREN_CLOSE}   { add_token("PAREN_CLOSE", yytext, yylineno); }
{BRACKET_OPEN}  { add_token("BRACKET_OPEN", yytext, yylineno); }
{BRACKET_CLOSE} { add_token("BRACKET_CLOSE", yytext, yylineno); }
{SEMICOLON}     { add_token("SEMICOLON", yytext, yylineno); }
{COMMA}         { add_token("COMMA", yytext, yylineno); }
{DOUBLE_DOT}    { add_token("DOUBLE_DOT", yytext, yylineno); }
{DOT}           { add_token("DOT", yytext, yylineno); }
{NAMESPACE}     { add_token("NAMESPACE", yytext, yylineno); }
{COLON}         { add_token("COLON", yytext, yylineno); }
{LINE_COMMENT}  { add_token("LINE_COMMENT", yytext, yylineno); }
{START_BLOCK}   { nested_comments_count = 1; BEGIN(BLOCK_COMMENT); }
{WS}            {}

<BLOCK_COMMENT>"/*" {
    ++nested_comments_count;
}
<BLOCK_COMMENT>"*/" {
    if (--nested_comments_count == 0) {
        BEGIN(INITIAL);
    }
}
<BLOCK_COMMENT>. {}

<<EOF>> {
    if (nested_comments_count > 0) {
        printf("Error: unclosed block comment\n");
        return 1;
    }
}

.               { printf("Error: unexpected character '%s' at line %d\n", yytext, yylineno); }
%%

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    if (!(yyin = fopen(argv[1], "r"))) {
        fprintf(stderr, "Failed to open file %s\n", argv[1]);
        return 1;
    }

    alpha_yylex(NULL);

    print_tokens();

    return 0;
}
