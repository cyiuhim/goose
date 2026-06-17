#include <parser/parser.h>

namespace {
    template<typename T> 
    bool unordered_set_contains_all(const std::unordered_set<T>& set, const std::vector<T>& vec) {
        for (const auto& elem : vec) {
            if (!set.contains(elem)) return false;
        }
        return true;
    }
}

LLParser::LLParser(const Grammar& grammar) : grammar{grammar} {
    separate_symbols();
    compute_tables(grammar);
}

void LLParser::parse(const Tokens& tokens) {
    // TODO 
}

std::unique_ptr<ParserNode> LLParser::get_result() {
    return std::move(tree);
}

void LLParser::separate_symbols() {
    for (int i = 0; i < SymbolType::END_ENUM; i++) {
        SymbolType cur_symbol = static_cast<SymbolType> (i);
        if (!is_valid_symbol(cur_symbol)) continue;
        if (is_terminal(cur_symbol)) terminals.insert(cur_symbol);
        else non_terminals.insert(cur_symbol);
    }
}

void LLParser::compute_tables(const Grammar& grammar) {
    // PRE: separate_symbols are called 
    compute_epsilon_reachable(grammar);
    compute_first_table(grammar);
    compute_follow_table(grammar);
    compute_parser_table();
}

/* 
 * compute all the non-terminals that can derive epsilon 
 * PRE: separate_symbols is called 
 */
void LLParser::compute_epsilon_reachable(const Grammar& grammar) {
    
    // use a fixpoint method: start with an empty set of non-terminals,
    // then go through each production rule and see if epsilon can be reached 
    // continue this step until the epsilon_reachable set is fixed 

    std::unordered_set<SymbolType> prev_epsilon_reachable;
    do {
        prev_epsilon_reachable = epsilon_reachable;
        for (const auto& [non_terminal, expansions] : grammar) {
            for (const auto& expansion : expansions) {
                // if the expansion contains only the epsilon character then it means the non-terminal directly 
                if (expansion.size() == 1 && expansion[0] == SymbolType::EPSILON) epsilon_reachable.insert(non_terminal);

                // if expansion contains only non-terminals that can derive epsilon, then also add the non-terminal in 
                if (unordered_set_contains_all(prev_epsilon_reachable, expansion)) epsilon_reachable.insert(non_terminal); 
            }
        }
    } while (prev_epsilon_reachable != epsilon_reachable);
}

/* 
 * compute the first table, with keys X, non-terminals, and value FIRST(X)
 * FIRST(X) is the set of first terminals of all strings that can be derived from X, where X is non-terminal
 */
void LLParser::compute_first_table(const Grammar& grammar) {
    
    // use a fixpoint method: start with empty table 
    // then go through each non-terminal and see what terminals can be the first terminal out of any string derived from the non-terminal
    // add those terminals to the table entry (which is a set of terminals) 
    // continue until the table doesn't change 

    // if X -> epsilon then add epsilon 
    // if X can derive epsilon (from compute_epsilon_reachable) then add epsilon to FIRST(X)
    // if X -> aY where a is terminal and Y is (possibly a string of) non-terminals then add a to FIRST(X)
    // if X -> ABC then: add all terminals in FIRST(A) to FIRST(X), if A can derive epsilon then also add all entries in FIRST(B), if B can also derive epsilon then also add C, and so forth
    // (or if it's a mix of terminals and non-terminals in expansion, go until one non-terminal doesn't reach epsilon, or until reaching the first terminal)

    for (const auto non_terminal : non_terminals) {
        first_table[non_terminal] = {};
    }

    SymbolMappingTable prev_first_table;
    do {
        prev_first_table = first_table;
        for (const auto& non_terminal : non_terminals) {
            if (!grammar.contains(non_terminal)) continue;
            
            const auto& cur_expansions = grammar.at(non_terminal);
            auto& cur_first_table_entry = first_table.at(non_terminal);
            for (const auto& expansion : cur_expansions) {
                if (expansion[0] == EPSILON) cur_first_table_entry.insert(EPSILON);
                if (epsilon_reachable.contains(non_terminal)) cur_first_table_entry.insert(EPSILON);
                for (SymbolType child_symbol : expansion) {
                    if (terminals.contains(child_symbol)) {
                        cur_first_table_entry.insert(child_symbol);
                        break;
                    }
                    for (SymbolType child_derived_symbol : prev_first_table.at(child_symbol)) {
                        cur_first_table_entry.insert(child_derived_symbol);
                    }
                    if (!epsilon_reachable.contains(child_symbol)) break; 
                }
            }
        }
    } while (prev_first_table != first_table);
}

void LLParser::compute_follow_table(const Grammar& grammar) {
    // TODO
}

void LLParser::compute_parser_table() {
    // PRE: compute_first_table and compute_follow_table are called
    // TODO
}

