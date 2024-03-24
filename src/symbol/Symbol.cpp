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
    }

    printf("\"%s\" %s (line %u) (scope %u)\n", this->name.c_str(), typeString.c_str(), this->line, this->scope);
}
