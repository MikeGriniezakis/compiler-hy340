//
// Created by mikegriniezakis on 24/3/2024.
//

#include "Symbol.h"

std::string Symbol::getName() {
    return this->name;
}

bool Symbol::isActive() const {
    return this->active;
}

ScopeSpace Symbol::getScopeSpace() const {
    return this->scope_space;
}

void Symbol::setActive(bool active) {
    this->active = active;
}

void Symbol::print() {
    std::string typeString;
    switch (this->type) {
        case GLOBAL:
            typeString = "[global variable]";
            break;
        case LIBFUNC:
            typeString = "[library function]";
            break;
        case USERFUNC:
            typeString = "[user function]";
            break;
        case SCOPED:
            typeString = "[local variable]";
            break;
        case FORMAL:
            typeString = "[formal argument]";
            break;
    }

    printf("\"%s\" %s (line %u) (scope %u)\n", this->name.c_str(), typeString.c_str(), this->line, this->scope);
}

SymbolStruct* Symbol::toStruct() {
    auto* symbolStruct = new SymbolStruct();
    symbolStruct->name = const_cast<char*>(this->name.c_str());
    symbolStruct->scope = this->scope;
    symbolStruct->line = this->line;
    symbolStruct->type = this->type;
    symbolStruct->offset = this->offset;
    symbolStruct->scope_space = this->scope_space;
    symbolStruct->tAddress = this->tAddress;
    symbolStruct->localSize = this->localSize;

    return symbolStruct;
}

uint Symbol::getScope() {
    return this->scope;
}

uint Symbol::getFunctionScope() {
    return this->function_scope;
}

uint Symbol::getOffset() {
    return this->offset;
}

SymbolType Symbol::getType() const {
    return this->type;
}

SymbolClass Symbol::getSymbolClass() const {
    return this->symbol_class;
}
