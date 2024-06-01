//
// Created by mikegriniezakis on 28/5/2024.
//

#ifndef TOSTRING_H
#define TOSTRING_H
#include "src/vm/vm.h"

typedef char* (*tostring_func_t)(avm_memcell*);

extern char* number_tostring(avm_memcell* m);

extern char* string_tostring(avm_memcell* m);

extern char* bool_tostring(avm_memcell* m);

extern char* table_tostring(avm_memcell* m);

extern char* userfunc_tostring(avm_memcell* m);

extern char* libfunc_tostring(avm_memcell* m);

extern char* nil_tostring(avm_memcell* m);

extern char* undef_tostring(avm_memcell* m);

inline tostring_func_t toStringFuncs[] = {
    number_tostring,
    string_tostring,
    bool_tostring,
    table_tostring,
    userfunc_tostring,
    libfunc_tostring,
    nil_tostring,
    undef_tostring
};

extern char* avm_to_string(avm_memcell* m);

inline char* typeString[] = {
    "number",
    "string",
    "bool",
    "table",
    "userfunc",
    "libfunc",
    "nil",
    "undef"
};

#endif //TOSTRING_H
