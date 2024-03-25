//
// Created by mikegriniezakis on 24/3/2024.
//

#include "SymbolTable.h"

#include <utility>

SymbolTable::SymbolTable() {
    // add scope for global variables
    this->scopes.emplace_back();

    this->initializeSTDFunctions();
}

void SymbolTable::initializeSTDFunctions() {
    for (const auto &functionName : this->libraryFunctions) {
        insertSymbol(functionName, 0, LIBFUNC, {});
    }
}

void SymbolTable::incScope() {
    this->scope++;

    if (this->scopes.size() == this->scope) {
        this->scopes.emplace_back();
    }
}

void SymbolTable::decScope() {
    if (this->scope == 0) {
        fprintf(stderr, "Cannot decrease scope below 0\n");
        return;
    }

    this->deactivateSymbols(this->scope);
    this->scope--;
}

Symbol* SymbolTable::insertSymbol(std::string name, uint line, bool isFunction, std::vector<Symbol> arguments) {
    SymbolType type;
    if (isFunction) {
        bool foundLib = false;
        for (auto &func : this->libraryFunctions) {
            if (name == func) {
                type = LIBFUNC;
                foundLib = true;
                break;
            }
        }

        if (foundLib == false) {
            type = USERFUNC;
        }
    } else {
        if (this->scope == 0) {
            type = GLOBAL;
        } else {
            type = SCOPED;
        }
    }
    auto* symbol = new Symbol(std::move(name), this->scope, line, type, std::move(arguments));

    this->symbolTable[symbol->getName()].push_back(symbol);
    this->scopes.at(this->scope).push_back(symbol);

    return symbol;
}

void SymbolTable::deactivateSymbols(const uint scope) {
    for (auto &symbol : this->scopes.at(scope)) {
        symbol->setActive(false);
    }
}

void SymbolTable::printSymbolTable() {
    for (int i = 0; i < this->scopes.size(); i++) {
        printf("\n----------    Scope #%d    ----------\n", i);

        for (auto &symbol : this->scopes.at(i)) {
            symbol->print();
        }
    }
}

Symbol* SymbolTable::lookupSymbol(const std::string& name) {
    if (this->symbolTable.find(name) == this->symbolTable.end()) {
        return nullptr;
    }

    for (auto &symbol : this->symbolTable[name]) {
        if (symbol->isActive()) {
            return symbol;
        }
    }

    return nullptr;
}

Symbol* SymbolTable::lookupSymbolScoped(const std::string& name) {
    for (auto &symbol : this->scopes.at(this->scope)) {
        if (symbol->getName() == name && symbol->isActive()) {
            return symbol;
        }
    }

    return nullptr;
}
