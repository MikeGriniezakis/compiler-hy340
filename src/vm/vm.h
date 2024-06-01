//
// Created by mikegriniezakis on 27/5/2024.
//

#ifndef VM_H
#define VM_H
#include "src/targetCode/targetCode.h"
#include <vector>

#define AVM_STACKSIZE 4096
#define AVM_STACKENV_SIZE 4
#define AVM_TABLE_HASHSIZE 211

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

struct avm_table;

struct avm_memcell {
    avm_memcell_t type;
    union {
        double numVal;
        char* strVal;
        bool boolVal;
        avm_table* tableVal;
        unsigned funcVal;
        char* libfuncVal;
    } data;
};

struct avm_table_bucket {
    avm_memcell key;
    avm_memcell value;
    avm_table_bucket* next;
};

struct avm_table {
    unsigned refCounter;
    avm_table_bucket* strIndexed[AVM_TABLE_HASHSIZE];
    avm_table_bucket* numIndexed[AVM_TABLE_HASHSIZE];
    unsigned total;
};


inline std::vector<avm_memcell> avm_stack = std::vector<avm_memcell>(AVM_STACKSIZE);
inline avm_memcell ax, bx, cx;
inline avm_memcell retval;
inline unsigned top, topsp;
inline std::vector<char *> stringConsts;
inline std::vector<double> numConsts;
inline std::vector<char *> namedLibfuncs;
inline std::vector<userfunc *> userFuncs;
inline std::vector<instruction *> instructions;
inline unsigned globalVarCount;


double consts_getnumber(unsigned index);
char* consts_getstring(unsigned index);
char* libfuncs_getused(unsigned index);

extern void avm_dec_top();

extern avm_memcell* avm_translate_operand(vmarg* arg, avm_memcell* reg);
extern avm_memcell* avm_table_get_elem(avm_table* table, avm_memcell* key);
extern avm_memcell* avm_table_set_elem(avm_table* table, avm_memcell* key, avm_memcell* value);

#endif //VM_H
