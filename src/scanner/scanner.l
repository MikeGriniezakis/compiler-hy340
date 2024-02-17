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

IF          "if"
INTCONST    [0-9]+
REALCONST   [0-9]+\.[0-9]+
STRING      \"[^\"]*\"
IDENT       [a-zA-Z][a-zA-Z0-9]*

WS          [ \t\n]

%%

{IF}         { add_token("IF", yytext, yylineno); }
{INTCONST}   { add_token("INTCONST", yytext, yylineno); }
{REALCONST}  { add_token("REALCONST", yytext, yylineno); }
{STRING}     { add_token("STRING", yytext, yylineno); }
{IDENT}      { add_token("IDENT", yytext, yylineno); }
{WS}         {}
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
