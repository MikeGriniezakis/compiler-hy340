//
// Created by mikegriniezakis on 25/5/2024.
//

#include "targetCode.h"

#include <cassert>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <unistd.h>

#include "src/generators/generators.h"

void TargetCode::makeOperand(expr* expr, vmarg* arg) {
    if (expr == nullptr) {
        arg->empty = true;
        return;
    }

    arg->empty = false;
    switch (expr->type) {
        case var_e:
        case tableitem_e:
        case arithexpr_e:
        case boolexpr_e:
        case assignexpr_e:
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
            break;
        }
        case constbool_e:
            arg->val = expr->boolConst;
            arg->type = bool_a;
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
            if (inFuncStart) {
                arg->val = userfuncs_newfunc(expr->symbol);
                inFuncStart = false;
            } else {
                for (int i = 0; i < userFuncs.size(); i++) {
                    auto* f = userFuncs.at(i);
                    if (strcmp(f->id, expr->symbol->name) == 0 && f->scope == expr->symbol->scope) {
                        arg->val = i;
                        break;
                    }
                }
            }
            break;
        case libraryfunc_e:
            arg->type = libfunc_a;
            arg->val = libfuncs_newused(expr->strConst);
            break;
        default: assert(0);
    }
}

void TargetCode::makeNumberOperand(vmarg* arg, double val) {
    arg->val = consts_newnumber(val);
    arg->type = number_a;
}

void TargetCode::makeBoolOperand(vmarg* arg, unsigned val) {
    arg->type = bool_a;
    arg->val = val;
}

void TargetCode::makeRetvalOperand(vmarg* arg) {
    arg->type = retval_a;
}

unsigned TargetCode::consts_newstring(char* str) {
    this->stringConsts.push_back(str);
    return this->stringConsts.size() - 1;
}

unsigned TargetCode::libfuncs_newused(char* str) {
    this->namedLibfuncs.push_back(str);
    return this->namedLibfuncs.size() - 1;
}

unsigned TargetCode::consts_newnumber(double num) {
    this->numConsts.push_back(num);
    return this->numConsts.size() - 1;
}

unsigned TargetCode::userfuncs_newfunc(SymbolStruct* sym) {
    auto* userfunc = new struct userfunc();
    userfunc->address = sym->tAddress;
    userfunc->localSize = sym->localSize;
    userfunc->id = sym->name;
    userfunc->scope = sym->scope;
    this->userFuncs.push_back(userfunc);
    return this->userFuncs.size() - 1;
}

userfunc* TargetCode::userfuncs_getfunc(bool pop) {
    userfunc* sym = this->userFuncs.back();
    if (pop) {
        this->userFuncs.pop_back();
    }
    return sym;
}


void TargetCode::addIncompleteJump(unsigned istrNo, unsigned iaddress) {
    auto* newJump = new inclomplete_jump();
    newJump->instrNo = istrNo;
    newJump->iaddress = iaddress;

    this->inclomplete_jumps.push_back(newJump);
}

void TargetCode::patchIncompleteJumps() {
    for (auto jump: this->inclomplete_jumps) {
        if (jump->iaddress == this->instructions.size()) {
            this->instructions[jump->instrNo]->result.val = this->instructions.size();
        } else {
            this->instructions[jump->instrNo]->result.val = this->quads->getQuad(jump->iaddress)->getTAddress();
        }
    }
}

void TargetCode::generate() {
    for (auto quad: this->quads->getQuads()) {
        (*generators[quad->getCode()])(quad, this);
        currentQuad++;
    }

    this->patchIncompleteJumps();
}

unsigned TargetCode::nextInstructionLabel() {
    return this->instructions.size();
}

void TargetCode::emit(instruction* instruction) {
    this->instructions.push_back(instruction);
}

unsigned TargetCode::getCurrentQuad() {
    return this->quads->nextQuad() - 1;
}

unsigned TargetCode::getQuadTAddress(unsigned quadNo) {
    return this->quads->getQuad(quadNo)->getTAddress();
}

void TargetCode::resetOperand(vmarg* arg) {
    arg->val = 0;
}

