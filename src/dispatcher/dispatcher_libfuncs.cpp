//
// Created by mikegriniezakis on 1/6/2024.
//

#include <cstring>

#include "dispatcher.h"
#include "src/vm/toString/toString.h"

extern void libfunc_print() {
    unsigned n = avm_totalactuals();

    for (unsigned i = 0; i < n; i++) {
        char* s = avm_to_string(avm_getactual(i));
        puts(s);
        free(s);
    }
}

extern void avm_registerlibfunc(char* id, library_func_t addr) {
    avm_stack[AVM_STACKSIZE - 1].type = libfunc_m;
    avm_stack[AVM_STACKSIZE - 1].data.libfuncVal = strdup(id);
    libFuncs[id] = addr;
}
