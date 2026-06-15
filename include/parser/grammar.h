#ifndef __GRAMMAR_H__
#define __GRAMMAR_H__

#include <map>
#include "symbol.h"

typedef std::map<SymbolType, std::vector<Expansion>> Grammar;

Grammar goose_grammar = {
    {START, {
        {FUNC_DFNS}
    }}, 
    {FUNC_DFNS, {
        {FUNC_DFN, FUNC_DFNS}, 
        {}
    }},
    {FUNC_DFN, {
        {START, FUNC_AT, IDENTIFIER, FUNC_AT, STATEMENTS, END, FUNC_AT, IDENTIFIER, FUNC_AT}
    }},
    {STATEMENTS, {
        {STATEMENT, STATEMENTS}, 
        {}
    }},
    {STATEMENT, {
        {RAW_STATEMENT, PERIOD}
    }},
    {RAW_STATEMENT, {
        {DECLARE_STATEMENT},
        {ASSIGN_STATEMENT},
        {FUNC_CALL_STATEMENT}
    }},
    {DECLARE_STATEMENT, {
        {FLAP, IDENTIFIER, EQUALS, EXPR}
    }},
    {ASSIGN_STATEMENT, {
        {IDENTIFIER, EQUALS, EXPR}
    }}, 
    {FUNC_CALL_STATEMENT, {
        {IDENTIFIER, PARAM_OPEN_PAREN, PARAMS, PARAM_CLOSE_PAREN}
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
        {}
    }},
    {PARAM, {
        {STRING_LITERAL},
        {NUMBER}
    }}
};


#endif 