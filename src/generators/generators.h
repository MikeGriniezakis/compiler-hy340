//
// Created by mikegriniezakis on 25/5/2024.
//

#ifndef GENERATORS_H
#define GENERATORS_H
#include "src/quads/Quads.h"
#include "src/targetCode/targetCode.h"

inline unsigned currentQuad = 0;

extern void generate_ADD(Quad* quad, VirtualMachine* vm);

extern void generate_SUB(Quad* quad, VirtualMachine* vm);

extern void generate_MUL(Quad* quad, VirtualMachine* vm);

extern void generate_DIV(Quad* quad, VirtualMachine* vm);

extern void generate_MOD(Quad* quad, VirtualMachine* vm);

extern void generate_NEWTABLE(Quad* quad, VirtualMachine* vm);

extern void generate_TABLEGETELEM(Quad* quad, VirtualMachine* vm);

extern void generate_TABLESETELEM(Quad* quad, VirtualMachine* vm);

extern void generate_ASSIGN(Quad* quad, VirtualMachine* vm);

extern void generate_NOP(Quad* quad, VirtualMachine* vm);

extern void generate_JUMP(Quad* quad, VirtualMachine* vm);

extern void generate_IF_EQ(Quad* quad, VirtualMachine* vm);

extern void generate_IF_NOTEQ(Quad* quad, VirtualMachine* vm);

extern void generate_IF_GREATER(Quad* quad, VirtualMachine* vm);

extern void generate_IF_GREATEREQ(Quad* quad, VirtualMachine* vm);

extern void generate_IF_LESS(Quad* quad, VirtualMachine* vm);

extern void generate_IF_LESSEQ(Quad* quad, VirtualMachine* vm);

extern void generate_NOT(Quad* quad, VirtualMachine* vm);

extern void generate_OR(Quad* quad, VirtualMachine* vm);

extern void generate_PARAM(Quad* quad, VirtualMachine* vm);

extern void generate_CALL(Quad* quad, VirtualMachine* vm);

extern void generate_GETRETVAL(Quad* quad, VirtualMachine* vm);

extern void generate_FUNCSTART(Quad* quad, VirtualMachine* vm);

extern void generate_RETURN(Quad* quad, VirtualMachine* vm);

extern void generate_FUNCEND(Quad* quad, VirtualMachine* vm);

extern void generate_UMINUS(Quad* quad, VirtualMachine* vm);

extern void generate_AND(Quad* quad, VirtualMachine* vm);

extern void generate(Quad* quad, VirtualMachine* vm);

typedef void (*generator_func_t)(Quad* quad, VirtualMachine* vm);

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
