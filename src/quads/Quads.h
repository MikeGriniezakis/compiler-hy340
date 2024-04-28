//
// Created by mikegriniezakis on 20/4/2024.
//

#ifndef QUADS_H
#define QUADS_H

#include <vector>

#include "Symbol.h"

enum iopcode {
    assign_op, mul_op, uminus_op, not_op,
    if_lesseq_op, if_greater_op, ret_op, funcend_op,
    tablegetelem_op, add_op, div_op, and_op,
    if_eq_op, if_greatereq_op, call_op, getretval_op,
    tablecreate_op, tablesetelem_op, sub_op, mod_op,
    or_op, if_noteq_op, if_less_op, param_op, funcstart_op
};

inline std::string iopCodesLabels[] = {
    "assign", "mul", "uminus", "not",
    "if_lesseq", "if_greater", "ret", "funcend",
    "tablegetelem", "add", "div", "and",
    "if_eq", "if_greatereq", "call", "getretval",
    "tablecreate", "tablesetelem", "sub", "mod",
    "or", "if_noteq", "if_less", "param", "funcstart"
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
};

class Quad {
    iopcode code;
    expr *result;
    expr *arg1;
    expr *arg2;
    unsigned label;
    unsigned line;
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
};

class Quads {
    std::vector<Quad*> quads;
    unsigned tempCounter;
public:
    Quads() : tempCounter(0) {}

    SymbolStruct* createTemp();
    void emit(iopcode code, expr *result, expr *arg1, expr *arg2, unsigned label, unsigned line);
    void printQuads();
    expr* newExpr(expr_t type);
};

#endif //QUADS_H
