//
// Created by mikegriniezakis on 28/5/2024.
//

#include <cassert>
#include <cstring>
#include <src/dispatcher/dispatcher.h>
#include "src/vm/memory/memory.h"

void avm_assign(avm_memcell* lv, avm_memcell* rv) {
    if (lv == rv)
        return;

    if (lv->type == table_m && rv->type == table_m && lv->data.tableVal == rv->data.tableVal)
        return;

    if (rv->type == undef_m)
        printf("assigning from 'undef' content!\n");
        // avm_warning("assigning from 'undef' content!");

    avm_memcellclear(lv);
    memcpy(lv, rv, sizeof(avm_memcell));

    if (lv->type == string_m)
        lv->data.strVal = strdup(rv->data.strVal);
    else if (lv->type == table_m)
        avm_table_inc_ref_counter(lv->data.tableVal);
}

extern void execute_assign (instruction* instr) {
    avm_memcell* lv = avm_translate_operand(&instr->result, nullptr);
    avm_memcell* rv = avm_translate_operand(&instr->arg1, &ax);

    assert(lv && (&avm_stack[AVM_STACKSIZE - 1] >= lv && lv > &avm_stack[top] || lv == &retval));
    assert(rv);

    avm_assign(lv, rv);
}
