//
// Created by mikegriniezakis on 24/3/2024.
//

#ifndef SYMBOL_H
#define SYMBOL_H
#include <string>
#include <utility>
#include <vector>

enum SymbolType {
    GLOBAL,
    LOCAL,
    FORMAL,
    USERFUNC,
    LIBFUNC
};

class Symbol {
    std::string name;
    const uint scope;
    const SymbolType type;
    std::vector<Symbol> arguments;
    bool active;
public:
    Symbol(std::string name, const uint scope, const SymbolType type, std::vector<Symbol> arguments) :
    name(std::move(name)),
    scope(scope),
    type(type),
    arguments(std::move(arguments)) {
        this->active = true;
    }

    std::string getName();
    uint getScope() const;
    SymbolType getType();
    std::vector<Symbol> getArguments();
    bool isActive() const;
    void setActive(bool active);
};

#endif //SYMBOL_H
