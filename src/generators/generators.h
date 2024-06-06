//
// Created by mikegriniezakis on 25/5/2024.
//

#ifndef GENERATORS_H
#define GENERATORS_H
#include "src/quads/Quads.h"
#include "src/targetCode/targetCode.h"

inline unsigned currentQuad = 0;
inline bool inFuncStart = false;

extern void generate_ADD(Quad* quad, TargetCode* vm);

extern void generate_SUB(Quad* quad, TargetCode* vm);

extern void generate_MUL(Quad* quad, TargetCode* vm);

extern void generate_DIV(Quad* quad, TargetCode* vm);

extern void generate_MOD(Quad* quad, TargetCode* vm);

extern void generate_NEWTABLE(Quad* quad, TargetCode* vm);

extern void generate_TABLEGETELEM(Quad* quad, TargetCode* vm);

extern void generate_TABLESETELEM(Quad* quad, TargetCode* vm);

extern void generate_ASSIGN(Quad* quad, TargetCode* vm);

extern void generate_NOP(Quad* quad, TargetCode* vm);

extern void generate_JUMP(Quad* quad, TargetCode* vm);

extern void generate_IF_EQ(Quad* quad, TargetCode* vm);

extern void generate_IF_NOTEQ(Quad* quad, TargetCode* vm);

extern void generate_IF_GREATER(Quad* quad, TargetCode* vm);

extern void generate_IF_GREATEREQ(Quad* quad, TargetCode* vm);

extern void generate_IF_LESS(Quad* quad, TargetCode* vm);

extern void generate_IF_LESSEQ(Quad* quad, TargetCode* vm);

extern void generate_NOT(Quad* quad, TargetCode* vm);

extern void generate_OR(Quad* quad, TargetCode* vm);

extern void generate_PARAM(Quad* quad, TargetCode* vm);

extern void generate_CALL(Quad* quad, TargetCode* vm);

extern void generate_GETRETVAL(Quad* quad, TargetCode* vm);

extern void generate_FUNCSTART(Quad* quad, TargetCode* vm);

extern void generate_RETURN(Quad* quad, TargetCode* vm);

extern void generate_FUNCEND(Quad* quad, TargetCode* vm);

extern void generate_UMINUS(Quad* quad, TargetCode* vm);

extern void generate_AND(Quad* quad, TargetCode* vm);

extern void generate(Quad* quad, TargetCode* vm);

typedef void (*generator_func_t)(Quad* quad, TargetCode* vm);

inline generator_func_t generators[] = {
    generate_ADD,
    generate_SUB,
    generate_MUL,
    generate_DIV,
    generate_MOD,
    generate_NEWTABLE,
    generate_TABLEGETELEM,
    generate_TABLESETELEM,
    generate_IF_GREATER,
    generate_IF_GREATEREQ,
    generate_IF_LESS,
    generate_IF_LESSEQ,
    generate_NOT,
    generate_OR,
    generate_PARAM,
    generate_CALL,
    generate_GETRETVAL,
    generate_FUNCSTART,
    generate_RETURN,
    generate_FUNCEND,
    generate_ASSIGN,
    generate_UMINUS,
    generate_AND,
    generate_IF_EQ,
    generate_IF_NOTEQ,
    generate_JUMP
};

#endif //GENERATORS_H
