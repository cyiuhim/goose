#include "dfa.h"
#include "state_list.h"
#include <stdexcept>
#include <format>
#include <fstream>

struct CharInfo {
    int line_no;
    int char_no;
    char c;
};

struct BacktrackInfo {
    std::string last_token;
    size_t last_idx;
    State* last_state;
};

// implements maximal munch algorithm, converts text into tokens

void DFA::parse(std::string file_name) {
    std::ifstream in {file_name};
    if (in.fail()) {
        throw std::runtime_error("File doesn't exist");
    }
    std::vector<CharInfo> text;
    std::string line;
    int line_cnt = 1;
    while (std::getline(in, line)) {
        int char_no = 1;
        for (char c : line) {
            text.push_back({line_cnt, char_no, c});
            char_no++;
        }
        text.push_back({line_cnt, char_no, '\n'});
        line_cnt++;
    }
    tokens.clear();
    size_t idx = 0;

    while (idx < text.size()) {
        State* cur_state = start_state;
        const size_t start_idx = idx;
        BacktrackInfo backtrack = {"", (size_t) (-1), nullptr};
        std::string cur_token;
        while (idx < text.size()) {
            if (!cur_state) break;
            if (cur_state != &comment_state && cur_state != &double_quote_state && alphabet.find(text[idx].c) == alphabet.end()) {
                throw std::runtime_error(std::format("invalid character: '{}' at line {} char {}", text[idx].c, text[idx].line_no, text[idx].char_no));
            }
            cur_state = cur_state->get_next_state(text[idx].c);
            cur_token += text[idx].c;
            if (accepting_states.find(cur_state) != accepting_states.end()) {
                backtrack = {cur_token, idx + 1, cur_state};
            }
            idx++;
        }
        // edit cur token to go back to last backtracked state
        if (!backtrack.last_state) {
            throw std::runtime_error(std::format("error parsing at line {} char {} with current token {}", text[idx].line_no, text[idx].char_no, cur_token));
        }
        Terminal token_type = accepting_states.at(backtrack.last_state);
        tokens.emplace_back(token_type, cur_token.substr(0, backtrack.last_idx - start_idx));
        idx = backtrack.last_idx;
    }
    process_identifiers();
    remove_spaces_and_comments();
}

void DFA::process_identifiers() {
    for (auto& token : tokens) {
        if (token.token_type == IDENTIFIER) {
            auto new_token_type = keyword_dict.check_word(token.lexeme);
            if (new_token_type.has_value()) {
                token.token_type = *new_token_type;
            }
        }
    }
}

void DFA::remove_spaces_and_comments() {
    Tokens new_tokens;
    for (const auto& token : tokens) {
        if (token.token_type == SPACES) continue;
        if (token.token_type == COMMENT) continue;
        new_tokens.push_back(token);
    }
    tokens = std::move(new_tokens);
}

Tokens DFA::get_result() {
    return tokens;
}

void DFA::populate_alphabet(std::string valid_chars) {
    for (char c : valid_chars) {
        alphabet.insert(c);
    }
}

void DFA::set_start_state(State* state) {
    start_state = state;
}

void DFA::add_state(State* state) {
    states.insert(state);
}

void DFA::add_accepting_states(std::vector<std::pair<State*, Terminal>> state_map) {
    for (auto& [state, token] : state_map) {
        accepting_states[state] = token;
    }
}

void DFA::add_keywords(std::vector<std::pair<std::string, Terminal>> keywords) {
    for (const auto& keyword_pair : keywords) {
        keyword_dict.add_word(keyword_pair.first, keyword_pair.second);
    }
}

DFA::DFA() : start_state{&empty_state} {
    populate_state_list();
    populate_alphabet(goose_alphabet);
    add_accepting_states(goose_accepting_state_map);
    add_keywords(goose_keywords);
}