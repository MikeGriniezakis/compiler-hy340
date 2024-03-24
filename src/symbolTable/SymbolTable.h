#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <map>
#include <vector>
#include "../symbol/Symbol.h"

class SymbolTable {
    uint scope = 0;
    std::map<std::string, std::vector<Symbol>> symbolTable;
    std::vector<std::vector<Symbol>> scopes;

    void initializeSTDFunctions();
public:
    SymbolTable();

    void incScope();
    void decScope();

    void printSymbolTable();
    void insertSymbol(Symbol symbol);
    Symbol* lookupSymbol(const std::string& name);
    void deactivateSymbols(uint scope);
};

#endif //SYMBOLTABLE_H
