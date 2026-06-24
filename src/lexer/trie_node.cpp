#include "trie_node.h"

TrieNode::TrieNode() : children(26, nullptr), end_token {std::nullopt} {
    
}

TrieNode::~TrieNode() {
    for (auto child : children) {
        if (child != nullptr) {
            delete child;
        }
    }
}

void TrieNode::add_word(std::string word, Terminal token) {
    if (word.empty()) {
        end_token = token;
        return;
    }
    char first_char = word[0];
    if (!children[first_char - 'a']) {
        children[first_char - 'a'] = new TrieNode;
    }
    children[first_char - 'a']->add_word(word.substr(1), token);
}

std::optional<Terminal> TrieNode::check_word(std::string word) {
    if (word.empty()) {
        return end_token;
    }
    if (!children.at(word[0] - 'a')) {
        return std::nullopt;
    }
    return children.at(word[0] - 'a')->check_word(word.substr(1));
}
