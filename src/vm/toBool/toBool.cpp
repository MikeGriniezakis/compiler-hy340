//
// Created by mikegriniezakis on 1/6/2024.
//

#include "toBool.h"

#include <cassert>

#include "src/vm/toString/toString.h"

extern bool number_tobool(avm_memcell* m) {
    return m->data.numVal != 0;
}

extern bool string_tobool(avm_memcell* m) {
    return m->data.strVal[0] != 0;
}

extern bool bool_tobool(avm_memcell* m) {
    return m->data.boolVal;
}

extern bool table_tobool(avm_memcell* m) {
    return true;
}

extern bool userfunc_tobool(avm_memcell* m) {
    return true;
}

extern bool libfunc_tobool(avm_memcell* m) {
    return true;
}

extern bool nil_tobool(avm_memcell* m) {
    return false;
}

extern bool undef_tobool(avm_memcell* m) {
    return false;
}

extern bool avm_to_bool(avm_memcell* m) {
    assert(m->type >= 0 && m->type < undef_m);
    return (*toBoolFuncs[m->type])(m);
}
