//
// Created by mikegriniezakis on 30/5/2024.
//

#include <cassert>

#include "dispatcher.h"
#include "src/vm/memory/memory.h"

extern void execute_arithmetic(instruction* instr) {
    avm_memcell* lv = avm_translate_operand(&instr->result, (avm_memcell*)0);
    avm_memcell* rv1 = avm_translate_operand(&instr->arg1, &ax);
    avm_memcell* rv2 = avm_translate_operand(&instr->arg2, &bx);

    assert(lv && (&avm_stack[AVM_STACKSIZE] >= lv && lv > &avm_stack[top] || lv == &retval));
    assert(rv1 && rv2);

    if (rv1->type != number_m || rv2->type != number_m) {
        // avm_error("not a number in arithmetic!");
        fprintf(stderr, "not a number in arithmetic!");
        executionFinished = true;
    } else {
        arithmetic_func_t op = arithmeticFuncs[instr->opcode - add_v];
        avm_memcellclear(lv);
        lv->type = number_m;
        lv->data.numVal = (*op)(rv1->data.numVal, rv2->data.numVal);
    }
}
