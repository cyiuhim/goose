#ifndef __TRIE_NODE_H__
#define __TRIE_NODE_H__

#include <vector>
#include <optional>
#include <string>

#include "token.h"

// only supports tries with only lowercase characters 
class TrieNode {
public: 
    // constructor
    TrieNode();
    // destructor 
    ~TrieNode();
    void add_word(std::string word, TokenType token);
    std::optional<TokenType> check_word(std::string word);

private:
    std::vector<TrieNode*> children;
    std::optional<TokenType> end_token; // end token if there is something here

};

#endif 
