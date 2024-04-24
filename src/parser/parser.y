%{
    #include <stdio.h>
    #include <string.h>
    #include <src/symbolTable/SymbolTable.h>
    #include <sstream>
    #include <src/quads/Quads.h>

    int alpha_yyerror (const char* yaccProvidedMessage);
    extern int alpha_yylex(void* ylval);

    extern int yylineno;
    extern char* yytext;
    extern FILE* yyin;

    SymbolTable* symbolTable = new SymbolTable();
    Quads* quads = new Quads();

    int functionCount = 0;
    int functionScopeCount = 0;
    int forScopeCount = 0;
    int whileScopeCount = 0;

    bool isMethodCall = false;
    bool isMemberCall = false;
    bool isFunction = false;
    bool isScopeIncreasedByFunction = false;

    std::vector<char*> errors;

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
                symbolTable->insertSymbol(symbol->name, symbol->line, isFunction, false, functionScopeCount);
                isFunction = false;
            }
        }


        if (symbol->type == ASSIGNMENT) {
            if (existingSymbol == nullptr) {
                symbolTable->insertSymbol(symbol->name, symbol->line, isFunction, false, functionScopeCount);
                isFunction = false;
            } else {
                if (existingSymbol->getScope() < (int) symbolTable->getScope() && existingSymbol->getFunctionScope() != functionScopeCount && existingSymbol->getScope() != 0 && (existingSymbol->getType() == SCOPED || existingSymbol->getType() == FORMAL)) {
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
    struct expr* expr;
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

%type <expr> term
%type <expr> assignexpr
%type <expr> expr
%type <expr> lvalue
%type <expr> primary
%type <expr> const
%type <expr> member
%type <expr> funcdef
%type <expr> methodcall
%type <expr> elist
%type <expr> callsuffix
%type <expr> normcall
%type <expr> call
%type <expr> elist_expressions
%type <expr> objectdef
%type <expr> indexedelem_list
%type <expr> indexedelem
%type <expr> indexed
%type <expr> ifprefix
%type <expr> elseprefix
%type <expr> whilestart

%%

program: stmts { printf("[PROGRAM] found stmts at line %d\n", yylineno); } | ;

stmts:
    stmt { printf("[STMTS] found stmt at line %d\n", yylineno); }
    | stmts stmt { printf("[STMTS] found stmts at line %d\n", yylineno); }
    |
    ;

stmt:
    expr SEMICOLON { printf("[STMT] found expr; at line %d\n", yylineno); }
    | ifstmt { printf("[STMT] found ifstmt at line %d\n", yylineno); }
    | whilestmt { printf("[STMT] found whilestmt at line %d\n", yylineno); }
    | forstmt { printf("[STMT] found forstmt at line %d\n", yylineno); }
    | returnstmt { printf("[STMT] found returnstmt at line %d\n", yylineno); }
    | BREAK {
        if (forScopeCount == 0 && whileScopeCount == 0) {
            yyerror("Break must be used inside a loop");
        }
    } SEMICOLON { printf("[STMT] found break; at line %d\n", yylineno); }
    | CONTINUE {
        if (forScopeCount == 0 && whileScopeCount == 0) {
            yyerror("Break must be used inside a loop");
        }
    } SEMICOLON { printf("[STMT] found continue; at line %d\n", yylineno); }
    | block { printf("[STMT] found block at line %d\n", yylineno); }
    | funcdef { printf("[STMT] found funcdef at line %d\n", yylineno); }
    | SEMICOLON { printf("[STMT] found ; at line %d\n", yylineno); }
    ;

expr:
    assignexpr { $$ = $1; printf("[EXPR] found assignexpr at line %d\n", yylineno); }
    | expr PLUS expr {
        $$ = quads->newExpr(arithexpr_e);
        quads->emit(add_op, $$, $1, $3, 0, yylineno);
        printf("[EXPR] found expr + expr at line %d\n", yylineno);
    }
    | expr MINUS expr {
        $$ = quads->newExpr(arithexpr_e);
        quads->emit(uminus_op, $$, $1, $3, 0, yylineno);
        printf("[EXPR] found expr - expr at line %d\n", yylineno);
    }
    | expr MULT expr {
        $$ = quads->newExpr(arithexpr_e);
        quads->emit(mul_op, $$, $1, $3, 0, yylineno);
        printf("[EXPR] found expr * expr at line %d\n", yylineno);
    }
    | expr DIV expr {
        $$ = quads->newExpr(arithexpr_e);
        quads->emit(div_op, $$, $1, $3, 0, yylineno);
        printf("[EXPR] found expr / expr at line %d\n", yylineno);
    }
    | expr MOD expr {
        $$ = quads->newExpr(arithexpr_e);
        quads->emit(mod_op, $$, $1, $3, 0, yylineno);
        printf("[EXPR] found expr % expr at line %d\n", yylineno);
    }
    | expr GT expr  {
        $$ = quads->newExpr(boolexpr_e);
        quads->emit(if_greater_op, $$, $1, $3, 0, yylineno);
        printf("[EXPR] found expr > expr at line %d\n", yylineno);
    }
    | expr GE expr {
        $$ = quads->newExpr(boolexpr_e);
        quads->emit(if_greatereq_op, $$, $1, $3, 0, yylineno);
        printf("[EXPR] found expr >= expr at line %d\n", yylineno);
    }
    | expr LT expr {
        $$ = quads->newExpr(boolexpr_e);
        quads->emit(if_less_op, $$, $1, $3, 0, yylineno);
        printf("[EXPR] found expr < expr at line %d\n", yylineno);
    }
    | expr LE expr {
        $$ = quads->newExpr(boolexpr_e);
        quads->emit(if_lesseq_op, $$, $1, $3, 0, yylineno);
        printf("[EXPR] found expr <= expr at line %d\n", yylineno);
    }
    | expr EQUAL expr {
        $$ = quads->newExpr(boolexpr_e);
        quads->emit(if_eq_op, $$, $1, $3, 0, yylineno);
        printf("[EXPR] found expr == expr at line %d\n", yylineno);
    }
    | expr DIFF expr {
        $$ = quads->newExpr(boolexpr_e);
        quads->emit(if_noteq_op, $$, $1, $3, 0, yylineno);
        printf("[EXPR] found expr != expr at line %d\n", yylineno);
    }
    | expr AND expr {
        $$ = quads->newExpr(boolexpr_e);
        quads->emit(and_op, $$, $1, $3, 0, yylineno);
        printf("[EXPR] found expr AND expr at line %d\n", yylineno);
    }
    | expr OR expr {
        $$ = quads->newExpr(boolexpr_e);
        quads->emit(or_op, $$, $1, $3, 0, yylineno);
        printf("[EXPR] found expr OR expr at line %d\n", yylineno);
    }
    | term { $$ = $1; printf("[EXPR] found term at line %d\n", yylineno); }
    ;

assignexpr:
    lvalue ASSIGN expr {
        $$ = quads->newExpr(assignexpr_e);
        quads->emit(assign_op, $1, $3, nullptr, 0, yylineno);
        insertToken($1->symbol, false);
        printf("[ASSIGNEXPR] found lvalue = expr at line %d\n", yylineno);
    }
    ;

term:
    PAREN_OPEN expr PAREN_CLOSE { $$ = $2; printf("[TERM] found (expr) at line %d\n", yylineno); }
    | MINUS expr %prec UMINUS { printf("[TERM] found -expr at line %d\n", yylineno); }
    | NOT expr { printf("[TERM] found !expr at line %d\n", yylineno); }
    | INC lvalue { printf("[TERM] found ++lvalue at line %d\n", yylineno); }
    | lvalue INC { printf("[TERM] found lvalue++ at line %d\n", yylineno); }
    | DEC lvalue { printf("[TERM] found --lvalue at line %d\n", yylineno); }
    | lvalue DEC { printf("[TERM] found lvalue-- at line %d\n", yylineno); }
    | primary { $$ = $1; printf("[TERM] found primary at line %d\n", yylineno); }
    ;

primary:
    lvalue {
        $$ = $1;
        insertToken($1->symbol, true);
        printf("[PRIMARY] found lvalue at line %d\n", yylineno);
    }
    | call { printf("[PRIMARY] found call at line %d\n", yylineno); }
    | objectdef { printf("[PRIMARY] found objectdef at line %d\n", yylineno); }
    | PAREN_OPEN funcdef PAREN_CLOSE { printf("[PRIMARY] found (funcdef) at line %d\n", yylineno); }
    | const { $$ = $1; printf("[PRIMARY] found const at line %d\n", yylineno); }
    ;

const:
    INTEGER {
        $$ = quads->newExpr(constnum_e);
        $$->numConst = $1;
        printf("[CONST] found integer at line %d\n", yylineno);
    }
    | FLOAT { printf("[CONST] found float at line %d\n", yylineno); }
    | STRING { printf("[CONST] found string at line %d\n", yylineno); }
    | TRUE { printf("[CONST] found true at line %d\n", yylineno); }
    | FALSE { printf("[CONST] found false at line %d\n", yylineno); }
    | NIL { printf("[CONST] found nil at line %d\n", yylineno); }
    ;

lvalue:
    ID {
        SymbolStruct* symbolStruct = new SymbolStruct();
        symbolStruct->name = $1;
        symbolStruct->line = yylineno;
        symbolStruct->type = ASSIGNMENT;

	$$ = quads->newExpr(var_e);
        $$->symbol = symbolStruct;

        printf("[LVALUE] found ID at line %d\n", yylineno);
      }
    | LOCAL ID {
        SymbolStruct* symbolStruct = new SymbolStruct();
        symbolStruct->name = $2;
        symbolStruct->line = yylineno;
        symbolStruct->type = SCOPED;

	$$ = quads->newExpr(var_e);
        $$->symbol = symbolStruct;

        printf("[LVALUE] found LOCAL ID at line %d\n", yylineno);
    }
    | NAMESPACE ID {
        SymbolStruct* symbolStruct = new SymbolStruct();
        symbolStruct->name = $2;
        symbolStruct->line = yylineno;
        symbolStruct->type = GLOBAL;

	$$ = quads->newExpr(var_e);
        $$->symbol = symbolStruct;

        printf("[LVALUE] found NAMESPACE ID at line %d\n", yylineno);
    }
    | member {
        isMemberCall = true;

        printf("[LVALUE] found member at line %d\n", yylineno);
    }
    ;

member:
    lvalue DOT ID { printf("[MEMBER] found lvalue.ID at line %d\n", yylineno); }
    | lvalue BRACKET_OPEN expr BRACKET_CLOSE { printf("[MEMBER] found lvalue[expr] at line %d\n", yylineno); }
    | call DOT ID { printf("[MEMBER] found call.ID at line %d\n", yylineno); }
    | call BRACKET_OPEN expr BRACKET_CLOSE { printf("[MEMBER] found call[expr] at line %d\n", yylineno); }
    ;

call:
    call PAREN_OPEN elist PAREN_CLOSE { printf("[CALL] found call(elist) at line %d\n", yylineno); }
    | lvalue callsuffix {
        Symbol* existingSymbol = symbolTable->lookupSymbol($1->symbol->name);

        if (isMethodCall) {
            isMethodCall = false;
        } else if (existingSymbol == nullptr) {
            char message[100];
            sprintf(message, "Function %s not defined", $1->symbol->name);
            yyerror(message);
        }

        printf("[CALL] found lvalue callsufix at line %d\n", yylineno);
    }
    | PAREN_OPEN funcdef PAREN_CLOSE PAREN_OPEN elist PAREN_CLOSE { printf("[CALL] found (funcdef)(elist) at line %d\n", yylineno); }
    ;

callsuffix:
    normcall { printf("[CALLSUFFIX] found normcall at line %d\n", yylineno); }
    | methodcall { isMethodCall = true; printf("[CALLSUFFIX] found methodcall at line %d\n", yylineno);}
    ;

normcall:
    PAREN_OPEN elist PAREN_CLOSE { printf("[NORMCALL] found (elist) at line %d\n", yylineno); }
    ;

methodcall:
    DOUBLE_DOT ID PAREN_OPEN elist PAREN_CLOSE { printf("[METHODCALL] found ..ID(elist) at line %d\n", yylineno); }
    ;

elist:
    expr { printf("[ELIST] found expr at line %d\n", yylineno); }
    | elist COMMA expr { printf("[ELIST] found elist, expr at line %d\n", yylineno); }
    |
    ;

objectdef:
    BRACKET_OPEN elist BRACKET_CLOSE { printf("[OBJECTDEF] found [elist] at line %d\n", yylineno); }
    | BRACKET_OPEN indexed BRACKET_CLOSE { printf("[OBJECTDEF] found [indexed] at line %d\n", yylineno); }
    ;

indexed:
    indexedelem { printf("[INDEXED] found indexedelem at line %d\n", yylineno); }
    | indexed COMMA indexedelem { printf("[INDEXED] found indexed, indexedelem at line %d\n", yylineno); }
    ;

indexedelem:
    CURLY_OPEN expr COLON expr CURLY_CLOSE { printf("[INDEXEDELEM] found {expr:expr} at line %d\n", yylineno); }
    ;

block:
    CURLY_OPEN { if (!isScopeIncreasedByFunction) {symbolTable->incScope();} else { isScopeIncreasedByFunction = false; } } stmts CURLY_CLOSE { symbolTable->decScope(); printf("[BLOCK] found {stmts} at line %d\n", yylineno); }
    ;

funcdef:
    FUNCTION { functionScopeCount++; } PAREN_OPEN {
        symbolTable->insertSymbol("$" + std::to_string(functionCount++), yylineno, true, false, functionScopeCount);
        isScopeIncreasedByFunction = true; symbolTable->incScope();
    } idlist PAREN_CLOSE block { functionScopeCount--; } { printf("[FUNCDEF] found function(idlist){} at line %d\n", yylineno); }
    | FUNCTION { functionScopeCount++; isFunction = true; } ID
     {
        Symbol* symbol = symbolTable->lookupSymbolScoped($3);

        if (symbolTable->isNameReserved($3)) {
            char message[100];
            sprintf(message, "Reserved name %s cannot be used", $3);
            yyerror(message);
        }

        if (symbol == nullptr) {
           symbol = symbolTable->insertSymbol($3, yylineno, isFunction, false, functionScopeCount);
        }

        isFunction = false;
     }
     PAREN_OPEN { isScopeIncreasedByFunction = true; symbolTable->incScope(); } idlist PAREN_CLOSE block { functionScopeCount--; } { printf("[FUNCDEF] found function(idlist){} at line %d\n", yylineno); }
    ;

idlist:
    ID {
        Symbol* symbol = symbolTable->lookupSymbolScoped($1);

         if (symbol == nullptr) {
             symbol = symbolTable->insertSymbol($1, yylineno, false, true, functionScopeCount);
         } else if (symbol->getScope() == 0) {
            char message[100];
             sprintf(message, "%s cannot be redefined", $1);
             yyerror(message);
         } else if (symbol->getFunctionScope() == functionScopeCount) {
             char message[100];
             sprintf(message, "%s cannot be redefined", $1);
             yyerror(message);
         }

         printf("[IDLIST] found ID at line %d\n", yylineno);
    }
    | idlist COMMA ID {
        Symbol* symbol = symbolTable->lookupSymbolScoped($3);

         if (symbol == nullptr) {
             symbol = symbolTable->insertSymbol($3, yylineno, false, true, functionScopeCount);
         } else if (symbol->getScope() == 0) {
            char message[100];
             sprintf(message, "%s cannot be redefined", $3);
             yyerror(message);
         } else if (symbol->getFunctionScope() == functionScopeCount) {
             char message[100];
             sprintf(message, "%s cannot be redefined", $3);
             yyerror(message);
         }

        printf("[IDLIST] found idlist, ID at line %d\n", yylineno);
    }
    |
    ;

ifstmt:
    IF PAREN_OPEN expr PAREN_CLOSE stmts { printf("[IFSTMT] found if (expr) stmts at line %d\n", yylineno); }
    | IF PAREN_OPEN expr PAREN_CLOSE stmts ELSE stmts { printf("[IFSTMT] found if (expr) stmts else stmts at line %d\n", yylineno); }
    ;

whilestmt:
    WHILE { whileScopeCount++; } PAREN_OPEN expr PAREN_CLOSE stmts { whileScopeCount--; printf("[WHILESTMT] found while (expr) stmts at line %d\n", yylineno);}
    ;

forstmt:
    FOR { forScopeCount++; } PAREN_OPEN elist SEMICOLON expr SEMICOLON elist PAREN_CLOSE stmts { forScopeCount--; printf("[FORSTMT] found for (elist; expr; elist) stmts at line %d\n", yylineno);}
    ;

returnstmt:
    RETURN { if (functionScopeCount == 0) { yyerror("Return must be used inside a function"); } } expr SEMICOLON { printf("[RETURNSTMT] found return expr; at line %d\n", yylineno); }
    | RETURN SEMICOLON { printf("[RETURNSTMT] found return; at line %d\n", yylineno); }
    ;

%%

int alpha_yyerror (const char* yaccProvidedMessage) {
    std::stringstream ss;

    ss << "Error at line " << yylineno << ": " << yaccProvidedMessage << "\n";

    errors.push_back(strdup(ss.str().c_str()));

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

    for (auto error : errors) {
        fprintf(stderr, "%s", error);
    }

    quads->printQuads();

    return 0;
}
