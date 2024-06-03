//
// Created by mikegriniezakis on 30/5/2024.
//

#include <cassert>

#include "dispatcher.h"
#include "src/vm/toString/toString.h"
#include "src/vm/toBool/toBool.h"

extern void execute_comparison(instruction* instr) {
    avm_memcell* arg1 = avm_translate_operand(&instr->arg1, &ax);
    avm_memcell* arg2 = avm_translate_operand(&instr->arg2, &bx);

    assert(arg1 && arg2);

    if (arg1->type != number_m || arg2->type != number_m) {
        fprintf(stderr, "Non numeric value used in comparison!");
        executionFinished = true;
    } else {
        auto op = comparisonFuncs[instr->opcode - jle_v];
        int result = (*op)(arg1->data.numVal, arg2->data.numVal);

        if (!executionFinished && result) {
            pc = instr->result.val;
        }
    }
}

extern void execute_jeq(instruction* input) {
    assert(input->result.type == label_a);

    avm_memcell* arg1 = avm_translate_operand(&input->arg1, &ax);
    avm_memcell* arg2 = avm_translate_operand(&input->arg2, &bx);

    unsigned char result = 0;
    if (arg1->type == undef_m || arg2->type == undef_m) {
        fprintf(stderr, "comparison with undefined opperand!");
    } else if (arg1->type == nil_m || arg2->type == nil_m) {
        result = arg1->type == nil_m && arg1->type == nil_m;
    } else if (arg1->type == bool_m || arg2->type == bool_m) {
        result = avm_to_bool(arg1) == avm_to_bool(arg2);
    } else if (arg1->type != arg2->type) {
        fprintf(stderr, "comparison between %s and %s is illegal!", typeString[arg1->type], typeString[arg2->type]);
    } else if (arg1->type == number_m && arg2->type == number_m) {
        result = arg1->data.numVal == arg2->data.numVal;
    } else {
        result = avm_to_bool(arg1) == avm_to_bool(arg2);
    }

    if (!executionFinished && result) {
        pc = input->result.val;
    }
}

extern void execute_jne(instruction* input) {
    assert(input->result.type == label_a);

    avm_memcell* arg1 = avm_translate_operand(&input->arg1, &ax);
    avm_memcell* arg2 = avm_translate_operand(&input->arg2, &bx);
    unsigned char result = 0;


    if (arg1->type == undef_m || arg2->type == undef_m)
        fprintf(stderr, "comparison with undefined opperand!");
    else if (arg1->type == nil_m || arg2->type == nil_m)
        result = !(arg1->type == nil_m && arg1->type == nil_m);
    else if (arg1->type == bool_m || arg2->type == bool_m)
        result = avm_to_bool(arg1) != avm_to_bool(arg2);
    else if (arg1->type != arg2->type)
        fprintf(stderr, "comparison between %s and %s is illegal!", typeString[arg1->type],
                typeString[arg2->type]);
    else if (arg1->type == number_m && arg2->type == number_m)
        result = arg1->data.numVal != arg2->data.numVal;
    else
        result = avm_to_bool(arg1) != avm_to_bool(arg2);

    if (!executionFinished && result)
        pc = input->result.val;
}

void execute_and(instruction* instr)     {}
void execute_or(instruction* instr)      {}
void execute_not(instruction* instr)     {}
void execute_nop(instruction*) {};
void execute_jump(instruction* instr) {
    pc = instr->result.val;
}
