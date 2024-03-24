//
// Created by mikegriniezakis on 24/3/2024.
//

#include "Symbol.h"

std::string Symbol::getName() {
    return name;
}

uint Symbol::getScope() const {
    return scope;
}

SymbolType Symbol::getType() {
    return type;
}

std::vector<Symbol> Symbol::getArguments() {
    return arguments;
}

bool Symbol::isActive() const {
    return active;
}

void Symbol::setActive(bool active) {
    this->active = active;
}
