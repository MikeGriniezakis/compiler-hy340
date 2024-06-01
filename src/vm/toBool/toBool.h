//
// Created by mikegriniezakis on 1/6/2024.
//

#ifndef TOBOOL_H
#define TOBOOL_H

#include <src/vm/vm.h>

typedef bool (*tobool_func_t)(avm_memcell*);

extern bool number_tobool(avm_memcell* m);
extern bool string_tobool(avm_memcell* m);
extern bool bool_tobool(avm_memcell* m);
extern bool table_tobool(avm_memcell* m);
extern bool userfunc_tobool(avm_memcell* m);
extern bool libfunc_tobool(avm_memcell* m);
extern bool nil_tobool(avm_memcell* m);
extern bool undef_tobool(avm_memcell* m);

inline tobool_func_t toBoolFuncs[] = {
    number_tobool,
    string_tobool,
    bool_tobool,
    table_tobool,
    userfunc_tobool,
    libfunc_tobool,
    nil_tobool,
    undef_tobool
};

extern bool avm_to_bool(avm_memcell* m);

#endif //TOBOOL_H
