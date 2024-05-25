//
// Created by mikegriniezakis on 25/5/2024.
//

#include "vm.h"

#include <cassert>
#include <iostream>

#include "src/generators/generators.h"

void VirtualMachine::makeOperand(expr* expr, vmarg* arg) {
    if (expr == nullptr)
        return;

    switch (expr->type) {
        case var_e:
        case tableitem_e:
        case arithexpr_e:
        case boolexpr_e:
        case newtable_e: {
            arg->val = expr->symbol->offset;

            switch (expr->symbol->scope_space) {
                case PROGRAM_VAR:
                    arg->type = global_a;
                    break;
                case FORMAL_ARG:
                    arg->type = formal_a;
                    break;
                case FUNCTION_LOCAL:
                    arg->type = local_a;
                    break;
                default: assert(0);
            }
        }
        case constbool_e:
            arg->val = expr->boolConst;
            break;
        case conststring_e:
            arg->val = consts_newstring(expr->strConst);
            arg->type = string_a;
            break;
        case constnum_e:
            arg->val = consts_newnumber(expr->numConst);
            arg->type = number_a;
            break;
        case nil_e:
            arg->type = nil_a;
            break;
        case programfunc_e:
            arg->type = userfunc_a;
            arg->val = userfuncs_newfunc(expr->symbol);
            break;
        case libraryfunc_e:
            arg->type = libfunc_a;
            arg->val = libfuncs_newused(expr->strConst);
            break;
        default: assert(0);
    }
}

void VirtualMachine::makeNumberOperand(vmarg* arg, double val) {
    arg->val = consts_newnumber(val);
    arg->type = number_a;
}

void VirtualMachine::makeBoolOperand(vmarg* arg, unsigned val) {
    arg->type = bool_a;
    arg->val = val;
}

void VirtualMachine::makeRetvalOperand(vmarg* arg) {
    arg->type = retval_a;
}

unsigned VirtualMachine::consts_newstring(char* str) {
    this->stringConsts.push_back(str);
    return this->stringConsts.size() - 1;
}

unsigned VirtualMachine::libfuncs_newused(char* str) {
    this->namedLibfuncs.push_back(str);
    return this->namedLibfuncs.size() - 1;
}

unsigned VirtualMachine::consts_newnumber(double num) {
    this->numConsts.push_back(num);
    return this->numConsts.size() - 1;
}

unsigned VirtualMachine::userfuncs_newfunc(SymbolStruct* sym) {
    this->userFuncs.push_back(sym);
    return this->userFuncs.size() - 1;
}

SymbolStruct* VirtualMachine::userfuncs_getfunc(bool pop) {
    SymbolStruct* sym = this->userFuncs.back();
    if (pop) {
        this->userFuncs.pop_back();
    }
    return sym;
}


void VirtualMachine::addIncompleteJump(unsigned istrNo, unsigned iaddress) {
    auto* newJump = new inclomplete_jump();
    newJump->instrNo = istrNo;
    newJump->iaddress = iaddress;

    this->inclomplete_jumps.push_back(newJump);
}

void VirtualMachine::patchIncompleteJumps() {
    for (auto jump: this->inclomplete_jumps) {
        if (jump->iaddress == this->instructions.size()) {
            this->instructions[jump->instrNo]->result.val = this->instructions.size();
        } else {
            this->instructions[jump->instrNo]->result.val = this->quads->getQuad(jump->iaddress)->getTAddress();
        }
    }
}

void VirtualMachine::generate() {
    for (auto quad: this->quads->getQuads()) {
        (*generators[quad->getCode()])(quad, this);
    }
}

unsigned VirtualMachine::nextInstructionLabel() {
    return this->instructions.size();
}

void VirtualMachine::emit(instruction* instruction) {
    this->instructions.push_back(instruction);
}

unsigned VirtualMachine::getCurrentQuad() {
    return this->quads->nextQuad() - 1;
}

unsigned VirtualMachine::getQuadTAddress(unsigned quadNo) {
    return this->quads->getQuad(quadNo)->getTAddress();
}

void VirtualMachine::resetOperand(vmarg* arg) {
    arg->val = 0;
}

void VirtualMachine::printVMArg(vmarg* arg) {
    if (arg == nullptr) {
        return;
    }

    printf("%d", arg->type);
    switch (arg->type) {
        case label_a:
            std::cout << "(label) " << arg->val;
            break;
        case global_a:
            std::cout << "(global), " << arg->type << ":" << arg->val;
            break;
        case formal_a:
            std::cout << "(global), " << arg->type << ":" << arg->val;
            break;
        case local_a:
            std::cout << "(global), " << arg->type << ":" << arg->val;
        break;
        case number_a:
            std::cout << "(number), " << arg->val << ':' << numConsts.at(arg->val);
            break;
        case string_a:
            std::cout << "(string) " << stringConsts.at(arg->val);
            break;
        case bool_a:
            std::cout << "(bool) " << (arg->val ? "true": "false");
            break;
        case nil_a:
            std::cout << "(nil)";
            break;
        case userfunc_a:
            std::cout << "(user function) " << this->userFuncs.at(arg->val)->name;
            break;
        case libfunc_a:
            std::cout << "(library function) " << this->namedLibfuncs.at(arg->val);
            break;
        case retval_a:
            std::cout << "(return value) " << arg->type << ":" << arg->val;
            break;
        default:
            std::cout << arg->type << ":" << arg->val;
            break;
    }
}


void VirtualMachine::printInstruction(instruction* instruction) {
    printf("%s\t", vmopcodeNames[instruction->opcode].c_str());
    this->printVMArg(&instruction->result);
    printf("\t");
    this->printVMArg(&instruction->arg1);
    printf("\t");
    this->printVMArg(&instruction->arg2);
    printf("\n");
}

void VirtualMachine::print() {
    for (int i = 0; i < this->instructions.size(); i++) {
        auto t = this->instructions[i];
        printf("Instruction#\topcode\t\tresult\t\t arg1\t\t arg2\t\tlabel\n");
        std::cout << i;
        printf("\t\t");
        this->printInstruction(t);
    }
}
