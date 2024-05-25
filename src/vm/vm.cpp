//
// Created by mikegriniezakis on 25/5/2024.
//

#include "vm.h"

#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>

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

void VirtualMachine::printVMArg(std::stringstream* ss, vmarg* arg) {
    if (arg == nullptr) {
        *ss << std::setw(15) << "NULL";
        return;
    }

    switch (arg->type) {
        case label_a:
            *ss << std::setw(15) << arg->type << "(label) " << arg->val;
            break;
        case global_a:
        case formal_a:
        case local_a:
            *ss << std::setw(15) << "(global), " << arg->type << ":" << arg->val;
            break;
        case number_a:
            *ss << std::setw(15) << arg->type << "(number), " << arg->val << ':' << numConsts.at(arg->val);
            break;
        case string_a:
            *ss << std::setw(15) << arg->type << "(string) " << stringConsts.at(arg->val);
            break;
        case bool_a:
            *ss << std::setw(15) << arg->type << "(bool) " << (arg->val ? "true" : "false");
            break;
        case nil_a:
            *ss << std::setw(15) << arg->type << "(nil)";
            break;
        case userfunc_a:
            *ss << std::setw(15) << arg->type << "(user function) " << this->userFuncs.at(arg->val)->name;
            break;
        case libfunc_a:
            *ss << std::setw(15) << arg->type << "(library function) " << this->namedLibfuncs.at(arg->val);
            break;
        case retval_a:
            *ss << std::setw(15) << "(return value) " << arg->type << ":" << arg->val;
            break;
        default:
            *ss << std::setw(15) << arg->type << ":" << arg->val;
            break;
    }
}


void VirtualMachine::printInstruction(std::stringstream* ss, instruction* instruction) {
    *ss << std::setw(15) << vmopcodeNames[instruction->opcode].c_str();
    this->printVMArg(ss, &instruction->result);
    this->printVMArg(ss, &instruction->arg1);
    this->printVMArg(ss, &instruction->arg2);
}

void VirtualMachine::print() {
    std::stringstream ss;
    for (int i = 0; i < this->instructions.size(); i++) {
        auto t = this->instructions[i];
        ss << std::setw(15) << "Instruction#";
        ss << std::setw(15) << "opcode";
        ss << std::setw(15) << "result";
        ss << std::setw(15) << "arg1";
        ss << std::setw(15) << "arg2";
        ss << std::endl;

        ss << std::setw(15) << i;
        this->printInstruction(&ss, t);
        ss << std::endl;

        printf("%s\n", ss.str().c_str());
        ss.clear();
    }
}

void VirtualMachine::createBinaryFile() {
    char terminator = '\0';

    FILE *file = fopen("output.bin", "wb");
    if (!file) {
        assert(0);
    }

    unsigned magic = 45823297;
    fwrite(&magic, sizeof(unsigned int), 1, file);
    // fwrite(&programVarOffset, sizeof(unsigned int), 1, file);

    unsigned int numConstsSize = numConsts.size();
    fwrite(&(numConstsSize), sizeof(unsigned int), 1, file);
    for (double num : numConsts) {
        fwrite(&num, sizeof(double), 1, file);
    }


    unsigned int stringConstsSize = stringConsts.size();
    fwrite(&stringConstsSize, sizeof(unsigned), 1, file);
    for (std::string str: stringConsts) {
        for (char c : str) {
            fwrite(&c, sizeof(char), 1, file);
        }
        fwrite(&terminator, sizeof(char), 1, file);
    }

    unsigned int namedLibFunctionsSize = namedLibfuncs.size();
    fwrite(&namedLibFunctionsSize, sizeof(unsigned), 1, file);
    for (std::string fun : namedLibfuncs) {
        for (char c : fun) {
            fwrite(&c, sizeof(char), 1, file);
        }
        fwrite(&terminator, sizeof(char), 1, file);
    }

    unsigned int userFunctionsSize = userFuncs.size();
    fwrite(&userFunctionsSize, sizeof(unsigned), 1, file);
    for (SymbolStruct* fun : userFuncs) {
        // for (fun->name) {
        //     fwrite(&c, sizeof(char), 1, file);
        // }

        fwrite(&terminator, sizeof(char), 1, file);

        // fwrite(&fun.address, sizeof(unsigned), 1, file);
        // fwrite(&fun.localSize, sizeof(unsigned), 1, file);
    }

    unsigned int instructionsSize = instructions.size();
    fwrite(&instructionsSize, sizeof(unsigned), 1, file);
    for (auto instruction : instructions) {
        fwrite(&instruction->opcode, sizeof(vmopcode), 1, file);

        // unsigned isResultNull = instruction->result.isEmpty;
        // fwrite(&isResultNull, sizeof(unsigned), 1, file);
    //     if (instruction->result != nullptr) {
    //         fwrite(&instruction->result.type, sizeof(vmarg_t), 1, file);
    //         fwrite(&instruction->result.val, sizeof(unsigned), 1, file);
    //         fwrite(&terminator, sizeof(char), 1, file);
    //     }
    //
    //     unsigned isArg1Null = instruction->arg1.isEmpty;
    //     fwrite(&isArg1Null, sizeof(unsigned), 1, file);
    //     if (!isArg1Null) {
    //         fwrite(&instruction->arg1.type, sizeof(vmarg_t), 1, file);
    //         fwrite(&instruction->arg1.val, sizeof(unsigned), 1, file);
    //         for (char c : instruction->arg1.name) {
    //             fwrite(&c, sizeof(char), 1, file);
    //         }
    //         fwrite(&terminator, sizeof(char), 1, file);
    //     }
    //
    //     unsigned isArg2Null = instruction->arg2.isEmpty;
    //     fwrite(&isArg2Null, sizeof(unsigned), 1, file);
    //     if (!isArg2Null) {
    //         fwrite(&instruction->arg2.type, sizeof(vmarg_t), 1, file);
    //         fwrite(&instruction->arg2.val, sizeof(unsigned), 1, file);
    //         for (char c : instruction->arg2.name) {
    //             fwrite(&c, sizeof(char), 1, file);
    //         }
    //         fwrite(&terminator, sizeof(char), 1, file);
    //     }
    //
        fwrite(&instruction->srcLine, sizeof(unsigned), 1, file);
    }

    fclose(file);
}
