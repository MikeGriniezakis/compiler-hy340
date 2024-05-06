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
    Quads* quads = new Quads(symbolTable);

    int functionCount = 0;
    int functionScopeCount = 0;
    int forScopeCount = 0;
    int whileScopeCount = 0;

    bool isMethodCall = false;
    bool isMemberCall = false;
    bool isFunction = false;
    bool isScopeIncreasedByFunction = false;

    std::vector<char*> errors;
    std::vector<int> offsets = {0};

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
                symbolTable->insertSymbol(symbol->name, symbol->line, isFunction, false, functionScopeCount, offsets[symbolTable->getScope()]++);
                isFunction = false;
            }
        }


        if (symbol->type == ASSIGNMENT) {
            if (existingSymbol == nullptr) {
                symbolTable->insertSymbol(symbol->name, symbol->line, isFunction, false, functionScopeCount, offsets[symbolTable->getScope()]++);
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
%type <expr> returnstmt

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
        if (!quads->checkArithmeticExpression($1, $3)) {
            yyerror("Arithmetic expression must be of the same type");
            return -1;
        }
        $$ = quads->newExpr(arithexpr_e);
        $$->symbol = quads->createTemp(offsets[symbolTable->getScope()]++);
        quads->emit(add_op, $$, $1, $3, 0, yylineno);
        printf("[EXPR] found expr + expr at line %d\n", yylineno);
    }
    | expr MINUS expr {
        if (!quads->checkArithmeticExpression($1, $3)) {
            yyerror("Arithmetic expression must be of the same type");
            return -1;
        }
        $$ = quads->newExpr(arithexpr_e);
        $$->symbol = quads->createTemp(offsets[symbolTable->getScope()]++);
        quads->emit(uminus_op, $$, $1, $3, 0, yylineno);
        printf("[EXPR] found expr - expr at line %d\n", yylineno);
    }
    | expr MULT expr {
        if (!quads->checkArithmeticExpression($1, $3)) {
            yyerror("Arithmetic expression must be of the same type");
            return -1;
        }
        $$ = quads->newExpr(arithexpr_e);
        $$->symbol = quads->createTemp(offsets[symbolTable->getScope()]++);
        quads->emit(mul_op, $$, $1, $3, 0, yylineno);
        printf("[EXPR] found expr * expr at line %d\n", yylineno);
    }
    | expr DIV expr {
        if (!quads->checkArithmeticExpression($1, $3)) {
            yyerror("Arithmetic expression must be of the same type");
            return -1;
        }
        $$ = quads->newExpr(arithexpr_e);
        $$->symbol = quads->createTemp(offsets[symbolTable->getScope()]++);
        quads->emit(div_op, $$, $1, $3, 0, yylineno);
        printf("[EXPR] found expr / expr at line %d\n", yylineno);
    }
    | expr MOD expr {
        if (!quads->checkArithmeticExpression($1, $3)) {
            yyerror("Arithmetic expression must be of the same type");
            return -1;
        }
        $$ = quads->newExpr(arithexpr_e);
        $$->symbol = quads->createTemp(offsets[symbolTable->getScope()]++);
        quads->emit(mod_op, $$, $1, $3, 0, yylineno);
        printf("[EXPR] found expr % expr at line %d\n", yylineno);
    }
    | expr GT expr  {
        if (!quads->checkArithmeticExpression($1, $3)) {
            yyerror("Arithmetic expression must be of the same type");
            return -1;
        }
        $$ = quads->newExpr(boolexpr_e);
        $$->symbol = quads->createTemp(offsets[symbolTable->getScope()]++);
        quads->emit(if_greater_op, $$, $1, $3, 0, yylineno);
        printf("[EXPR] found expr > expr at line %d\n", yylineno);
    }
    | expr GE expr {
        if (!quads->checkArithmeticExpression($1, $3)) {
            yyerror("Arithmetic expression must be of the same type");
            return -1;
        }
        $$ = quads->newExpr(boolexpr_e);
        $$->symbol = quads->createTemp(offsets[symbolTable->getScope()]++);
        quads->emit(if_greatereq_op, $$, $1, $3, 0, yylineno);
        printf("[EXPR] found expr >= expr at line %d\n", yylineno);
    }
    | expr LT expr {
        if (!quads->checkArithmeticExpression($1, $3)) {
            yyerror("Arithmetic expression must be of the same type");
            return -1;
        }
        $$ = quads->newExpr(boolexpr_e);
        $$->symbol = quads->createTemp(offsets[symbolTable->getScope()]++);
        quads->emit(if_less_op, $$, $1, $3, 0, yylineno);
        printf("[EXPR] found expr < expr at line %d\n", yylineno);
    }
    | expr LE expr {
        if (!quads->checkArithmeticExpression($1, $3)) {
            yyerror("Arithmetic expression must be of the same type");
            return -1;
        }
        $$ = quads->newExpr(boolexpr_e);
        $$->symbol = quads->createTemp(offsets[symbolTable->getScope()]++);
        quads->emit(if_lesseq_op, $$, $1, $3, 0, yylineno);
        printf("[EXPR] found expr <= expr at line %d\n", yylineno);
    }
    | expr EQUAL expr {
        if (!quads->checkArithmeticExpression($1, $3)) {
            yyerror("Arithmetic expression must be of the same type");
            return -1;
        }
        $$ = quads->newExpr(boolexpr_e);
        $$->symbol = quads->createTemp(offsets[symbolTable->getScope()]++);
        quads->emit(if_eq_op, $$, $1, $3, 0, yylineno);
        printf("[EXPR] found expr == expr at line %d\n", yylineno);
    }
    | expr DIFF expr {
        if (!quads->checkArithmeticExpression($1, $3)) {
            yyerror("Arithmetic expression must be of the same type");
            return -1;
        }
        $$ = quads->newExpr(boolexpr_e);
        $$->symbol = quads->createTemp(offsets[symbolTable->getScope()]++);
        quads->emit(if_noteq_op, $$, $1, $3, 0, yylineno);
        printf("[EXPR] found expr != expr at line %d\n", yylineno);
    }
    | expr AND expr {
        if (!quads->checkArithmeticExpression($1, $3)) {
            yyerror("Arithmetic expression must be of the same type");
            return -1;
        }
        $$ = quads->newExpr(boolexpr_e);
        $$->symbol = quads->createTemp(offsets[symbolTable->getScope()]++);
        quads->emit(and_op, $$, $1, $3, 0, yylineno);
        printf("[EXPR] found expr AND expr at line %d\n", yylineno);
    }
    | expr OR expr {
        if (!quads->checkArithmeticExpression($1, $3)) {
            yyerror("Arithmetic expression must be of the same type");
            return -1;
        }
        $$ = quads->newExpr(boolexpr_e);
        $$->symbol = quads->createTemp(offsets[symbolTable->getScope()]++);
        quads->emit(or_op, $$, $1, $3, 0, yylineno);
        printf("[EXPR] found expr OR expr at line %d\n", yylineno);
    }
    | term { $$ = $1; printf("[EXPR] found term at line %d\n", yylineno); }
    ;

assignexpr:
    lvalue ASSIGN expr {
        if ($1->type == tableitem_e) {
            quads->emit(tablesetelem_op, $1, $1->index, $3, 0, yylineno);

            $$ = quads->emitIfTableItem($1, yylineno, offsets[symbolTable->getScope()]++);
            $$->type = assignexpr_e;
        } else {
            quads->emit(assign_op, $1, $3, nullptr, 0, yylineno);
            $$ = quads->newExpr(assignexpr_e);
            $$->symbol = quads->createTemp(offsets[symbolTable->getScope()]++);
            quads->emit(assign_op, $$, $1, nullptr, 0, yylineno);
            insertToken($1->symbol, false);
        }
        printf("[ASSIGNEXPR] found lvalue = expr at line %d\n", yylineno);
    }
    ;

term:
    PAREN_OPEN expr PAREN_CLOSE { $$ = $2; printf("[TERM] found (expr) at line %d\n", yylineno); }
    | MINUS expr %prec UMINUS {
        quads->checkArithmeticExpression($2);
        $$ = quads->newExpr(arithexpr_e);
        $$->symbol = quads->createTemp(offsets[symbolTable->getScope()]++);
        quads->emit(uminus_op, $$, $2, nullptr, 0, yylineno);
        printf("[TERM] found -expr at line %d\n", yylineno);
    }
    | NOT expr {
        $$ = quads->newExpr(boolexpr_e);
        $$->symbol = quads->createTemp(offsets[symbolTable->getScope()]++);
        quads->emit(not_op, $$, $2, nullptr, 0, yylineno);
        printf("[TERM] found !expr at line %d\n", yylineno);
    }
    | INC lvalue {
        quads->checkArithmeticExpression($2);
        expr* addExpr = quads->newExpr(constnum_e);
        addExpr->numConst = 1;

        if ($2->type == tableitem_e) {
            expr* temp = quads->emitIfTableItem($2, yylineno, offsets[symbolTable->getScope()]++);
            quads->emit(add_op, temp, temp, addExpr, 0, yylineno);
            quads->emit(tablesetelem_op, $2, $2->index, temp, 0, yylineno);
        } else {
            quads->emit(add_op, $2, $2, addExpr, 0, yylineno);
            $$ = quads->newExpr(arithexpr_e);
            $$->symbol = quads->createTemp(offsets[symbolTable->getScope()]++);
            quads->emit(assign_op, $$, $2, nullptr, 0, yylineno);
        }

        printf("[TERM] found ++lvalue at line %d\n", yylineno);
    }
    | lvalue INC {
        quads->checkArithmeticExpression($1);
        $$ = quads->newExpr(var_e);
        $$->symbol = quads->createTemp(offsets[symbolTable->getScope()]++);

        expr* addExpr = quads->newExpr(constnum_e);
        addExpr->numConst = 1;

        if ($1->type == tableitem_e) {
            expr* temp = quads->emitIfTableItem($1, yylineno, offsets[symbolTable->getScope()]++);
            quads->emit(assign_op, $$, temp, nullptr, 0, yylineno);
            quads->emit(add_op, temp, temp, addExpr, 0, yylineno);
            quads->emit(tablesetelem_op, $1, $1->index, temp, 0, yylineno);
        } else {
            quads->emit(assign_op, $$, $1, nullptr, 0, yylineno);
            quads->emit(add_op, $1, $1, addExpr, 0, yylineno);
        }

        printf("[TERM] found lvalue++ at line %d\n", yylineno);
    }
    | DEC lvalue {
        quads->checkArithmeticExpression($2);
        expr* decExpr = quads->newExpr(constnum_e);
        decExpr->numConst = -1;

        if ($2->type == tableitem_e) {
            expr* temp = quads->emitIfTableItem($2, yylineno, offsets[symbolTable->getScope()]++);
            quads->emit(sub_op, temp, temp, decExpr, 0, yylineno);
            quads->emit(tablesetelem_op, $2, $2->index, temp, 0, yylineno);
        } else {
            quads->emit(sub_op, $2, $2, decExpr, 0, yylineno);
            $$ = quads->newExpr(arithexpr_e);
            $$->symbol = quads->createTemp(offsets[symbolTable->getScope()]++);
            quads->emit(assign_op, $$, $2, nullptr, 0, yylineno);
        }

        printf("[TERM] found --lvalue at line %d\n", yylineno);
    }
    | lvalue DEC {
        quads->checkArithmeticExpression($1);
        $$ = quads->newExpr(var_e);
        $$->symbol = quads->createTemp(offsets[symbolTable->getScope()]++);

        expr* decExpr = quads->newExpr(constnum_e);
        decExpr->numConst = -1;

        if ($1->type == tableitem_e) {
            expr* temp = quads->emitIfTableItem($1, yylineno, offsets[symbolTable->getScope()]++);
            quads->emit(assign_op, $$, temp, nullptr, 0, yylineno);
            quads->emit(sub_op, temp, temp, decExpr, 0, yylineno);
            quads->emit(tablesetelem_op, $1, $1->index, temp, 0, yylineno);
        } else {
            quads->emit(assign_op, $$, $1, nullptr, 0, yylineno);
            quads->emit(sub_op, $1, $1, decExpr, 0, yylineno);
        }

        printf("[TERM] found lvalue-- at line %d\n", yylineno);
    }
    | primary { $$ = $1; printf("[TERM] found primary at line %d\n", yylineno); }
    ;

primary:
    lvalue {
        $$ = quads->emitIfTableItem($1, yylineno, offsets[symbolTable->getScope()]++);
        insertToken($1->symbol, true);
        printf("[PRIMARY] found lvalue at line %d\n", yylineno);
    }
    | call { printf("[PRIMARY] found call at line %d\n", yylineno); }
    | objectdef { printf("[PRIMARY] found objectdef at line %d\n", yylineno); }
    | PAREN_OPEN funcdef PAREN_CLOSE {
        $$ = $2;
        printf("[PRIMARY] found (funcdef) at line %d\n", yylineno);
    }
    | const { $$ = $1; printf("[PRIMARY] found const at line %d\n", yylineno); }
    ;

const:
    INTEGER {
        $$ = quads->newExpr(constnum_e);
        $$->numConst = $1;
        printf("[CONST] found integer at line %d\n", yylineno);
    }
    | FLOAT {
        $$ = quads->newExpr(constnum_e);
        $$->numConst = $1;
        printf("[CONST] found float at line %d\n", yylineno);
    }
    | STRING {
        $$ = quads->newExpr(conststring_e);
        $$->strConst = $1;
        printf("[CONST] found string at line %d\n", yylineno);
    }
    | TRUE {
        $$ = quads->newExpr(constbool_e);
        $$->boolConst = false;
        printf("[CONST] found true at line %d\n", yylineno);
    }
    | FALSE {
        $$ = quads->newExpr(constbool_e);
        $$->boolConst = false;
        printf("[CONST] found false at line %d\n", yylineno);
    }
    | NIL {
        $$ = quads->newExpr(nil_e);
        printf("[CONST] found nil at line %d\n", yylineno);
    }
    ;

lvalue:
    ID {
        SymbolStruct* symbolStruct = new SymbolStruct();
        symbolStruct->name = $1;
        symbolStruct->line = yylineno;
        symbolStruct->type = ASSIGNMENT;


        Symbol* existingSymbol = symbolTable->lookupSymbol($1);
        $$ = quads->newExpr(var_e);

        if (existingSymbol == nullptr) {
            $$->symbol = symbolStruct;
        } else {
            $$->symbol = existingSymbol->toStruct();
        }

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
        $$ = $1;
        isMemberCall = true;

        printf("[LVALUE] found member at line %d\n", yylineno);
    }
    ;

member:
    lvalue DOT ID {
        $$ = quads->makeMember($1, $3, offsets[symbolTable->getScope()]++, yylineno);
        printf("[MEMBER] found lvalue.ID at line %d\n", yylineno);
    }
    | lvalue BRACKET_OPEN expr BRACKET_CLOSE {
        expr* temp = quads->emitIfTableItem($1, yylineno, offsets[symbolTable->getScope()]++);

        $$ = quads->newExpr(tableitem_e);
        $$->symbol = temp->symbol;
        $$->index = $3;
        printf("[MEMBER] found lvalue[expr] at line %d\n", yylineno);
    }
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
    DOUBLE_DOT ID PAREN_OPEN elist PAREN_CLOSE {
        printf("[METHODCALL] found ..ID(elist) at line %d\n", yylineno);
    }
    ;

elist:
    expr {
        $$ = $1;
        printf("[ELIST] found expr at line %d\n", yylineno);
    }
    | expr COMMA elist {
        $$ = $1;
        $$->next = $3;
        printf("[ELIST] found elist, expr at line %d\n", yylineno);
    }
    | { $$ = nullptr; }
    ;

objectdef:
    BRACKET_OPEN elist BRACKET_CLOSE {
        $$ = quads->newExpr(newtable_e);
        $$->symbol = quads->createTemp(offsets[symbolTable->getScope()]++);

        quads->emit(tablecreate_op, $$, nullptr, nullptr, 0, yylineno);
        for (int i = 0; $elist; $elist = $elist->next) {
            expr* elist_expr = quads->newExpr(constnum_e);
            elist_expr->numConst = i++;
            quads->emit(tablesetelem_op, $$, elist_expr, $elist, 0, yylineno);
        }

        printf("[OBJECTDEF] found [elist] at line %d\n", yylineno);
    }
    | BRACKET_OPEN indexed BRACKET_CLOSE {
        $$ = quads->newExpr(newtable_e);
        $$->symbol = quads->createTemp(offsets[symbolTable->getScope()]++);

        quads->emit(tablecreate_op, $$, nullptr, nullptr, 0, yylineno);
        expr* index = $indexed;
        while (true) {
            if (index == nullptr) {
                break;
            }

            quads->emit(tablesetelem_op, $$, index->index, index, 0, yylineno);

            index = index->next;
        }

        printf("[OBJECTDEF] found [indexed] at line %d\n", yylineno);
    }
    ;

indexed:
    indexedelem {
        $$ = $1;
        printf("[INDEXED] found indexedelem at line %d\n", yylineno);
    }
    | indexedelem COMMA indexed {
        $$ = $1;
        $$->next = $3;
        printf("[INDEXED] found indexed, indexedelem at line %d\n", yylineno);
    }
    | { $$ = nullptr; }
    ;

indexedelem:
    CURLY_OPEN expr COLON expr CURLY_CLOSE {
        $$ = $4;
        $$->index = $2;
        printf("[INDEXEDELEM] found {expr:expr} at line %d\n", yylineno);
    }
    ;

block:
    CURLY_OPEN { if (!isScopeIncreasedByFunction) {
        symbolTable->incScope();
        if (offsets.size()-1 < symbolTable->getScope()) {
            offsets.push_back(0);
        }
    } else { isScopeIncreasedByFunction = false; } } stmts CURLY_CLOSE {
        symbolTable->decScope();
        offsets.at(symbolTable->getScope() + 1) = 0;
        printf("[BLOCK] found {stmts} at line %d\n", yylineno);
    }
    ;

funcdef:
    FUNCTION { functionScopeCount++; } PAREN_OPEN {
        Symbol* symbol = symbolTable->insertSymbol("$" + std::to_string(functionCount++), yylineno, true, false, functionScopeCount, offsets[symbolTable->getScope()]++);
        $<expr>$ = quads->newExpr(libraryfunc_e);
        $<expr>$->symbol = symbol->toStruct();
        quads->emit(funcstart_op, $<expr>$, nullptr, nullptr, 0, yylineno);
        isScopeIncreasedByFunction = true;
        symbolTable->incScope();
        if (offsets.size()-1 < symbolTable->getScope()) {
            offsets.push_back(0);
        }
    } idlist PAREN_CLOSE block {
        quads->emit(funcend_op, $<expr>4, nullptr, nullptr, 0, yylineno);
        functionScopeCount--;
    } { printf("[FUNCDEF] found function(idlist){} at line %d\n", yylineno); $$ = $<expr>4; }
    | FUNCTION { functionScopeCount++; isFunction = true; } ID
     {
         Symbol* symbol = symbolTable->lookupSymbolScoped($3);

         if (symbolTable->isNameReserved($3)) {
             char message[100];
             sprintf(message, "Reserved name %s cannot be used", $3);
             yyerror(message);
         }

         if (symbol == nullptr) {
             symbol = symbolTable->insertSymbol($3, yylineno, isFunction, false, functionScopeCount, offsets[symbolTable->getScope()]++);
         }
         $<expr>$ = quads->newExpr(libraryfunc_e);
         $<expr>$->symbol = symbol->toStruct();
         quads->emit(funcstart_op, $<expr>$, nullptr, nullptr, 0, yylineno);

         isFunction = false;
     }
     PAREN_OPEN {
         isScopeIncreasedByFunction = true;
         symbolTable->incScope();
         if (offsets.size()-1 < symbolTable->getScope()) {
             offsets.push_back(0);
         }
     } idlist PAREN_CLOSE block {
         functionScopeCount--;
         quads->emit(funcend_op, $<expr>4, nullptr, nullptr, 0, yylineno);
     } { printf("[FUNCDEF] found function(idlist){} at line %d\n", yylineno); $$ = $<expr>4; }
    ;

idlist:
    ID {
        Symbol* symbol = symbolTable->lookupSymbolScoped($1);

         if (symbol == nullptr) {
             symbol = symbolTable->insertSymbol($1, yylineno, false, true, functionScopeCount, offsets[symbolTable->getScope()]++);
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
             symbol = symbolTable->insertSymbol($3, yylineno, false, true, functionScopeCount, offsets[symbolTable->getScope()]++);
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
    RETURN { if (functionScopeCount == 0) { yyerror("Return must be used inside a function"); } } expr SEMICOLON {
        quads->emit(ret_op, nullptr, $3, nullptr, 0, yylineno);
        printf("[RETURNSTMT] found return expr; at line %d\n", yylineno);
    }
    | RETURN SEMICOLON { printf("[RETURNSTMT] found return; at line %d\n", yylineno); quads->emit(ret_op, nullptr, nullptr, nullptr, 0, yylineno); }
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
