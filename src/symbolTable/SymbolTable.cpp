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
        insertSymbol(functionName, 0, true, false, 0, 0);
    }
}

bool SymbolTable::isNameReserved(const std::string& name) {
    return name == "print" || name == "input" || name == "objectmemberkeys" || name == "objecttotalkeys" ||
           name == "objectcopy" || name == "totalarguments" || name == "argument" || name == "typeof" ||
           name == "strtonum" || name == "sqrt" || name == "cos" || name == "sin";
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

Symbol* SymbolTable::insertSymbol(std::string name, uint line, bool isFunction, bool isFormal, uint functionScope, uint offset) {
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
        if (isFormal) {
            type = FORMAL;
        } else if (this->scope == 0) {
            type = GLOBAL;
        } else {
            type = SCOPED;
        }
    }
    ScopeSpace space;
    if (isFormal) {
        space = FORMAL_ARG;
    } else if (isFunction) {
        space = FUNCTION_LOCAL;
    } else {
        space = PROGRAM_VAR;
    }

    auto* symbol = new Symbol(std::move(name), this->scope, line, type, isFunction ? FUNC : VAR, functionScope, offset, space);

    this->symbolTable[symbol->getName()].push_back(symbol);
    this->scopes.at(this->scope).push_back(symbol);

    return symbol;
}

void SymbolTable::deactivateSymbols(const uint scope) {
    for (auto &symbol : this->scopes.at(scope)) {
        symbol->setActive(false);
    }
}

uint SymbolTable::getScope() const {
    return this->scope;
}

void SymbolTable::printSymbolTable() {
    for (int i = 0; i < this->scopes.size(); i++) {
        if (this->scopes.at(i).empty()) {
            continue;
        }

        printf("\n----------    Scope #%d    ----------\n", i);

        for (auto &symbol : this->scopes.at(i)) {
            symbol->print();
        }
    }
}

Symbol* SymbolTable::lookupSymbol(const std::string& name, int scope) {
    if (this->symbolTable.find(name) == this->symbolTable.end()) {
        return nullptr;
    }

    if (scope == -1) {
        return nullptr;
    }

    for (auto &symbol : this->scopes.at(scope)) {
        if (symbol->isActive() && symbol->getName() == name) {
            return symbol;
        }
    }

    return lookupSymbol(name, scope - 1);
}

Symbol* SymbolTable::lookupSymbolGlobal(const std::string& name) {
    for (auto &symbol : this->scopes.at(0)) {
        if (symbol->getName() == name && symbol->isActive()) {
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

    return this->lookupSymbol(name, this->scope);
}
