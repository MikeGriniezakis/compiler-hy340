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

        if (i == n-1) {
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

extern void avm_registerlibfunc(char* id, library_func_t addr) {
    unsigned i = 1;
    while (true) {
        if (avm_stack[AVM_STACKSIZE - i].type == undef_m) {
            avm_stack[AVM_STACKSIZE - 1].type = libfunc_m;
            avm_stack[AVM_STACKSIZE - 1].data.libfuncVal = strdup(id);
            libFuncs[id] = addr;
            break;
        }

        i++;
    }
}
