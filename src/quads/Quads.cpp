//
// Created by mikegriniezakis on 20/4/2024.
//

#include "Quads.h"
#include <cstring>
#include <iostream>
#include <iomanip>
#include <sstream>

void printExpr(std::stringstream* ss, expr* expr) {
    if (expr == nullptr) {
        *ss << std::setw(15) <<  " ";
        return;
    }

    switch (expr->type) {
        case constnum_e:
            *ss << std::setw(15) << expr->numConst;
            break;
        case constbool_e:
            *ss << std::setw(15) << (expr->boolConst ? "true" : "false");
            break;
        case conststring_e:
            *ss << std::setw(15) << expr->strConst;
            break;
        case nil_e:
            *ss << std::setw(15) << "nil";
            break;
        default:
            if (expr->symbol != nullptr) {
                *ss << std::setw(15) << expr->symbol->name;
            }
            break;
    }
}

void Quad::print() {
    std::stringstream ss;

    ss << std::setw(15) << iopCodesLabels[this->code].c_str();
    printExpr(&ss, this->result);
    printExpr(&ss, this->arg1);
    printExpr(&ss, this->arg2);
    if (this->label != 0 || this->getCode() == jump_op) {
        ss << std::setw(15) << this->label;
    }

    printf("%s\n", ss.str().c_str());
}

void Quad::setLabel(unsigned label) {
    this->label = label;
}

void Quad::setTAddress(unsigned tAddress) {
    this->tAddress = tAddress;
}

unsigned Quad::getTAddress() {
    return this->tAddress;
}

void Quads::emit(iopcode code, expr* result, expr* arg1, expr* arg2, unsigned label, unsigned line) {
    Quad* quad = new Quad(code, result, arg1, arg2, label, line);

    this->quads.push_back(quad);
}

void Quads::emit(int code, expr* result, expr* arg1, expr* arg2, unsigned label, unsigned line) {
    auto opcode = static_cast<iopcode>(code);

    Quad* quad = new Quad(opcode, result, arg1, arg2, label, line);

    this->quads.push_back(quad);
}


expr* Quads::newExpr(expr_t type) {
    auto expr = new struct expr;
    expr->type = type;

    return expr;
}

void Quads::printQuads() {
    std::cout << std::setw(4) << "Quad#" << std::setw(15) << "Opcode" << std::setw(15) << "Result" << std::setw(15) <<
            "Arg1" << std::setw(15) << "Arg2" << std::setw(15) << "Label" << std::endl;
    for (int i = 0; i < this->quads.size(); i++) {
        auto quad = this->quads[i];
        std::cout << std::setw(4) << i;
        quad->print();
    }
}

bool Quads::checkArithmeticExpression(const expr* first, const expr* second) {
    return (first->type != arithexpr_e || first->type != constnum_e) && (
               second->type != arithexpr_e || second->type != constnum_e);
}

bool Quads::checkArithmeticExpression(const expr* expr) {
    return expr->type != arithexpr_e || expr->type != constnum_e;
}

expr* Quads::emitIfTableItem(expr* expr, unsigned line) {
    if (expr->type != tableitem_e) {
        return expr;
    }

    auto res = this->newExpr(var_e);
    res->symbol = this->createTemp();

    emit(tablegetelem_op, res, expr, expr->index, 0, line);

    return res;
}


expr* Quads::makeMember(expr* lvalue, char* name, unsigned line) {
    lvalue = this->emitIfTableItem(lvalue, line);
    auto expr = newExpr(tableitem_e);
    expr->index = newExpr(conststring_e);
    expr->index->strConst = strdup(name);
    expr->symbol = lvalue->symbol;

    return expr;
}

expr* Quads::makeCall(expr* call, expr* elist, unsigned line) {
    expr* func = this->emitIfTableItem(call, line);

    expr* arg = elist;
    while (arg != nullptr) {
        this->emit(param_op, arg, nullptr, nullptr, 0, line);
        arg = arg->next;
    }

    emit(call_op, func, nullptr, nullptr, 0, line);
    expr* res = this->newExpr(var_e);
    res->symbol = this->createTemp();
    emit(getretval_op, res, nullptr, nullptr, 0, line);

    return res;
}


SymbolStruct* Quads::createTemp() {
    auto temp = new SymbolStruct();
    char* name = (char *) malloc(10);
    sprintf(name, "_t%d", this->tempCounter++);
    temp->name = strdup(name);

    Symbol* existingSymbol = this->symbolTable->lookupSymbolScoped(temp->name);
    if (existingSymbol == nullptr || existingSymbol->getScope() != symbolTable->getScope()) {
        Symbol* symbol = this->symbolTable->insertSymbol(temp->name, 0, false, false, 0);
        temp->offset = symbol->getOffset();
        temp->scope = symbol->getScope();
        temp->scope_space = symbol->getScopeSpace();
    }

    return temp;
}

unsigned Quads::nextQuad() {
    return this->quads.size();
}

void Quads::patchLabel(unsigned quad, unsigned label) {
    this->quads[quad]->setLabel(label);
}

void Quads::patchList(int list, int label) {
    while (list) {
        int next = this->quads.at(list)->getLabel();
        quads.at(list)->setLabel(label);
        list = next;
    }
}

std::vector<int> Quads::merge(std::vector<int> list1, std::vector<int> list2) {
    std::vector<int> mergedList = list1;
    mergedList.insert(mergedList.end(), list2.begin(), list2.end());

    return mergedList;
}

Quad* Quads::getQuad(unsigned quad) {
    return this->quads[quad];
}

std::vector<Quad *> Quads::getQuads() {
    return this->quads;
}
