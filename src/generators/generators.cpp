//
// Created by mikegriniezakis on 25/5/2024.
//
#include "generators.h"

void generate(vmopcode op, Quad* quad, TargetCode* vm) {
    auto t = new instruction();
    t->opcode = op;
    vm->makeOperand(quad->getArg1(), &t->arg1);
    vm->makeOperand(quad->getArg2(), &t->arg2);
    vm->makeOperand(quad->getResult(), &t->result);
    quad->setTAddress(vm->nextInstructionLabel());
    vm->emit(t);
}

void generate_relational(vmopcode op, Quad* quad, TargetCode* vm) {
    auto t = new instruction();
    t->opcode = op;
    vm->makeOperand(quad->getArg1(), &t->arg1);
    vm->makeOperand(quad->getArg2(), &t->arg2);

    t->result.type = label_a;
    if (quad->getLabel() < currentQuad)
        t->result.val = vm->getQuadTAddress(quad->getLabel());
    else
        vm->addIncompleteJump(vm->nextInstructionLabel(), quad->getLabel());

    quad->setTAddress(vm->nextInstructionLabel());
    vm->emit(t);
}

extern void generate_ADD(Quad* quad, TargetCode* vm) {
    generate(add_v, quad, vm);
}

extern void generate_SUB(Quad* quad, TargetCode* vm) {
    generate(sub_v, quad, vm);
}

extern void generate_MUL(Quad* quad, TargetCode* vm) {
    generate(mul_v, quad, vm);
}

extern void generate_DIV(Quad* quad, TargetCode* vm) {
    generate(div_v, quad, vm);
}

extern void generate_MOD(Quad* quad, TargetCode* vm) {
    generate(mod_v, quad, vm);
}

extern void generate_NEWTABLE(Quad* quad, TargetCode* vm) {
    generate(newtable_v, quad, vm);
}

extern void generate_TABLEGETELEM(Quad* quad, TargetCode* vm) {
    generate(tablegetelem_v, quad, vm);
}

extern void generate_TABLESETELEM(Quad* quad, TargetCode* vm) {
    generate(tablesetelem_v, quad, vm);
}

extern void generate_ASSIGN(Quad* quad, TargetCode* vm) {
    generate(assign_v, quad, vm);
}

extern void generate_NOP(Quad* quad, TargetCode* vm) {
    auto* t = new instruction();
    t->opcode = nop_v;
    vm->emit(t);
}


extern void generate_JUMP(Quad* quad, TargetCode* vm) {
    generate_relational(jump_v, quad, vm);
}

extern void generate_IF_EQ(Quad* quad, TargetCode* vm) {
    generate_relational(jeq_v, quad, vm);
}

extern void generate_IF_NOTEQ(Quad* quad, TargetCode* vm) {
    generate_relational(jne_v, quad, vm);
}

extern void generate_IF_GREATER(Quad* quad, TargetCode* vm) {
    generate_relational(jgt_v, quad, vm);
}

extern void generate_IF_GREATEREQ(Quad* quad, TargetCode* vm) {
    generate_relational(jge_v, quad, vm);
}

extern void generate_IF_LESS(Quad* quad, TargetCode* vm) {
    generate_relational(jlt_v, quad, vm);
}

extern void generate_IF_LESSEQ(Quad* quad, TargetCode* vm) {
    generate_relational(jle_v, quad, vm);
}

extern void generate_NOT(Quad* quad, TargetCode* vm) {
    quad->setTAddress(vm->nextInstructionLabel());
    auto t = new instruction();

    t->opcode = jeq_v;
    vm->makeOperand(quad->getArg1(), &t->arg1);
    vm->makeBoolOperand(&t->arg2, 0);
    t->result.type = label_a;
    t->result.val = vm->nextInstructionLabel() + 3;
    vm->emit(t);

    t->opcode = assign_v;
    vm->makeBoolOperand(&t->arg1, 0);
    vm->resetOperand(&t->arg2);
    vm->makeOperand(quad->getResult(), &t->result);
    vm->emit(t);

    t->opcode = jump_v;
    vm->resetOperand(&t->arg1);
    vm->resetOperand(&t->arg2);
    t->result.type = label_a;
    t->result.val = vm->nextInstructionLabel() + 2;
    vm->emit(t);

    t->opcode = assign_v;
    vm->makeBoolOperand(&t->arg1, 1);
    vm->resetOperand(&t->arg2);
    vm->makeOperand(quad->getResult(), &t->result);
    vm->emit(t);
}

