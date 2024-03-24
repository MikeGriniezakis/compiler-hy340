//
// Created by mikegriniezakis on 24/3/2024.
//

#include "SymbolTable.h"

SymbolTable::SymbolTable() {
    // add scope for global variables
    this->scopes.emplace_back();

    this->initializeSTDFunctions();
}

void SymbolTable::initializeSTDFunctions() {
    std::string libraryFunctions[] = {
        "print",
        "input",
        "objectmemberkeys",
        "objecttotalkeys",
        "objectcopy",
        "totalarguments",
        "argument",
        "typeof",
        "strtonum",
        "sqrt",
        "cos",
        "sin"
    };

    for (const auto &functionName : libraryFunctions) {
        Symbol symbol(functionName, 0, 0, LIBFUNC, {});
        insertSymbol(symbol);
    }
}

void SymbolTable::incScope() {
    this->scope++;

    this->scopes.emplace_back();
}

void SymbolTable::decScope() {
    if (this->scope == 0) {
        fprintf(stderr, "Cannot decrease scope below 0\n");
        return;
    }

    this->scope--;
    this->deactivateSymbols(this->scope);
}

void SymbolTable::insertSymbol(Symbol symbol) {
    this->symbolTable[symbol.getName()].push_back(symbol);
    this->scopes.at(this->scope).push_back(symbol);
}

void SymbolTable::deactivateSymbols(const uint scope) {
    for (auto &symbol : this->scopes.at(scope)) {
        symbol.setActive(false);
    }
}

void SymbolTable::printSymbolTable() {
    for (int i = 0; i < this->scopes.size(); i++) {
        printf("----------    Scope %d    ----------\n\n", i);

        for (auto &symbol : this->scopes.at(i)) {
            symbol.print();
        }
    }
}

Symbol* SymbolTable::lookupSymbol(const std::string& name) {
    if (this->symbolTable.find(name) == this->symbolTable.end()) {
        return nullptr;
    }

    for (auto &symbol : this->symbolTable[name]) {
        if (symbol.isActive()) {
            return &symbol;
        }
    }

    return nullptr;
}
