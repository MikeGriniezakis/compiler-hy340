//
// Created by mikegriniezakis on 25/5/2024.
//

#ifndef TARGET_CODE_H
#define TARGET_CODE_H
#include "src/quads/Quads.h"

enum vmopcode {
    assign_v,
    add_v,
    sub_v,
    mul_v,
    div_v,
    mod_v,
    uminus_v,
    and_v,
    or_v,
    not_v,
    jeq_v,
    jne_v,
    jle_v,
    jge_v,
    jlt_v,
    jgt_v,
    call_v,
    pusharg_v,
    funcenter_v,
    funcexit_v,
    newtable_v,
    tablegetelem_v,
    tablesetelem_v,
    nop_v,
    jump_v
};

inline std::string vmopcodeNames[] = {
    "assign",
    "add",
    "sub",
    "mul",
    "div",
    "mod",
    "uminus",
    "and",
    "or",
    "not",
    "jeq",
    "jne",
    "jle",
    "jge",
    "jlt",
    "jgt",
    "call",
    "pusharg",
    "funcenter",
    "funcexit",
    "newtable",
    "tablegetelem",
    "tablesetelem",
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
    bool empty;
};

struct instruction {
    vmopcode opcode;
    vmarg result;
    vmarg arg1;
    vmarg arg2;
    unsigned srcLine;
};

struct userfunc {
    unsigned address;
    unsigned localSize;
    unsigned scope;
    char* id;
};

struct inclomplete_jump {
    unsigned instrNo;
    unsigned iaddress;
    inclomplete_jump* next;
};

class VirtualMachine {
    Quads* quads;
    SymbolTable* symbolTable;
    std::vector<char *> stringConsts;
    std::vector<double> numConsts;
    std::vector<char *> namedLibfuncs;
    std::vector<instruction *> instructions;
    std::vector<userfunc *> userFuncs;
    std::vector<inclomplete_jump *> inclomplete_jumps;

public:
    explicit VirtualMachine(Quads* quads, SymbolTable* table): quads(quads), symbolTable(table) {
    };

    unsigned consts_newstring(char* str);

    unsigned libfuncs_newused(char* str);

    unsigned consts_newnumber(double num);

    unsigned userfuncs_newfunc(SymbolStruct* sym);

    userfunc* userfuncs_getfunc(bool pop);

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

    void printInstruction(std::stringstream* ss, instruction* instruction);

    void printVMArg(std::stringstream* ss, vmarg* vmarg);

    void print();

    void createBinaryFile();
};

#endif //TARGET_CODE_H
