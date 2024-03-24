#ifndef SYMBOL_H
#define SYMBOL_H
#include <string>
#include <utility>
#include <vector>

enum SymbolType {
    GLOBAL,
    SCOPED,
    USERFUNC,
    LIBFUNC
};

class Symbol {
    std::string name;
    const uint scope;
    const uint line;
    const SymbolType type;
    std::vector<Symbol> arguments;
    bool active;
public:
    Symbol(std::string name, const uint scope, const uint line, const SymbolType type, std::vector<Symbol> arguments) :
    name(std::move(name)),
    scope(scope),
    line(line),
    type(type),
    arguments(std::move(arguments)) {
        this->active = true;
    }

    std::string getName();
    bool isActive() const;
    void setActive(bool active);

    void print();
};

#endif //SYMBOL_H