void TargetCode::printVMArg(std::stringstream* ss, vmarg* arg) {
    if (arg == nullptr || arg->empty) {
        *ss << std::setw(25) << "NULL";
        return;
    }
    std::stringstream typeSS;

    switch (arg->type) {
        case label_a:
            typeSS << arg->type << "(label) " << arg->val;
            break;
        case global_a:
            typeSS << arg->type << "(global), " << arg->val;
            break;
        case formal_a:
            typeSS << arg->type << "(formal), " << arg->val;
            break;
        case local_a:
            typeSS << arg->type << "(local), " << arg->val;
            break;
        case number_a:
            typeSS << arg->type << "(number), " << arg->val << ':' << numConsts.at(arg->val);
            break;
        case string_a:
            typeSS << arg->type << "(string) " << arg->val << ':' << stringConsts.at(arg->val);
            break;
        case bool_a:
            typeSS << arg->type << "(bool) " << (arg->val ? "true" : "false");
            break;
        case nil_a:
            typeSS << arg->type << "(nil)";
            break;
        case userfunc_a:
            typeSS << arg->type << "(user function) " << this->userFuncs.at(arg->val)->id;
            break;
        case libfunc_a:
            typeSS << arg->type << "(library function) " << this->namedLibfuncs.at(arg->val);
            break;
        case retval_a:
            typeSS << "(return value) " << arg->type << ":" << arg->val;
            break;
        default:
            typeSS << arg->type << ":" << arg->val;
            break;
    }
    *ss << std::setw(25) << typeSS.str();
}


void TargetCode::printInstruction(std::stringstream* ss, instruction* instruction) {
    *ss << std::setw(25) << vmopcodeNames[instruction->opcode].c_str();
    this->printVMArg(ss, &instruction->result);
    this->printVMArg(ss, &instruction->arg1);
    this->printVMArg(ss, &instruction->arg2);
}

void TargetCode::print() {
    std::stringstream ss;
    ss << std::setw(25) << "Instruction#";
    ss << std::setw(25) << "opcode";
    ss << std::setw(25) << "result";
    ss << std::setw(25) << "arg1";
    ss << std::setw(25) << "arg2";
    ss << std::endl;
    for (int i = 0; i < this->instructions.size(); i++) {
        auto t = this->instructions[i];
        ss << std::setw(25) << i;
        this->printInstruction(&ss, t);
        ss << std::endl;
    }
    printf("%s\n", ss.str().c_str());
    ss.clear();
}

void TargetCode::createBinaryFile() {
    constexpr char nullTerminator = '\0';
    FILE *file = fopen("output.bin", "wb");
    if (!file) {
        assert(0);
    }

    constexpr unsigned magic = 131655629;
    fwrite(&magic, sizeof(unsigned), 1, file);
    const unsigned offset = this->symbolTable->getVarOffset();
    fwrite(&offset, sizeof(unsigned), 1, file);

    const unsigned numConstsSize = numConsts.size();
    fwrite(&numConstsSize, sizeof(unsigned), 1, file);
    for (double num : numConsts) {
        fwrite(&num, sizeof(double), 1, file);
    }

    const unsigned stringConstsSize = stringConsts.size();
    fwrite(&stringConstsSize, sizeof(unsigned), 1, file);
    for (std::string str: stringConsts) {
        for (char c : str) {
            fwrite(&c, sizeof(char), 1, file);
        }
        fwrite(&nullTerminator, sizeof(char), 1, file);
    }

    const unsigned namedLibFunctionsSize = namedLibfuncs.size();
    fwrite(&namedLibFunctionsSize, sizeof(unsigned), 1, file);
    for (std::string f : namedLibfuncs) {
        for (char c : f) {
            fwrite(&c, sizeof(char), 1, file);
        }
        fwrite(&nullTerminator, sizeof(char), 1, file);
    }

    const unsigned userFunctionsSize = userFuncs.size();
    fwrite(&userFunctionsSize, sizeof(unsigned), 1, file);
    for (const auto f : userFuncs) {
        std::string id = f->id;
        for (char c : id) {
            fwrite(&c, sizeof(char), 1, file);
        }
        fwrite(&nullTerminator, sizeof(char), 1, file);

        fwrite(&f->address, sizeof(unsigned), 1, file);
        fwrite(&f->localSize, sizeof(unsigned), 1, file);
    }

    const unsigned instructionsSize = instructions.size();
    fwrite(&instructionsSize, sizeof(unsigned), 1, file);
    for (const auto instruction : instructions) {
        fwrite(&instruction->opcode, sizeof(vmopcode), 1, file);

        fwrite(&instruction->result.empty, sizeof(unsigned), 1, file);
        if (!instruction->result.empty) {
            fwrite(&instruction->result.type, sizeof(vmarg_t), 1, file);
            fwrite(&instruction->result.val, sizeof(unsigned), 1, file);
        }

        fwrite(&instruction->arg1.empty, sizeof(unsigned), 1, file);
        if (!instruction->arg1.empty) {
            fwrite(&instruction->arg1.type, sizeof(vmarg_t), 1, file);
            fwrite(&instruction->arg1.val, sizeof(unsigned), 1, file);
        }

        fwrite(&instruction->arg2.empty, sizeof(unsigned), 1, file);
        if (!instruction->arg2.empty) {
            fwrite(&instruction->arg2.type, sizeof(vmarg_t), 1, file);
            fwrite(&instruction->arg2.val, sizeof(unsigned), 1, file);
        }

        fwrite(&instruction->srcLine, sizeof(unsigned), 1, file);
    }

    fclose(file);
}
