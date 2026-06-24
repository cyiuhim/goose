#ifndef __STATE_LIST_H__
#define __STATE_LIST_H__

#include <cctype>
#include <string>

#include <symbol.h>

#include "state.h"


inline State empty_state;
inline State langle_state;
inline State langle2_state; 
inline State rangle_state;
inline State rangle2_state;
inline State equal_state;
inline State period_state;
inline State identifier_state;
inline State at_state;
inline State number_state;
inline State hash_state;
inline State comment_state;
inline State newline_state;
inline State space_state;
inline State double_quote_state;
inline State quote_end_state;
inline State comma_state;

struct Transition {
    State* begin_state;
    State* end_state;
    std::function<bool(char)> condition;    
};

inline Transition transitions[] = {
    {&empty_state, &hash_state, [](char c) -> bool {
        return c == '#';
    }},
    {&hash_state, &comment_state, [](char c) -> bool {
        return c == '#';
    }},
    {&comment_state, &comment_state, [](char c) -> bool {
        return c != '\n';
    }},
    {&empty_state, &langle_state, [](char c) -> bool {
        return c == '<';
    }},
    {&langle_state, &langle2_state, [](char c) -> bool {
        return c == '<';
    }},
    {&empty_state, &rangle_state, [](char c) -> bool {
        return c == '>';
    }},
    {&rangle_state, &rangle2_state, [](char c) -> bool {
        return c == '>';
    }},
    {&empty_state, &equal_state, [](char c) -> bool {
        return c == '=';
    }},
    {&empty_state, &space_state, [](char c) -> bool {
        return c == ' ' || c == '\n' || c == '\r' || c == '\t';
    }},
    {&space_state, &space_state, [](char c) -> bool {
        return c == ' ' || c == '\n' || c == '\r' || c == '\t';
    }},
    {&empty_state, &identifier_state, [](char c) -> bool {
        return isalpha(c) || c == '_';
    }},
    {&identifier_state, &identifier_state, [](char c) -> bool {
        return isalnum(c) || c == '_';
    }},
    {&empty_state, &number_state, [](char c) -> bool {
        return isdigit(c);
    }},
    {&number_state, &number_state, [](char c) -> bool {
        return isdigit(c);
    }},
    {&empty_state, &at_state, [](char c) -> bool {
        return c == '@';
    }},
    {&empty_state, &period_state, [](char c) -> bool {
        return c == '.';
    }},
    {&empty_state, &double_quote_state, [](char c) -> bool {
        return c == '\"';
    }},
    {&double_quote_state, &double_quote_state, [](char c) -> bool {
        return c != '\n' && c != '\"';
    }},
    {&double_quote_state, &quote_end_state, [](char c) -> bool {
        return c == '\"';
    }},
    {&empty_state, &comma_state, [](char c) -> bool {
        return c == ',';
    }}
};

void populate_state_list();

inline std::string goose_alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_#<>.,@= \n\t\r'\"";

inline std::vector<std::pair<State*, Terminal>> goose_accepting_state_map = {
    {&comment_state, COMMENT},
    {&langle2_state, PARAM_OPEN_PAREN},
    {&rangle2_state, PARAM_CLOSE_PAREN},
    {&equal_state, ASSIGN},
    {&space_state, SPACES},
    {&identifier_state, IDENTIFIER},
    {&number_state, NUMBER},
    {&at_state, FUNC_AT}, 
    {&period_state, PERIOD},
    {&quote_end_state, STRING_LITERAL},
    {&comma_state, COMMA}
};

inline std::vector<std::pair<std::string, Terminal>> goose_keywords = {
    {"flap", FLAP},
    {"start", START},
    {"end", END}
};

#endif 
