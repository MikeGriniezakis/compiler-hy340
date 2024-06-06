#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <map>
#include <vector>
#include "../symbol/Symbol.h"

class SymbolTable {
    uint scope = 0;
    uint varOffset = 0;
    uint formalOffset = 0;
    uint functionLocalOffset = 0;
    uint scopeSpace = 1;
    std::map<std::string, std::vector<Symbol*>> symbolTable;
    std::vector<std::vector<Symbol*>> scopes;
    std::string libraryFunctions[12] = {
        "print",
        "typeof",
        "totalarguments",
        "argument",
        "input",
        "objectmemberkeys",
        "objecttotalkeys",
        "objectcopy",
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

    uint getScope() const;

    ScopeSpace currScopeSpace();
    uint currScopeOffset();
    void incCurrScopeOffset();
    void enterScopeSpace();
    void exitScopeSpace();
    void printSymbolTable();
    void resetFormalScope();
    unsigned getVarOffset();
    Symbol* insertSymbol(std::string name, uint line, bool isFunction, bool isFormal, uint functionScope);
    Symbol* lookupSymbol(const std::string& name, int scope);
    Symbol* lookupSymbolScoped(const std::string& name);
    Symbol* lookupSymbolGlobal(const std::string& name);
    void deactivateSymbols(uint scope);
    bool isNameReserved(const std::string& name);
};

#endif //SYMBOLTABLE_H
