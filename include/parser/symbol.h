#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include <set>
#include <string>
#include <unordered_set>

enum SymbolType {
    START,
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

    IDENTIFIER,
    START,
    END,
    ASSIGN,
    EQUALS,
    NUMBER,
    PERIOD,
    COMMA,
    PARAM_OPEN_PAREN,
    PARAM_CLOSE_PAREN,
    FLAP,
    FUNC_AT,
    STRING_LITERAL,

    // end of symbol type 
    END_ENUM
};

constexpr inline const bool is_terminal(SymbolType st) {
    return st > SymbolType::END_NON_TERMINALS;
}

typedef std::vector<SymbolType> Expansion;

#endif 
