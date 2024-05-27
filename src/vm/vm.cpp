//
// Created by mikegriniezakis on 27/5/2024.
//

#include "vm.h"

#include <cassert>
#include <cstring>

extern avm_memcell* avm_translate_operand(vmarg* arg, avm_memcell* reg) {
    switch (arg->type) {
        case global_a: return &avm_stack.at(AVM_STACKSIZE - 1 - arg->val);
        case local_a: return &avm_stack.at(topsp - arg->val);
        case formal_a: return &avm_stack.at(topsp + AVM_STACKENV_SIZE + 1 + arg->val);
        case retval_a: return &retval;
        case number_a: {
            reg->type = number_m;
            reg->data.numVal = consts_getnumber(arg->val);
            return reg;
        }
        case string_a: {
            reg->type = string_m;
            reg->data.strVal = strdup(consts_getstring(arg->val));
            return reg;
        }
        case bool_a: {
            reg->type = bool_m;
            reg->data.boolVal = arg->val;
            return reg;
        }
        case nil_a: {
            reg->type = nil_m;
            return reg;
        }
        case userfunc_a: {
            reg->type = userfunc_m;
            reg->data.funcVal = arg->val;
            return reg;
        }
        case libfunc_a: {
            reg->type = libfunc_m;
            reg->data.libfuncVal = libfuncs_getused(arg->val);
            return reg;
        }
        default: assert(0);
    }
}
