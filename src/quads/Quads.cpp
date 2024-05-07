//
// Created by mikegriniezakis on 20/4/2024.
//

#include "Quads.h"
#include <cstring>
#include <sstream>

void printExpr(std::stringstream* ss, expr* expr) {
    if (expr == nullptr) {
        *ss << "\t";
        return;
    }

    switch (expr->type) {
        case constnum_e:
            *ss << expr->numConst << "\t";
            break;
        case boolexpr_e:
        case constbool_e:
            if (expr->boolConst) {
                *ss << "true\t";
            } else {
                *ss << "false\t";
            }
            break;
        case conststring_e:
            *ss << expr->strConst << "\t";
            break;
        case nil_e:
            *ss << "nil\t";
            break;
        default:
            if (expr->symbol != nullptr) {
                *ss << expr->symbol->name << "\t";
            } else {
                fprintf(stderr, "Error: Symbol with type %s is null\n", iopCodesLabels[expr->type].c_str());
            }
            break;
    }
}

void Quad::print() {
    std::stringstream ss;

    ss << iopCodesLabels[this->code].c_str() << "\t";
    printExpr(&ss, this->result);
    printExpr(&ss, this->arg1);
    printExpr(&ss, this->arg2);
    ss << this->label << "\t";

    printf("%s\n", ss.str().c_str());
}

void Quad::setLabel(unsigned label) {
    this->label = label;
}

void Quads::emit(iopcode code, expr* result, expr* arg1, expr* arg2, unsigned label, unsigned line) {
    if (label == 0) {
        label = this->nextQuad();
    }

    Quad* quad = new Quad(code, result, arg1, arg2, label, line);

    this->quads.push_back(quad);
}

expr* Quads::newExpr(expr_t type) {
    auto expr = new struct expr;
    expr->type = type;

    return expr;
}

void Quads::printQuads() {
    printf("\n\nQuad#\tOpcode\tResult\tArg1\tArg2\tLabel\n");
    for (int i = 0; i < this->quads.size(); i++) {
        auto quad = this->quads[i];
        printf("%d\t", i);
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

expr* Quads::emitIfTableItem(expr* expr, unsigned line, int offset) {
    if (expr->type != tableitem_e) {
        return expr;
    }

    auto res = this->newExpr(var_e);
    res->symbol = this->createTemp(offset);

    emit(tablegetelem_op, res, expr, expr->index, 0, line);

    return res;
}


expr* Quads::makeMember(expr* lvalue, char* name, unsigned line, int offset) {
    lvalue = this->emitIfTableItem(lvalue, line, offset);
    auto expr = newExpr(tableitem_e);
    expr->index = newExpr(conststring_e);
    expr->index->strConst = strdup(name);
    expr->symbol = lvalue->symbol;

    return expr;
}

expr* Quads::makeCall(expr* call, expr* elist, unsigned line, int offset) {
    expr* func = this->emitIfTableItem(call, line, offset);

    expr* arg = elist;
    while (arg != nullptr) {
        this->emit(param_op, arg, nullptr, nullptr, 0, line);
        arg = arg->next;
    }

    emit(call_op, func, nullptr, nullptr, 0, line);
    expr* res = this->newExpr(var_e);
    res->symbol = this->createTemp(offset);
    emit(getretval_op, res, nullptr, nullptr, 0, line);

    return res;
}


SymbolStruct* Quads::createTemp(int offset) {
    auto temp = new SymbolStruct();
    char* name = (char *) malloc(10);
    sprintf(name, "_t%d", this->tempCounter++);
    temp->name = strdup(name);
    temp->offset = offset;

    this->symbolTable->insertSymbol(temp->name, 0, false, false, 0, offset);

    return temp;
}

unsigned Quads::nextQuad() {
    return this->quads.size();
}

void Quads::patchLabel(unsigned quad, unsigned label) {
    this->quads[quad]->setLabel(label);
}
