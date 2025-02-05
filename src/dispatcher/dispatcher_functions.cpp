//
// Created by mikegriniezakis on 28/5/2024.
//

#include <cassert>
#include <src/dispatcher/dispatcher.h>

#include "src/vm/memory/memory.h"
#include "src/vm/toString/toString.h"

extern void execute_call (instruction* instr) {
    avm_memcell* func = avm_translate_operand(&instr->result, &ax);
    assert(func);
    restorePc = pc;
    avm_call_save_env();

    switch (func->type) {
        case userfunc_m:
            pc = userFuncs.at(func->data.funcVal)->address;
            assert(pc < AVM_ENDING_PC.size());
            assert(instructions.at(pc)->opcode == funcenter_v);
            break;
        case libfunc_m:
            avm_call_lib_func(func->data.libfuncVal);
            break;
        case string_m:
            avm_call_lib_func(func->data.strVal);
            break;
        default:
            char* s = avm_to_string(func);
            // avm_error("call: cannot bind '%s' to function!", s);
            fprintf(stderr, "call: cannot bind '%s' to function!", s);
            executionFinished = true;
    }
}

extern userfunc* avm_get_func_info(unsigned i) {
    if (userFuncs.at(i)->address == pc) {
        return userFuncs.at(i);
    }
    return nullptr;
}

extern void execute_funcenter (instruction* instr) {
    avm_memcell* func = avm_translate_operand(&instr->result, &ax);
    assert(func);
    assert(pc == userFuncs.at(func->data.funcVal)->address);

    totalActuals = 0;
    userfunc* funcInfo = avm_get_func_info(func->data.funcVal);
    topsp = top;
    top = top - funcInfo->localSize;
}

extern void execute_funcexit (instruction* instr) {
    if (instructions.at(restorePc)->opcode != call_v) {
        return;
    }

    unsigned oldTop = top;
    top = avm_get_env_value(topsp + AVM_SAVEDTOP_OFFSET);
    pc = avm_get_env_value(topsp + AVM_SAVEDPC_OFFSET);
    topsp = avm_get_env_value(topsp + AVM_SAVEDTOPSP_OFFSET);
    while (++oldTop  <= top) {
        avm_memcellclear(&avm_stack.at(oldTop));
    }
}

library_func_t avm_get_library_func(char* id) {
    return libFuncs.at(id);
}

extern unsigned avm_totalactuals() {
    return avm_get_env_value(topsp + AVM_NUMACTUALS_OFFSET);
}

extern avm_memcell* avm_getactual(unsigned i) {
    return &avm_stack[topsp + AVM_STACKENV_SIZE + 1 + i];
}

extern void avm_call_lib_func(char* funcName) {
    library_func_t f = avm_get_library_func(funcName);

    if (!f) {
        fprintf(stderr, "unsupported lib func '%s' called!", funcName);
        executionFinished = true;
    } else {
        topsp = top;
        totalActuals = 0;
        (*f)();
        if (!executionFinished) {
            execute_funcexit(nullptr);
        }
    }
}

extern void avm_push_env_value(unsigned val) {
    avm_stack.at(top).type = number_m;
    avm_stack.at(top).data.numVal = val;
    avm_dec_top();
}

extern unsigned avm_get_env_value(unsigned i) {
    assert(avm_stack[i].type == number_m);

    auto val = (unsigned) avm_stack[i].data.numVal;
    assert(avm_stack[i].data.numVal == (double) val);
    return val;
}

extern void avm_call_save_env() {
    avm_push_env_value(totalActuals);
    avm_push_env_value(pc + 1);
    avm_push_env_value(top + totalActuals + 2);
    avm_push_env_value(topsp);
}

extern void execute_pusharg(instruction* instr) {
    avm_memcell* arg = avm_translate_operand(&instr->result, &ax);
    avm_assign(&avm_stack[top], arg);
    ++totalActuals;
    avm_dec_top();
}

