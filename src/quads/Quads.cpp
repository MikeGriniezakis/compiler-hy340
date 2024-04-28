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

void Quads::emit(iopcode code, expr* result, expr* arg1, expr* arg2, unsigned label, unsigned line) {
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

SymbolStruct* Quads::createTemp() {
    auto temp = new SymbolStruct();
    char* name = (char*)malloc(10);
    sprintf(name, "_t%d", this->tempCounter++);
    temp->name = strdup(name);

    return temp;
}
