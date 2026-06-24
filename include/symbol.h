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
    SOF, // start of file
    _EOF, // end of file
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

typedef std::variant<NonTerminal, Terminal> SymbolType;
typedef std::vector<SymbolType> Expansion;

struct LexerToken {
    Terminal token_type;
    std::string lexeme;
};

typedef std::vector<LexerToken> Tokens;

#endif 
