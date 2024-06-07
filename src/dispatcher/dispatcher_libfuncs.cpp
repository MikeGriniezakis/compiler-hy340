//
// Created by mikegriniezakis on 1/6/2024.
//

#include <cstring>
#include <sstream>

#include "dispatcher.h"
#include "src/vm/memory/memory.h"
#include "src/vm/toString/toString.h"

extern void libfunc_print() {
    unsigned n = avm_totalactuals();

    unsigned i = 0;
    while (true) {
        char* s = avm_to_string(avm_getactual(i));

        bool escaped = false;
        for (int j = 0; j < strlen(s); j++) {
            char c = s[j];
            if (c == '\\') {
                escaped = true;
                if (j + 1 >= strlen(s)) {
                    printf("\\");
                    break;
                }
                char next = s[j + 1];
                if (next == 'n') {
                    printf("\n");
                } else if (next == 't') {
                    printf("\t");
                } else if (next == '"') {
                    printf("\"");
                }
            } else {
                if (!escaped && c != '"')
                    printf("%c", c);
                escaped = false;
            }
        }
        free(s);

        if (i == n - 1) {
            break;
        }
        i++;
    }
}

extern void libfunc_typeof() {
    unsigned n = avm_totalactuals();
    if (n != 1) {
        // avm_error("one argument (not %d) expected in 'typeof'!", n);
        fprintf(stderr, "one argument (not %d) expected in 'typeof'!", n);
        executionFinished = true;
    } else {
        avm_memcellclear(&retval);
        retval.type = string_m;
        retval.data.strVal = strdup(typeString[avm_getactual(0)->type]);
    }
}

extern void libfunc_totalarguments() {
    unsigned p_topsp = avm_get_env_value(topsp + AVM_SAVEDTOPSP_OFFSET);
    avm_memcellclear(&retval);

    if (!p_topsp) {
        fprintf(stderr, "'totalarguments' called outside a function!");
        retval.type = nil_m;
        return;
    }

    retval.type = number_m;
    retval.data.numVal = avm_get_env_value(p_topsp + AVM_NUMACTUALS_OFFSET);
}

extern void libfunc_argument() {
    unsigned prev_topsp = avm_get_env_value(topsp + AVM_SAVEDTOPSP_OFFSET);
    avm_memcellclear(&retval);

    if (!prev_topsp) {
        fprintf(stderr, "'argument' called outside of function!");
        retval.type = nil_m;
        return;
    }
    avm_memcell* index = avm_getactual(0);
    unsigned total_actuals = avm_get_env_value(prev_topsp + AVM_NUMACTUALS_OFFSET);
    if (index->type != number_m)
        fprintf(stderr, "argument index is not a number!");
    else if (index->data.numVal < 0 || index->data.numVal > total_actuals)
        fprintf(stderr, "argument index out of bounds!");
    else {
        unsigned offset = prev_topsp + AVM_STACKENV_SIZE + 1 + static_cast<int>(index->data.numVal);

        retval.type = avm_stack[offset].type;
        switch (retval.type) {
            case number_m:
                retval.data.numVal = avm_stack[offset].data.numVal;
                break;
            case string_m:
                retval.data.strVal = avm_stack[offset].data.strVal;
                break;
            case bool_m:
                retval.data.boolVal = avm_stack[offset].data.boolVal;
                break;
            case table_m:
                retval.data.tableVal = avm_stack[offset].data.tableVal;
                break;
            case userfunc_m:
                retval.data.funcVal = avm_stack[offset].data.funcVal;
                break;
            case libfunc_m:
                retval.data.libfuncVal = avm_stack[offset].data.libfuncVal;
                break;
        }
    }
}

extern void avm_registerlibfunc(char* id, library_func_t addr) {
    unsigned size = libFuncs.size() + 1;
    if (avm_stack[AVM_STACKSIZE - size].type == undef_m) {
        avm_stack[AVM_STACKSIZE - size].type = libfunc_m;
        avm_stack[AVM_STACKSIZE - size].data.libfuncVal = strdup(id);
        libFuncs[id] = addr;
    }
}
