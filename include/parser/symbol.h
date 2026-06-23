#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include <string>
#include <vector>

enum SymbolType {
    START_SYMBOL,
    FUNC_DFNS,
    FUNC_DFN,
    STATEMENTS,
    STATEMENT,
    RAW_STATEMENT,
    DECLARE_STATEMENT,
    ID_START_STATEMENT,
    ASSIGN_STATEMENT,
    FUNC_CALL_STATEMENT,
    EXPR,
    TERM,
    FACTOR,
    PARAMS,
    PARAM,

    // end of non-terminals 
    END_NON_TERMINALS,

    _EPSILON,
    _IDENTIFIER,
    _NUMBER,
    _ASSIGN,
    _PERIOD,
    _PARAM_OPEN_PAREN,
    _PARAM_CLOSE_PAREN,
    _FUNC_AT,
    _STRING_LITERAL,
    _COMMA,
    _FLAP,
    _START,
    _END,
    
    // special $ character
    _END_SYMBOL,

    // end of symbol type 
    END_ENUM
};

constexpr inline const bool is_valid_symbol(SymbolType st) {
    return st != END_NON_TERMINALS && st != END_ENUM;
}

constexpr inline const bool is_terminal(SymbolType st) {
    return st > SymbolType::END_NON_TERMINALS;
}

typedef std::vector<SymbolType> Expansion;

#endif 
