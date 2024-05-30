//
// Created by mikegriniezakis on 28/5/2024.
//

#ifndef MEMORY_H
#define MEMORY_H
#include "src/vm/vm.h"

#define AVM_WIPEOUT(m) memset(&(m), 0, sizeof(m))

typedef void(*memclear_func_t)(struct avm_memcell*);
extern void avm_memcellclear(avm_memcell * mc);
extern void memclear_string(avm_memcell * mc);
extern void memclear_table(avm_memcell * mc);
extern void memclear_libfunc(avm_memcell * mc);

extern avm_table* avm_table_new();
extern void avm_table_destroy(avm_table* t);
extern void avm_table_inc_ref_counter(avm_table* table);
extern void avm_table_dec_ref_counter(avm_table* table);
extern void avm_table_buckets_init(avm_table_bucket** p);
extern void avm_table_bucket_clear(avm_table_bucket** p);

inline memclear_func_t memclearFuncs[]={
    nullptr,  // nunder_m
    memclear_string,  // string_m
    nullptr,  // bool_m
    memclear_table,
    nullptr,  // userfunc_m
    memclear_libfunc,
    nullptr,  //nil_m
    nullptr   //undef_m
};

#endif //MEMORY_H