extern void generate_OR(Quad* quad, TargetCode* vm) {
    quad->setTAddress(vm->nextInstructionLabel());
    auto t = new instruction();

    t->opcode = jeq_v;
    vm->makeOperand(quad->getArg1(), &t->arg1);
    vm->makeBoolOperand(&t->arg2, 1);
    t->result.type = label_a;
    t->result.val = vm->nextInstructionLabel() + 4;
    vm->emit(t);

    vm->makeOperand(quad->getArg2(), &t->arg1);
    t->result.val = vm->nextInstructionLabel() + 3;
    vm->emit(t);

    t->opcode = assign_v;
    vm->makeBoolOperand(&t->arg1, 0);
    vm->resetOperand(&t->arg2);
    vm->makeOperand(quad->getResult(), &t->result);
    vm->emit(t);

    t->opcode = jump_v;
    vm->resetOperand(&t->arg1);
    vm->resetOperand(&t->arg2);
    t->result.type = label_a;
    t->result.val = vm->nextInstructionLabel() + 2;
    vm->emit(t);

    t->opcode = assign_v;
    vm->makeBoolOperand(&t->arg1, 1);
    vm->resetOperand(&t->arg2);
    vm->makeOperand(quad->getResult(), &t->result);
    vm->emit(t);
}

extern void generate_PARAM(Quad* quad, TargetCode* vm) {
    quad->setTAddress(vm->nextInstructionLabel());
    auto t = new instruction();
    t->opcode = pusharg_v;
    vm->makeOperand(quad->getResult(), &t->result);
    vm->makeOperand(quad->getArg1(), &t->arg1);
    vm->makeOperand(quad->getArg2(), &t->arg2);
    vm->emit(t);
}

extern void generate_CALL(Quad* quad, TargetCode* vm) {
    quad->setTAddress(vm->nextInstructionLabel());
    auto t = new instruction();
    t->opcode = call_v;
    vm->makeOperand(quad->getResult(), &t->result);
    vm->makeOperand(quad->getArg1(), &t->arg1);
    vm->makeOperand(quad->getArg2(), &t->arg2);
    vm->emit(t);
}

extern void generate_GETRETVAL(Quad* quad, TargetCode* vm) {
    quad->setTAddress(vm->nextInstructionLabel());
    auto t = new instruction();
    t->opcode = assign_v;
    vm->makeOperand(quad->getResult(), &t->result);
    vm->makeRetvalOperand(&t->arg1);
    vm->makeOperand(quad->getArg2(), &t->arg2);
    vm->emit(t);
}

extern void generate_FUNCSTART(Quad* quad, TargetCode* vm) {
    auto f = quad->getResult()->symbol;
    f->tAddress = vm->nextInstructionLabel();
    quad->setTAddress(vm->nextInstructionLabel());

    auto t = new instruction();
    t->opcode = funcenter_v;
    inFuncStart = true;
    vm->makeOperand(quad->getResult(), &t->result);
    vm->emit(t);
}

extern void generate_RETURN(Quad* quad, TargetCode* vm) {
    quad->setTAddress(vm->nextInstructionLabel());
    auto t = new instruction();
    t->opcode = assign_v;
    vm->makeRetvalOperand(&t->result);
    vm->makeOperand(quad->getArg1(), &t->arg1);
    vm->emit(t);
}

extern void generate_FUNCEND(Quad* quad, TargetCode* vm) {
    quad->setTAddress(vm->nextInstructionLabel());
    auto t = new instruction();
    t->opcode = funcexit_v;
    vm->makeOperand(quad->getResult(), &t->result);
    vm->emit(t);
}

extern void generate_UMINUS(Quad* quad, TargetCode* vm) {
    quad->setTAddress(vm->nextInstructionLabel());
    auto t = new instruction();
    t->opcode = mul_v;
    vm->makeOperand(quad->getArg1(), &t->arg1);
    vm->makeOperand(quad->getArg2(), &t->arg2);
    t->arg2.val = vm->consts_newnumber(-1);
    t->arg2.type = number_a;
    vm->makeOperand(quad->getResult(), &t->result);
    t->srcLine = quad->getLine();
    vm->emit(t);
}

extern void generate_AND(Quad* quad, TargetCode* vm) {
    quad->setTAddress(vm->nextInstructionLabel());
    auto t = new instruction();

    t->opcode = jeq_v;
    vm->makeOperand(quad->getArg1(), &t->arg1);
    vm->makeBoolOperand(&t->arg2, 0);
    t->result.type = label_a;
    t->result.val = vm->nextInstructionLabel() + 4;
    vm->emit(t);

    vm->makeOperand(quad->getArg2(), &t->arg1);
    t->result.val = vm->nextInstructionLabel() + 3;
    vm->emit(t);

    t->opcode = assign_v;
    vm->makeBoolOperand(&t->arg1, 1);
    vm->resetOperand(&t->arg2);
    vm->makeOperand(quad->getResult(), &t->result);
    vm->emit(t);

    t->opcode = jump_v;
    vm->resetOperand(&t->arg1);
    vm->resetOperand(&t->arg2);
    t->result.type = label_a;
    t->result.val = vm->nextInstructionLabel() + 2;
    vm->emit(t);

    t->opcode = assign_v;
    vm->makeBoolOperand(&t->arg1, 0);
    vm->resetOperand(&t->arg2);
    vm->makeOperand(quad->getResult(), &t->result);
    vm->emit(t);
}
