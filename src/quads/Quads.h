//
// Created by mikegriniezakis on 20/4/2024.
//

#ifndef QUADS_H
#define QUADS_H

#include <vector>

#include "Symbol.h"
#include "SymbolTable.h"

enum iopcode {
    add_op,
    sub_op,
    mul_op,
    div_op,
    mod_op,
    tablecreate_op,
    tablegetelem_op,
    tablesetelem_op,
    if_greater_op,
    if_greatereq_op,
    if_less_op,
    if_lesseq_op,
    not_op,
    or_op,
    param_op,
    call_op,
    getretval_op,
    funcstart_op,
    ret_op,
    funcend_op,
    assign_op,
    uminus_op,
    and_op,
    if_eq_op,
    if_noteq_op,
    jump_op
};

inline const std::string iopCodesLabels[] = {
        "add",
        "sub",
        "mul",
        "div",
        "mod",
        "tableCreate",
        "tableGetElem",
        "tableSetElem",
        "if_greater",
        "if_greatereq",
        "if_less",
        "if_lesseq",
        "not",
        "or",
        "param",
        "call",
        "getRetVal",
        "funcStart",
        "return",
        "funcEnd",
        "assign",
        "uminus",
        "and",
        "if_eq",
        "if_noteq",
        "jump"
};

enum expr_t {
    var_e,
    tableitem_e,
    programfunc_e,
    libraryfunc_e,
    arithexpr_e,
    boolexpr_e,
    assignexpr_e,
    newtable_e,
    constnum_e,
    constbool_e,
    conststring_e,
    nil_e
};

struct expr {
    expr_t type;
    expr* index;
    SymbolStruct* symbol;
    double numConst;
    char* strConst;
    bool boolConst;
    expr* next;
    std::vector<int> trueList;
    std::vector<int> falseList;
};

struct call {
    expr* elist;
    bool method;
    char* name;
};

struct forStatement {
    int enter;
    int test;
};

class Quad {
    iopcode code;
    expr *result;
    expr *arg1;
    expr *arg2;
    unsigned label;
    unsigned line;
    unsigned tAddress;
public:
    Quad(iopcode code, expr *result, expr *arg1, expr *arg2, unsigned label, unsigned line)
               : code(code), result(result), arg1(arg1), arg2(arg2), label(label), line(line) {}

    void print();

public:
    expr* getResult() { return result; }
    expr* getArg1() { return arg1; }
    expr* getArg2() { return arg2; }
    iopcode getCode() { return code; }
    unsigned getLabel() { return label; }
    unsigned getLine() { return line; }

    void setLabel(unsigned label);
    void setTAddress(unsigned tAddress);
    unsigned getTAddress();
};

class Quads {
    std::vector<Quad*> quads;
    unsigned tempCounter;
    SymbolTable* symbolTable;
public:
    Quads(SymbolTable* symbolTable) : tempCounter(0), symbolTable(symbolTable) {}

    std::vector<Quad*> getQuads();
    Quad* getQuad(unsigned quad);
    void resetTempCounter() { tempCounter = 0; }
    SymbolStruct* createTemp();
    void emit(iopcode code, expr *result, expr *arg1, expr *arg2, unsigned label, unsigned line);
    void emit(int code, expr *result, expr *arg1, expr *arg2, unsigned label, unsigned line);
    expr* emitIfTableItem(expr *result, unsigned line);
    void printQuads();
    expr* newExpr(expr_t type);
    expr* makeMember(expr* lvalue, char* name, unsigned line);
    expr* makeCall(expr* call, expr* elist, unsigned line);

    void patchLabel(unsigned quad, unsigned label);
    bool checkArithmeticExpression(const expr* first, const expr* second);
    bool checkArithmeticExpression(const expr* expr);
    unsigned nextQuad();
    void patchList(int list, int label);
    std::vector<int> merge(std::vector<int> list1, std::vector<int> list2);
};

#endif //QUADS_H
