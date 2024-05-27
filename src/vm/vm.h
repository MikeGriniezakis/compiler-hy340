//
// Created by mikegriniezakis on 27/5/2024.
//

#ifndef VM_H
#define VM_H
#include "src/targetCode/targetCode.h"
#include <vector>

#define AVM_STACKSIZE 4096
#define AVM_STACKENV_SIZE 4

enum avm_memcell_t {
    number_m = 0,
    string_m = 1,
    bool_m = 2,
    table_m = 3,
    userfunc_m = 4,
    libfunc_m = 5,
    nil_m = 6,
    undef_m = 7
};

struct avm_memcell {
    avm_memcell_t type;
    union {
        double numVal;
        char* strVal;
        bool boolVal;
        // avm_table* tableVal;
        unsigned funcVal;
        char* libfuncVal;
    } data;
};

std::vector<avm_memcell> avm_stack = std::vector<avm_memcell>(AVM_STACKSIZE);
avm_memcell ax, bx, cx;
avm_memcell retval;
unsigned top, topsp;

double consts_getnumber(unsigned index);
char* consts_getstring(unsigned index);
char* libfuncs_getused(unsigned index);

extern avm_memcell* avm_translate_operand(vmarg* arg, avm_memcell* reg);

#endif //VM_H
