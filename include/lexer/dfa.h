#ifndef __DFA_H__
#define __DFA_H__

#include <set>
#include <string>
#include "state.h"
#include "token.h"
#include "trie_node.h"

class DFA {
public: 
    DFA();
    void parse (std::string file_name);
    std::vector<std::pair<TokenType, std::string>> get_result();

private:
    void process_identifiers();
    void remove_spaces_and_comments();
    void populate_alphabet(std::string valid_chars);
    void set_start_state(State* state);
    void add_state(State* state);
    void add_accepting_states(std::vector<std::pair<State*, TokenType>>);
    void add_keywords(std::vector<std::pair<std::string, TokenType>>);

    std::vector<std::pair<TokenType, std::string>> tokens;
    std::set<char> alphabet;
    State* start_state;
    std::set<State*> states;
    std::map<State*, TokenType> accepting_states;
    TrieNode keyword_dict;
};

#endif 
