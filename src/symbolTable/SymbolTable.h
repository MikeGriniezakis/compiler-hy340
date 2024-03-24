#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <map>
#include <vector>
#include "../symbol/Symbol.h"

class SymbolTable {
    uint scope = 0;
    std::map<std::string, std::vector<Symbol*>> symbolTable;
    std::vector<std::vector<Symbol*>> scopes;
    std::string libraryFunctions[12] = {
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

    void initializeSTDFunctions();
public:
    SymbolTable();

    void incScope();
    void decScope();

    void printSymbolTable();
    Symbol* insertSymbol(std::string name, uint line, bool isFunction, std::vector<Symbol> arguments);
    Symbol* lookupSymbol(const std::string& name);
    void deactivateSymbols(uint scope);
};

#endif //SYMBOLTABLE_H
