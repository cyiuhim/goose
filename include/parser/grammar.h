#ifndef __GRAMMAR_H__
#define __GRAMMAR_H__

#include "symbol.h"

#include <unordered_map>

typedef std::unordered_map<SymbolType, std::vector<Expansion>> Grammar;

inline Grammar goose_grammar = {
    {START_SYMBOL, {
        {FUNC_DFNS}
    }},
    {FUNC_DFNS, {
        {FUNC_DFN, FUNC_DFNS}, 
        {_EPSILON}
    }},
    {FUNC_DFN, {
        {_START, _FUNC_AT, _IDENTIFIER, _FUNC_AT, STATEMENTS, _END, _FUNC_AT, _IDENTIFIER, _FUNC_AT}
    }},
    {STATEMENTS, {
        {STATEMENT, STATEMENTS}, 
        {_EPSILON}
    }},
    {STATEMENT, {
        {RAW_STATEMENT, _PERIOD}
    }},
    {RAW_STATEMENT, {
        {DECLARE_STATEMENT},
        {_IDENTIFIER, ID_START_STATEMENT},
    }},
    {ID_START_STATEMENT, {
        {ASSIGN_STATEMENT},
        {FUNC_CALL_STATEMENT}
    }},
    {DECLARE_STATEMENT, {
        {_FLAP, _IDENTIFIER, _ASSIGN, EXPR}
    }},
    {ASSIGN_STATEMENT, {
        {_ASSIGN, EXPR}
    }}, 
    {FUNC_CALL_STATEMENT, {
        {_PARAM_OPEN_PAREN, PARAMS, _PARAM_CLOSE_PAREN}
    }},
    {EXPR, {
        {TERM}
    }},
    {TERM, {
        {FACTOR}
    }},
    {FACTOR, {
        {_NUMBER}
    }},
    {PARAMS, {
        {PARAM, _COMMA, PARAMS}, 
        {_EPSILON}
    }},
    {PARAM, {
        {_STRING_LITERAL},
        {_NUMBER}
    }}
};


#endif 