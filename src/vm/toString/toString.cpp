//
// Created by mikegriniezakis on 28/5/2024.
//

#include "toString.h"

#include <cassert>
#include <cstring>

char* avm_to_string(avm_memcell* m) {
    assert(m->type >= 0 && m->type <= undef_m);
    return (*toStringFuncs[m->type])(m);
}

extern char* number_tostring(avm_memcell* m) {
    char output[18];
    sprintf(output,"%G", m->data.numVal);
    return strdup(output);
}

extern char* string_tostring(avm_memcell* m) {
    return strdup(m->data.strVal);
}

extern char* bool_tostring(avm_memcell* m) {
    return const_cast<char*>(m->data.boolVal ? "TRUE" : "FALSE");
}

extern char* table_tostring(avm_memcell* m) {
    return "";
}

extern char* userfunc_tostring(avm_memcell* m) {
    return "";
}

extern char* libfunc_tostring(avm_memcell* m) {
    return "";
}

extern char* nil_tostring(avm_memcell* m) {
    return "";
}

extern char* undef_tostring(avm_memcell* m) {
    return "undefined";
}
