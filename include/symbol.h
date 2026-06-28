#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include <variant>
#include <vector>
#include <string>

enum NonTerminal {
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
    END_NON_TERMINALS
};

enum Terminal {
    IDENTIFIER, // any identifier
    NUMBER, // number
    ASSIGN, // =
    PERIOD, // end of statement
    PARAM_OPEN_PAREN, // <<
    PARAM_CLOSE_PAREN, // >>
    FUNC_AT, // function at symbol, @
    COMMENT, // comment
    SPACES, // one or more spaces
    STRING_LITERAL, // wrapped inside a pair of double quotes
    COMMA, // ,

    // keywords
    FLAP, // flap (let)
    START, // start
    END, // end

    // special symbols 
    EPSILON,
    END_SYMBOL, // $

    END_TERMINALS
};

static std::vector<std::string> non_terminal_strings = {
    "START_SYMBOL",
    "FUNC_DFNS",
    "FUNC_DFN",
    "STATEMENTS",
    "STATEMENT",
    "RAW_STATEMENT",
    "DECLARE_STATEMENT",
    "ID_START_STATEMENT",
    "ASSIGN_STATEMENT",
    "FUNC_CALL_STATEMENT",
    "EXPR",
    "TERM",
    "FACTOR",
    "PARAMS",
    "PARAM",
};

static std::vector<std::string> terminal_strings = {
    "identifier",
    "number",
    "=",
    ".",
    "<<",
    ">>",
    "@",
    "//",
    " ",
    "string literal",
    ",",
    "flap",
    "start",
    "end",
    "epsilon",
    "end of file"
};

inline std::string to_string(Terminal terminal) {
    return terminal_strings.at(static_cast<int> (terminal));
}

inline std::string to_string(NonTerminal non_terminal) {
    return non_terminal_strings.at(static_cast<int> (non_terminal));
}

typedef std::variant<NonTerminal, Terminal> SymbolType;
typedef std::vector<SymbolType> Expansion;

struct LexerToken {
    Terminal token_type;
    std::string lexeme;
};

typedef std::vector<LexerToken> Tokens;

#endif 
