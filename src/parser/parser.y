%{
    #include <stdio.h>
    #include <src/symbolTable/SymbolTable.h>
    #include <src/parser/parser.h>

    int alpha_yyerror (const char* yaccProvidedMessage);
    extern int alpha_yylex(void* ylval);

    extern int yylineno;
    extern char* yytext;
    extern FILE* yyin;

    SymbolTable* symbolTable = new SymbolTable();
%}

%define parse.error verbose
%define api.prefix "alpha_yy"
%lex-param  {NULL}

%start program

%union {
    int     intValue;
    char*   stringValue;
    double  doubleValue;
    bool    boolValue;
}

%token <stringValue> ID
%token <stringValue> STRING
%token <intValue> INTEGER
%token <doubleValue> FLOAT
%token <boolValue> TRUE FALSE
%token SEMICOLON ASSIGN COMMA DOUBLE_DOT DOT NAMESPACE COLON
%token PLUS MINUS MULT DIV MOD
%token IF ELSE WHILE FOR FUNCTION RETURN BREAK CONTINUE AND OR NOT LOCAL
%token NIL EQUAL DIFF INC DEC GE LE GT LT
%token CURLY_OPEN CURLY_CLOSE PAREN_OPEN PAREN_CLOSE BRACKET_OPEN BRACKET_CLOSE

%right ASSIGN NOT INC DEC
%left PLUS MINUS MULT DIV
%nonassoc UMINUS
%left PAREN_OPEN PAREN_CLOSE

%%

program: stmts | ;

stmts:
    stmt
    | stmts stmt
    ;

stmt:
    expr SEMICOLON
    | ifstmt
    | whilestmt
    | forstmt
    | returnstmt
    | BREAK SEMICOLON
    | CONTINUE SEMICOLON
    | block
    | funcdef
    ;

expr:
    assignexpr
    | expr PLUS expr
    | expr MINUS expr
    | expr MULT expr
    | expr DIV expr
    | expr MOD expr
    | expr GT expr
    | expr GE expr
    | expr LT expr
    | expr LE expr
    | expr EQUAL expr
    | expr DIFF expr
    | expr AND expr
    | expr OR expr
    | term
    ;

assignexpr:
    lvalue ASSIGN expr
    ;

term:
    PAREN_OPEN expr PAREN_CLOSE
    | MINUS expr
    | NOT expr
    | INC lvalue
    | lvalue INC
    | DEC lvalue
    | lvalue DEC
    | primary
    ;

primary:
    lvalue
    | call
    | objectdef
    | PAREN_OPEN funcdef PAREN_CLOSE
    | const
    ;

const:
    INTEGER
    | FLOAT
    | STRING
    | TRUE
    | FALSE
    | NIL
    ;

lvalue:
    ID
    | LOCAL ID
    | NAMESPACE ID
    | member
    ;

member:
    lvalue DOT ID
    | lvalue BRACKET_OPEN expr BRACKET_CLOSE
    | call DOT ID
    | call BRACKET_OPEN expr BRACKET_CLOSE
    ;

call:
    call PAREN_OPEN elist PAREN_CLOSE
    | lvalue callsuffix
    | PAREN_OPEN funcdef PAREN_CLOSE PAREN_OPEN elist PAREN_CLOSE
    ;

callsuffix:
    normcall
    | methodcall
    ;

normcall:
    PAREN_OPEN elist PAREN_CLOSE
    ;

methodcall:
    DOUBLE_DOT ID PAREN_OPEN elist PAREN_CLOSE
    ;

elist:
    expr
    | elist COMMA expr
    ;

objectdef:
    BRACKET_OPEN elist BRACKET_CLOSE
    | BRACKET_OPEN indexed BRACKET_CLOSE
    ;

indexed:
    indexedelem
    | indexed COMMA indexedelem
    ;

indexedelem:
    CURLY_OPEN expr COLON expr CURLY_CLOSE
    ;

block:
    CURLY_OPEN stmts CURLY_CLOSE
    ;

funcdef:
    FUNCTION PAREN_OPEN idlist PAREN_CLOSE block
    | FUNCTION ID PAREN_OPEN idlist PAREN_CLOSE block
    ;

idlist:
    ID
    | idlist COMMA ID
    |
    ;

ifstmt:
    IF PAREN_OPEN expr PAREN_CLOSE stmts
    | IF PAREN_OPEN expr PAREN_CLOSE stmts ELSE stmts
    ;

whilestmt:
    WHILE PAREN_OPEN expr PAREN_CLOSE stmts
    ;

forstmt:
    FOR PAREN_OPEN elist SEMICOLON expr SEMICOLON elist PAREN_CLOSE stmts
    ;

returnstmt:
    RETURN expr SEMICOLON
    | RETURN SEMICOLON
    ;

%%

int alpha_yyerror (const char* yaccProvidedMessage) {
    fprintf(stderr, "%s: at line %d, before token: %s\n", yaccProvidedMessage, yylineno, yytext);
    return 1;
}

int main(int argc, char** argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror(argv[1]);
            return 1;
        }
    }
    yyparse();

    // symbolTable->printSymbolTable();

    return 0;
}
