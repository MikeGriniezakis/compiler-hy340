//
// Created by mikegriniezakis on 28/5/2024.
//

#include "memory.h"

#include <cassert>
#include <cstring>

#include "src/vm/vm.h"

extern void avm_memcellclear(avm_memcell* mc) {
    if (mc->type == undef_m) {
        return;
    }

    memclear_func_t f = memclearFuncs[mc->type];
    if (f)
        (*f)(mc);

    mc->type = undef_m;
}

extern void memclear_string(avm_memcell* mc) {
    free(mc->data.strVal);
}

extern void memclear_libfunc(avm_memcell* mc) {
    free(mc->data.libfuncVal);
}

extern void memclear_table(avm_memcell * mc) {
    assert(mc->data.tableVal);
    avm_table_dec_ref_counter(mc->data.tableVal);
}

extern void avm_table_inc_ref_counter(avm_table* table) {
    ++table->refCounter;
}

void avm_table_dec_ref_counter(avm_table* table) {
    assert(table->refCounter > 0);
    if (!--table->refCounter)
        avm_table_destroy(table);
}

extern avm_table* avm_table_new() {
    auto* t = (avm_table*)malloc(sizeof(avm_table));
    AVM_WIPEOUT(t);

    t->refCounter = 0;
    t->total = 0;
    avm_table_buckets_init(t->numIndexed);
    avm_table_buckets_init(t->strIndexed);

    return t;
}

extern void avm_table_buckets_init(avm_table_bucket** p) {
    for (unsigned i = 0; i < AVM_TABLE_HASHSIZE; ++i) {
        p[i] = (avm_table_bucket*)nullptr;
    }
}
extern void avm_table_bucket_clear(avm_table_bucket** p) {
    for (unsigned i = 0; i < AVM_TABLE_HASHSIZE; i++) {
        for (avm_table_bucket* b = *p; b;) {
            avm_table_bucket* del = b;
            b = b->next;
            avm_memcellclear(&del->key);
            avm_memcellclear(&del->value);
            free(del);
        }
        p[i] = (avm_table_bucket*)nullptr;
    }
}

void avm_table_destroy(avm_table* t) {
    avm_table_bucket_clear(t->numIndexed);
    avm_table_bucket_clear(t->strIndexed);
    free(t);
}
