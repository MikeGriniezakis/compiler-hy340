#ifndef SYMBOL_H
#define SYMBOL_H
#include <string>
#include <utility>
#include <vector>

enum SymbolType {
    ASSIGNMENT,
    GLOBAL,
    SCOPED,
    FORMAL,
    USERFUNC,
    LIBFUNC
};

enum SymbolClass {
    FUNC,
    VAR
};

struct SymbolStruct {
    char* name;
    int scope;
    int line;
    SymbolClass symbol_class;
    SymbolType type;
};

class Symbol {
    bool active;
    std::string name;
    const uint scope;
    const uint function_scope;
    const uint line;
    const SymbolType type;
    const SymbolClass symbol_class;
public:
    Symbol(std::string name, const uint scope, const uint line, const SymbolType type, const SymbolClass symbol_class, const uint function_scope) :
    name(std::move(name)),
    scope(scope),
    function_scope(function_scope),
    line(line),
    symbol_class(symbol_class),
    type(type) {
        this->active = true;
    }

    SymbolStruct* toStruct();

    std::string getName();
    bool isActive() const;
    uint getScope();
    uint getFunctionScope();
    SymbolType getType() const;
    SymbolClass getSymbolClass() const;
    void setActive(bool active);

    void print();
};

#endif //SYMBOL_H
