#ifndef __GRAMMAR_H__
#define __GRAMMAR_H__

#include <symbol.h>

#include <unordered_map>

typedef std::unordered_map<NonTerminal, std::vector<Expansion>> Grammar;

inline Grammar goose_grammar = {
    {START_SYMBOL, {
        {FUNC_DFNS}
    }},
    {FUNC_DFNS, {
        {FUNC_DFN, FUNC_DFNS}, 
        {EPSILON}
    }},
    {FUNC_DFN, {
        {START, FUNC_AT, IDENTIFIER, FUNC_AT, STATEMENTS, END, FUNC_AT, IDENTIFIER, FUNC_AT}
    }},
    {STATEMENTS, {
        {STATEMENT, STATEMENTS}, 
        {EPSILON}
    }},
    {STATEMENT, {
        {RAW_STATEMENT, PERIOD}
    }},
    {RAW_STATEMENT, {
        {DECLARE_STATEMENT},
        {IDENTIFIER, ID_START_STATEMENT},
    }},
    {ID_START_STATEMENT, {
        {ASSIGN_STATEMENT},
        {FUNC_CALL_STATEMENT}
    }},
    {DECLARE_STATEMENT, {
        {FLAP, IDENTIFIER, ASSIGN, EXPR}
    }},
    {ASSIGN_STATEMENT, {
        {ASSIGN, EXPR}
    }}, 
    {FUNC_CALL_STATEMENT, {
        {PARAM_OPEN_PAREN, PARAMS, PARAM_CLOSE_PAREN}
    }},
    {EXPR, {
        {TERM}
    }},
    {TERM, {
        {FACTOR}
    }},
    {FACTOR, {
        {NUMBER}
    }},
    {PARAMS, {
        {PARAM, COMMA, PARAMS}, 
        {EPSILON}
    }},
    {PARAM, {
        {STRING_LITERAL},
        {NUMBER}
    }}
};


#endif 