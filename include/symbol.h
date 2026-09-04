#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include <variant>
#include <vector>
#include <string>
#include <unordered_map>

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
    FACTOR_TAIL,
    FUNC_CALL,
    ARGS,
    ARGS_TAIL,
    ARG,
    PARAMS,
    PARAMS_TAIL,
    PARAM,
    TYPE_NAME,
    OPTIONAL_RETURN_TAIL,

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
    SINGLE_ARROW,    // ->

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
static std::unordered_map<NonTerminal, std::string> non_terminal_strings = {
    {START_SYMBOL, "START_SYMBOL"},
    {FUNC_DFNS, "FUNC_DFNS"},
    {FUNC_DFN, "FUNC_DFN"},
    {STATEMENTS, "STATEMENTS"},
    {STATEMENT, "STATEMENT"},
    {RAW_STATEMENT, "RAW_STATEMENT"},
    {DECLARE_STATEMENT, "DECLARE_STATEMENT"},
    {ID_START_STATEMENT, "ID_START_STATEMENT"},
    {ASSIGN_STATEMENT, "ASSIGN_STATEMENT"},
    {FUNC_CALL_STATEMENT, "FUNC_CALL_STATEMENT"},
    {RETURN_STATEMENT, "RETURN_STATEMENT"},
    {EXPR, "EXPR"},
    {EXPR_TAIL, "EXPR_TAIL"},
    {TERM, "TERM"},
    {TERM_TAIL, "TERM_TAIL"},
    {FACTOR, "FACTOR"},
    {FACTOR_TAIL, "FACTOR_TAIL"},
    {FUNC_CALL, "FUNC_CALL"},
    {ARGS, "ARGS"},
    {ARGS_TAIL, "ARGS_TAIL"},
    {ARG, "ARG"},
    {PARAMS, "PARAMS"},
    {PARAMS_TAIL, "PARAMS_TAIL"},
    {PARAM, "PARAM"},
    {TYPE_NAME, "TYPE_NAME"},
    {OPTIONAL_RETURN_TAIL, "OPTIONAL_RETURN_TAIL"}};

/*
 * for purposes of error messages only, so that we're not printing out a number for the enum but rather mapped to the token types
 */
static std::unordered_map<Terminal, std::string> terminal_strings = {
    {IDENTIFIER, "identifier"},
    {NUMBER_LITERAL, "number literal"},
    {ASSIGN, "="},
    {PERIOD, "."},
    {ARG_OPEN_PAREN, "<<"},
    {ARG_CLOSE_PAREN, ">>"},
    {OPEN_PAREN, "("},
    {CLOSE_PAREN, ")"},
    {FUNC_AT, "@"},
    {STRING_LITERAL, "string literal"},
    {COMMA, ","},
    {PLUS, "+"},
    {MINUS, "-"},
    {TIMES, "*"},
    {DIVIDE, "/"},
    {MOD, "%"},
    {SINGLE_ARROW, "->"},
    {FLAP, "flap"},
    {START, "start"},
    {END, "end"},
    {NUMBER, "number"},
    {WORD, "word"},
    {RETURN, "return"},
    {EPSILON, "epsilon"},
    {END_SYMBOL, "end of file"}};

inline std::string to_string(Terminal terminal)
{
    return terminal_strings.at(terminal);
}

inline std::string to_string(NonTerminal non_terminal)
{
    return non_terminal_strings.at(non_terminal);
}

using SymbolType = std::variant<NonTerminal, Terminal>;
using Expansion = std::vector<SymbolType>;

struct LexerToken
{
    Terminal token_type;
    std::string lexeme;
    unsigned int line_no;
    unsigned int char_no;
};

using Tokens = std::vector<LexerToken>;

#endif
