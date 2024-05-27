#ifndef SYMBOL_H
#define SYMBOL_H
#include <string>
#include <utility>

enum SymbolType {
    ASSIGNMENT,
    GLOBAL,
    SCOPED,
    FORMAL,
    USERFUNC,
    LIBFUNC
};

enum ScopeSpace {
    PROGRAM_VAR,
    FUNCTION_LOCAL,
    FORMAL_ARG
};

enum SymbolClass {
    FUNC,
    VAR
};

struct SymbolStruct {
    char* name;
    int scope;
    int line;
    int offset;
    unsigned tAddress;
    ScopeSpace scope_space;
    SymbolClass symbol_class;
    SymbolType type;
};

class Symbol {
    bool active;
    std::string name;
    const uint scope;
    const uint offset;
    const uint function_scope;
    const uint line;
    const unsigned tAddress;
    const ScopeSpace scope_space;
    const SymbolType type;
    const SymbolClass symbol_class;
public:
    Symbol(std::string name, const uint scope, const uint line, const SymbolType type, const SymbolClass symbol_class, const uint function_scope, const uint offset, ScopeSpace scope_space) :
    name(std::move(name)),
    scope(scope),
    function_scope(function_scope),
    line(line),
    symbol_class(symbol_class),
    offset(offset),
    scope_space(scope_space),
    tAddress(0),
    type(type) {
        this->active = true;
    }

    SymbolStruct* toStruct();

    std::string getName();
    bool isActive() const;
    uint getOffset();
    uint getScope();
    uint getFunctionScope();
    SymbolType getType() const;
    SymbolClass getSymbolClass() const;
    void setActive(bool active);

    void print();
};

#endif //SYMBOL_H
