//
// Created by mikegriniezakis on 1/6/2024.
//

#include <cassert>

#include "dispatcher.h"
#include "src/vm/memory/memory.h"
#include "src/vm/toString/toString.h"

extern void execute_newtable(instruction* instr) {
    avm_memcell* lv = avm_translate_operand(&instr->result, (avm_memcell *) nullptr);
    // assert(lv && (avm_stack[N-1]));
    avm_memcellclear(lv);
    lv->type = table_m;
    lv->data.tableVal = new avm_table();
    avm_table_inc_ref_counter(lv->data.tableVal);
}

extern void execute_tablegetelem(instruction* instr) {
    avm_memcell* lv = avm_translate_operand(&instr->result, (avm_memcell *) nullptr);
    avm_memcell* t = avm_translate_operand(&instr->arg1, (avm_memcell *) nullptr);
    avm_memcell* i = avm_translate_operand(&instr->arg2, &ax);

    avm_memcellclear(lv);
    lv->type = nil_m;

    if (t->type != table_m) {
        fprintf(stderr, "illegal use of type %s as table!", typeString[t->type]);
    } else {
        avm_memcell* content = avm_table_get_elem(t->data.tableVal, i);
        if (content) {
            avm_assign(lv, content);
        } else {
            char* ts = avm_to_string(t);
            char* is = avm_to_string(i);
            fprintf(stderr, "tablegetelem: %s[%s] not found!", ts, is);
            free(ts);
            free(is);
        }
    }
}

extern void execute_tablesetelem(instruction* instr) {
    avm_memcell* t = avm_translate_operand(&instr->result, (avm_memcell *) nullptr);
    avm_memcell* i = avm_translate_operand(&instr->arg1, &ax);
    avm_memcell* c = avm_translate_operand(&instr->arg2, &bx);

    if (t->type != table_m) {
        fprintf(stderr, "illegal use of type %s as table!", typeString[t->type]);
    } else {
        avm_table_set_elem(t->data.tableVal, i, c);
    }
}
