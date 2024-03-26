%{
    #include <stdio.h>
    #include <src/symbolTable/SymbolTable.h>

    int alpha_yyerror (const char* yaccProvidedMessage);
    extern int alpha_yylex(void* ylval);

    extern int yylineno;
    extern char* yytext;
    extern FILE* yyin;

    SymbolTable* symbolTable = new SymbolTable();

    int functionScopeCount = 0;
    int forScopeCount = 0;
    int whileScopeCount = 0;

    bool isMethodCall = false;
    bool isMemberCall = false;
    bool isFunction = false;
    bool isScopeIncreasedByFunction = false;

    void insertToken(struct SymbolStruct* symbol, bool isArgument) {
        Symbol* existingSymbol = nullptr;
        if (symbol->type == GLOBAL) {
            existingSymbol = symbolTable->lookupSymbolGlobal(symbol->name);
        } else if (symbol->type == ASSIGNMENT) {
            existingSymbol = symbolTable->lookupSymbol(symbol->name);
        } else {
            existingSymbol = symbolTable->lookupSymbolScoped(symbol->name);
        }

        if (symbol->type == GLOBAL && existingSymbol == nullptr) {
            char message[100];
            sprintf(message, "Global variable %s not found", symbol->name);
            yyerror(message);

            return;
        }

        if (existingSymbol == nullptr) {
            if (symbolTable->isNameReserved(symbol->name)) {
                char message[100];
                sprintf(message, "Reserved name %s cannot be used", symbol->name);
                yyerror(message);

                return;
            }
        }

        if (symbol->type == SCOPED) {
            if (existingSymbol == nullptr) {
                symbolTable->insertSymbol(symbol->name, symbol->line, isFunction, false);
                isFunction = false;
            }
        }


        if (symbol->type == ASSIGNMENT) {
            if (existingSymbol == nullptr) {
                symbolTable->insertSymbol(symbol->name, symbol->line, isFunction, false);
                isFunction = false;
            } else {
                if (existingSymbol->getScope() < (int) symbolTable->getScope() && existingSymbol->getScope() != 0 && (existingSymbol->getType() == SCOPED || existingSymbol->getType() == FORMAL)) {
                    char message[100];
                    sprintf(message, "%s not defined", symbol->name);
                    yyerror(message);
                } else if (existingSymbol->getSymbolClass() != isFunction ? FUNC : VAR && !isMemberCall && !isArgument) {
                    char message[100];
                    sprintf(message, "%s, %s redefined as %s", symbol->name, existingSymbol->getSymbolClass() == FUNC ? "function" : "variable", isFunction ? "function" : "variable");
                    yyerror(message);
                }

                isMemberCall = false;
            }
        }
    }
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
    struct SymbolStruct* symbol;
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

%type <symbol> lvalue
%type <symbol> idlist

%%

program: stmts | ;

stmts:
    stmt
    | stmts stmt
    |
    ;

stmt:
    expr SEMICOLON
    | ifstmt
    | whilestmt
    | forstmt
    | returnstmt
    | BREAK {
        if (forScopeCount == 0 && whileScopeCount == 0) {
            yyerror("Break must be used inside a loop");
        }
    } SEMICOLON
    | CONTINUE {
        if (forScopeCount == 0 && whileScopeCount == 0) {
            yyerror("Break must be used inside a loop");
        }
    } SEMICOLON
    | block
    | funcdef
    | SEMICOLON
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
    lvalue ASSIGN expr { insertToken($1, false); }
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
    lvalue { insertToken($1, true); }
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
    ID {
        SymbolStruct* symbolStruct = new SymbolStruct();
        symbolStruct->name = $1;
        symbolStruct->line = yylineno;
        symbolStruct->type = ASSIGNMENT;

        $$ = symbolStruct;
      }
    | LOCAL ID {
        SymbolStruct* symbolStruct = new SymbolStruct();
        symbolStruct->name = $2;
        symbolStruct->line = yylineno;
        symbolStruct->type = SCOPED;

        $$ = symbolStruct;
    }
    | NAMESPACE ID {
        SymbolStruct* symbolStruct = new SymbolStruct();
        symbolStruct->name = $2;
        symbolStruct->line = yylineno;
        symbolStruct->type = GLOBAL;

        $$ = symbolStruct;
    }
    | member {
        isMemberCall = true;
    }
    ;

member:
    lvalue DOT ID
    | lvalue BRACKET_OPEN expr BRACKET_CLOSE
    | call DOT ID
    | call BRACKET_OPEN expr BRACKET_CLOSE
    ;

call:
    call PAREN_OPEN elist PAREN_CLOSE
    | lvalue callsuffix {
        Symbol* existingSymbol = symbolTable->lookupSymbol($1->name);

        if (isMethodCall) {
            isMethodCall = false;
        } else if (existingSymbol == nullptr) {
            char message[100];
            sprintf(message, "Function %s not defined", $1->name);
            yyerror(message);
        }
    }
    | PAREN_OPEN funcdef PAREN_CLOSE PAREN_OPEN elist PAREN_CLOSE
    ;

callsuffix:
    normcall
    | methodcall { isMethodCall = true; }
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
    |
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
    CURLY_OPEN { if (!isScopeIncreasedByFunction) {symbolTable->incScope();} else { isScopeIncreasedByFunction = false; } } stmts CURLY_CLOSE { symbolTable->decScope(); }
    ;

funcdef:
    FUNCTION { functionScopeCount++; } PAREN_OPEN { isScopeIncreasedByFunction = true; symbolTable->incScope(); } idlist PAREN_CLOSE block { functionScopeCount--; }
    | FUNCTION { functionScopeCount++; isFunction = true; } ID
     {
        Symbol* symbol = symbolTable->lookupSymbolScoped($3);

        if (symbolTable->isNameReserved($3)) {
            char message[100];
            sprintf(message, "Reserved name %s cannot be used", $3);
            yyerror(message);
        }

        if (symbol == nullptr) {
           symbol = symbolTable->insertSymbol($3, yylineno, isFunction, false);
        }

        isFunction = false;
     }
     PAREN_OPEN { isScopeIncreasedByFunction = true; symbolTable->incScope(); } idlist PAREN_CLOSE block { functionScopeCount--; }
    ;

idlist:
    ID {
        Symbol* symbol = symbolTable->lookupSymbolScoped($1);

         if (symbol == nullptr) {
             symbol = symbolTable->insertSymbol($1, yylineno, false, true);
         }
    }
    | idlist COMMA ID {
        Symbol* symbol = symbolTable->lookupSymbolScoped($3);

         if (symbol == nullptr) {
             symbol = symbolTable->insertSymbol($3, yylineno, false, true);
         }
    }
    |
    ;

ifstmt:
    IF PAREN_OPEN expr PAREN_CLOSE stmts
    | IF PAREN_OPEN expr PAREN_CLOSE stmts ELSE stmts
    ;

whilestmt:
    WHILE { whileScopeCount++; } PAREN_OPEN expr PAREN_CLOSE stmts { whileScopeCount--; }
    ;

forstmt:
    FOR { forScopeCount++; } PAREN_OPEN elist SEMICOLON expr SEMICOLON elist PAREN_CLOSE stmts { forScopeCount--; }
    ;

returnstmt:
    RETURN { if (functionScopeCount == 0) { yyerror("Return must be used inside a function"); } } expr SEMICOLON
    | RETURN SEMICOLON
    ;

%%

int alpha_yyerror (const char* yaccProvidedMessage) {
    fprintf(stderr, "Line %d: %s\n", yylineno, yaccProvidedMessage);
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

    symbolTable->printSymbolTable();

    return 0;
}
