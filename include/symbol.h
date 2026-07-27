#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include <variant>
#include <vector>
#include <string>

enum NonTerminal
{
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
    RETURN_STATEMENT,
    EXPR,
    EXPR_TAIL,
    TERM,
    TERM_TAIL,
    FACTOR,
    FUNC_CALL,
    ARGS,
    ARGS_TAIL,
    ARG,
    PARAMS,
    PARAM,
    TYPE_NAME,

    // end of non-terminals
    END_NON_TERMINALS
};

enum Terminal
{
    IDENTIFIER,      // any identifier
    NUMBER_LITERAL,  // number
    ASSIGN,          // =
    PERIOD,          // end of statement
    ARG_OPEN_PAREN,  // <<
    ARG_CLOSE_PAREN, // >>
    OPEN_PAREN,      // (
    CLOSE_PAREN,     // )
    FUNC_AT,         // function at symbol, @
    COMMENT,         // comment
    SPACES,          // one or more spaces
    STRING_LITERAL,  // wrapped inside a pair of double quotes
    COMMA,           // ,
    PLUS,            // +
    MINUS,           // -
    TIMES,           // *
    DIVIDE,          // /
    MOD,             // %

    // keywords
    FLAP,   // flap (let)
    START,  // start
    END,    // end
    NUMBER, // number
    WORD,   // word
    RETURN, // return

    // special symbols
    EPSILON,
    END_SYMBOL, // $

    END_TERMINALS
};

/*
 * for purposes of error messages only, so that we're not printing out a number for the enum but rather mapped to the token types
 */
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
    "RETURN_STATEMENT",
    "EXPR",
    "EXPR_TAIL",
    "TERM",
    "TERM_TAIL",
    "FACTOR",
    "FUNC_CALL",
    "ARGS",
    "ARGS_TAIL",
    "ARG",
    "PARAMS",
    "PARAM",
    "TYPE_NAME"};

/*
 * for purposes of error messages only, so that we're not printing out a number for the enum but rather mapped to the token types
 */
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
    "+",
    "-",
    "*",
    "/",
    "%",
    "flap",
    "start",
    "end",
    "number",
    "word",
    "return",
    "epsilon",
    "end of file"};

inline std::string to_string(Terminal terminal)
{
    return terminal_strings.at(static_cast<int>(terminal));
}

inline std::string to_string(NonTerminal non_terminal)
{
    return non_terminal_strings.at(static_cast<int>(non_terminal));
}

typedef std::variant<NonTerminal, Terminal> SymbolType;
typedef std::vector<SymbolType> Expansion;

struct LexerToken
{
    Terminal token_type;
    std::string lexeme;
    unsigned int line_no;
    unsigned int char_no;
};

typedef std::vector<LexerToken> Tokens;

#endif
