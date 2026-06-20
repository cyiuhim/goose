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
    ASSIGN_STATEMENT,
    FUNC_CALL_STATEMENT,
    EXPR,
    TERM,
    FACTOR,
    PARAMS,
    PARAM,

    // end of non-terminals 
    END_NON_TERMINALS,

    EPSILON,
    IDENTIFIER,
    NUMBER,
    ASSIGN,
    PERIOD,
    PARAM_OPEN_PAREN,
    PARAM_CLOSE_PAREN,
    FUNC_AT,
    STRING_LITERAL,
    COMMA,
    FLAP,
    START,
    END,
    
    // special $ character
    END_SYMBOL,

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
