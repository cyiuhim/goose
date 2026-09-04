#ifndef __STATE_LIST_H__
#define __STATE_LIST_H__

#include <cctype>
#include <string>

#include <symbol.h>
#include <helper.h>

#include "state.h"

inline State empty_state;
inline State langle_state;
inline State langle2_state;
inline State rangle_state;
inline State rangle2_state;
inline State lparen_state;
inline State rparen_state;
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
inline State single_arrow_state;
inline State plus_state;
inline State minus_state;
inline State asterisk_state;
inline State slash_state;
inline State percent_state;

struct Transition
{
    State *begin_state;
    State *end_state;
    std::function<bool(char)> condition;
};

inline Transition transitions[] = {
    {&empty_state, &hash_state, build_transition_function("#")},
    {&hash_state, &comment_state, build_transition_function("#")},
    {&comment_state, &comment_state, [](char c) -> bool
     {
         return c != '\n';
     }},
    {&empty_state, &langle_state, build_transition_function("<")},
    {&langle_state, &langle2_state, build_transition_function("<")},
    {&empty_state, &rangle_state, build_transition_function(">")},
    {&rangle_state, &rangle2_state, build_transition_function(">")},
    {&empty_state, &lparen_state, build_transition_function("(")},
    {&empty_state, &rparen_state, build_transition_function(")")},
    {&empty_state, &equal_state, build_transition_function("=")},
    {&empty_state, &space_state, build_transition_function(" \n\r\t")},
    {&space_state, &space_state, build_transition_function(" \n\r\t")},
    {&empty_state, &identifier_state, [](char c) -> bool
     {
         return isalpha(c) || c == '_';
     }},
    {&identifier_state, &identifier_state, [](char c) -> bool
     {
         return isalnum(c) || c == '_';
     }},
    {&empty_state, &number_state, [](char c) -> bool
     {
         return isdigit(c);
     }},
    {&number_state, &number_state, [](char c) -> bool
     {
         return isdigit(c);
     }},
    {&empty_state, &at_state, build_transition_function("@")},
    {&empty_state, &period_state, build_transition_function(".")},
    {&empty_state, &double_quote_state, build_transition_function("\"")},
    {&double_quote_state, &double_quote_state, [](char c) -> bool
     {
         return c != '\n' && c != '\"';
     }},
    {&double_quote_state, &quote_end_state, build_transition_function("\"")},
    {&empty_state, &comma_state, build_transition_function(",")},
    {&empty_state, &plus_state, build_transition_function("+")},
    {&empty_state, &minus_state, build_transition_function("-")},
    {&empty_state, &asterisk_state, build_transition_function("*")},
    {&empty_state, &slash_state, build_transition_function("/")},
    {&empty_state, &percent_state, build_transition_function("%")},
    {&minus_state, &single_arrow_state, build_transition_function(">")},
};

void populate_state_list();

inline std::string goose_alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_#<>().,@+-*/%= \n\t\r'\"";

inline std::vector<std::pair<State *, Terminal>> goose_accepting_state_map = {
    {&comment_state, COMMENT},
    {&langle2_state, ARG_OPEN_PAREN},
    {&rangle2_state, ARG_CLOSE_PAREN},
    {&lparen_state, OPEN_PAREN},
    {&rparen_state, CLOSE_PAREN},
    {&equal_state, ASSIGN},
    {&space_state, SPACES},
    {&identifier_state, IDENTIFIER},
    {&number_state, NUMBER_LITERAL},
    {&at_state, FUNC_AT},
    {&period_state, PERIOD},
    {&quote_end_state, STRING_LITERAL},
    {&comma_state, COMMA},
    {&plus_state, PLUS},
    {&minus_state, MINUS},
    {&asterisk_state, TIMES},
    {&slash_state, DIVIDE},
    {&percent_state, MOD},
    {&single_arrow_state, SINGLE_ARROW}};

inline std::vector<std::pair<std::string, Terminal>> goose_keywords = {
    {"flap", FLAP},
    {"start", START},
    {"end", END},
    {"word", WORD},
    {"number", NUMBER},
    {"return", RETURN}};

#endif
