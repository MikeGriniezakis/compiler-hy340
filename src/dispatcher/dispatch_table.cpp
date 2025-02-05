//
// Created by mikegriniezakis on 1/6/2024.
//

#include <cassert>

#include "dispatcher.h"
#include "src/vm/memory/memory.h"
#include "src/vm/toString/toString.h"

extern void execute_newtable(instruction* instr) {
    avm_memcell* lv = avm_translate_operand(&instr->result, nullptr);
    avm_memcellclear(lv);
    lv->type = table_m;
    lv->data.tableVal = new avm_table();
    avm_table_inc_ref_counter(lv->data.tableVal);
}

extern void execute_tablegetelem(instruction* instr) {
    avm_memcell* lv = avm_translate_operand(&instr->result, nullptr);
    avm_memcell* t = avm_translate_operand(&instr->arg1, nullptr);
    avm_memcell* i = avm_translate_operand(&instr->arg2, &ax);

    assert(lv && (&avm_stack[AVM_STACKSIZE - 1]) >= lv && lv > &avm_stack[top] || lv == &retval);
    assert(t && (&avm_stack[AVM_STACKSIZE - 1]) >= t && t > &avm_stack[top]);
    assert(i);

    avm_memcellclear(lv);
    lv->type = nil_m;

    if (t->type != table_m) {
        fprintf(stderr, "illegal use of type %s as table!\n", typeString[t->type]);
    } else {
        avm_memcell* content = avm_table_get_elem(t->data.tableVal, i);
        if (content) {
            avm_assign(lv, content);
        } else {
            char* is = avm_to_string(i);
            fprintf(stderr, "tablegetelem: [%s] not found!\n", is);
            free(is);
        }
    }
}

extern void execute_tablesetelem(instruction* instr) {
    avm_memcell* t = avm_translate_operand(&instr->result, nullptr);
    avm_memcell* i = avm_translate_operand(&instr->arg1, &ax);
    avm_memcell* c = avm_translate_operand(&instr->arg2, &bx);

    if (t->type != table_m) {
        fprintf(stderr, "illegal use of type %s as table!\n", typeString[t->type]);
    } else {
        avm_table_set_elem(t->data.tableVal, i, c);
    }
}
