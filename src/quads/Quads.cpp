//
// Created by mikegriniezakis on 20/4/2024.
//

#include "Quads.h"

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
        printf("%d\t%s\t%s\t%s\t%s\t%d\n",
            i,
            iopCodesLabels[quad->getCode()].c_str(),
            quad->getResult()->symbol->name,
            quad->getArg1() != nullptr ? quad->getArg1()->symbol->name : "\t",
            quad->getArg2() != nullptr ? quad->getArg1()->symbol->name : "\t",
            quad->getLabel()
        );
    }
}
