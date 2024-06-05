//
// Created by mikegriniezakis on 28/5/2024.
//

#include "toString.h"

#include <cassert>
#include <cstring>
#include <sstream>

#include "src/dispatcher/dispatcher.h"

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
    return strdup(m->data.boolVal ? "TRUE" : "FALSE");
}

extern char* table_tostring(avm_memcell* m) {
    std::stringstream ss;
    for (unsigned i = 0; i < AVM_TABLE_HASHSIZE; i++) {
        avm_table_bucket* numStart = m->data.tableVal->numIndexed[i];
        avm_table_bucket* strStart = m->data.tableVal->strIndexed[i];

        while (numStart) {
            ss << "{key: " << numStart->key.data.numVal << ", value: " << avm_to_string(&numStart->value) << "}\n";
            numStart = numStart->next;
        }
        while (strStart) {
            ss << "{key: " << strStart->key.data.numVal << ", value: " << avm_to_string(&strStart->value) << "}\n";
            strStart = strStart->next;
        }
    }

    return strdup(ss.str().c_str());
}

extern char* userfunc_tostring(avm_memcell* m) {
    return strdup(userFuncs.at(m->data.funcVal)->id);
}

extern char* libfunc_tostring(avm_memcell* m) {
    return strdup(m->data.libfuncVal);
}

extern char* nil_tostring(avm_memcell* m) {
    return strdup("nil");
}

extern char* undef_tostring(avm_memcell* m) {
    return strdup("undefined");
}
