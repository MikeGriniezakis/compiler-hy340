//
// Created by mikegriniezakis on 1/6/2024.
//

#include <cstring>
#include <sstream>

#include "dispatcher.h"
#include "src/vm/toString/toString.h"

extern void libfunc_print() {
    unsigned n = avm_totalactuals();

    unsigned i = n-1;
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

        if (i == 0) {
            break;
        }
        i--;
    }
}

extern void avm_registerlibfunc(char* id, library_func_t addr) {
    avm_stack[AVM_STACKSIZE - 1].type = libfunc_m;
    avm_stack[AVM_STACKSIZE - 1].data.libfuncVal = strdup(id);
    libFuncs[id] = addr;
}
