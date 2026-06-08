#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include <set>

enum SymbolType {
    // non-terminals
    START,
    FUNC_DFNS,
    FUNC_DFNS_PRIME,
    FUNC_DFN,
    STATEMENTS,
    STATEMENT,
    RAW_STATEMENT,
    DECLARE_STATEMENT,
    ASSIGN_STATEMENT, 
    HONK_STATEMENT,
    EXPR,
    TERM,
    FACTOR,
    PARAMS,
    PARAM,

    // boundary markers of terminals in enum 
    __FIRST_TERMINAL,
    // terminals 
    _START,
    _SOF,
    _EOF,
    _IDENTIFIER,
    _END,
    _ASSIGN,
    _EQUALS,
    _NUMBER_LITERAL,
    _PERIOD,
    _PARAM_OPEN_PAREN,
    _PARAM_CLOSE_PAREN,
    _HONK,
    _FLAP,
    _STRING_LITERAL,

};

constexpr inline const bool is_terminal(SymbolType st) {
    return st > SymbolType::__FIRST_TERMINAL;
}

typedef std::vector<SymbolType> Expansion;

#endif 
