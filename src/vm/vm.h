//
// Created by mikegriniezakis on 25/5/2024.
//

#ifndef VM_H
#define VM_H
#include "src/quads/Quads.h"

enum vmopcode {
    assign_v, mul_v, uminus_v, not_v, jle_v, jgt_v, funcenter_v, tablegetelem_v,
    add_v, div_v, and_v, jeq_v, jge_v, call_v, funcexit_v, tablesetelem_v,
    sub_v, mod_v, or_v, jne_v, jlt_v, pusharg_v, newtable_v, nop_v, jump_v
};

inline std::string vmopcodeNames[] = {
    "assign",
    "mul",
    "uminus",
    "not",
    "jle",
    "jgt",
    "funcenter",
    "tablegetelem",
    "add",
    "div",
    "and",
    "jeq",
    "jge",
    "call",
    "funcexit",
    "tablesetelem",
    "sub",
    "mod",
    "or",
    "jne",
    "jlt",
    "pusharg",
    "newtable",
    "nop",
    "jump"
};

enum vmarg_t {
    label_a = 0,
    global_a = 1,
    formal_a = 2,
    local_a = 3,
    number_a = 4,
    string_a = 5,
    bool_a = 6,
    nil_a = 7,
    userfunc_a = 8,
    libfunc_a = 9,
    retval_a = 10
};

struct vmarg {
    vmarg_t type;
    unsigned val;
};

struct instruction {
    vmopcode opcode;
    vmarg result;
    vmarg arg1;
    vmarg arg2;
    unsigned srcLine;
};

struct inclomplete_jump {
    unsigned instrNo;
    unsigned iaddress;
    inclomplete_jump* next;
};

class VirtualMachine {
    Quads* quads;
    std::vector<char *> stringConsts;
    std::vector<double> numConsts;
    std::vector<char *> namedLibfuncs;
    std::vector<instruction *> instructions;
    std::vector<SymbolStruct *> userFuncs;
    std::vector<inclomplete_jump *> inclomplete_jumps;

public:
    explicit VirtualMachine(Quads* quads): quads(quads) {
    };

    unsigned consts_newstring(char* str);

    unsigned libfuncs_newused(char* str);

    unsigned consts_newnumber(double num);

    unsigned userfuncs_newfunc(SymbolStruct* sym);

    SymbolStruct* userfuncs_getfunc(bool pop);

    void resetOperand(vmarg* arg);

    void makeOperand(expr* expr, vmarg* arg);

    void makeNumberOperand(vmarg* arg, double val);

    void makeBoolOperand(vmarg* arg, unsigned val);

    void makeRetvalOperand(vmarg* arg);

    void addIncompleteJump(unsigned istrNo, unsigned iaddress);

    void patchIncompleteJumps();

    void generate();

    unsigned getQuadTAddress(unsigned quadNo);

    unsigned getCurrentQuad();

    unsigned nextInstructionLabel();

    void emit(instruction* instruction);

    void printInstruction(instruction* instruction);

    void printVMArg(vmarg* vmarg);

    void print();
};

#endif //VM_H
