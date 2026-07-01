/* 
 * honk!
 * references:
 * https://www.geeksforgeeks.org/compiler-design/construction-of-ll1-parsing-table/ 
 * https://en.wikipedia.org/wiki/LL_parser#Constructing_an_LL(1)_parsing_table
 */

#include <iostream>
#include <format>
#include <stack>
#include <ranges>

#include <parser/parser.h>
#include <helper.h>

namespace {
    template<typename T> 
    bool unordered_set_contains_all(const std::unordered_set<T>& set, const std::vector<T>& vec) {
        for (const auto& elem : vec) {
            if (!set.contains(elem)) return false;
        }
        return true;
    }

    void insert_into_parser_table(ParserTable& parser_table, 
                                  NonTerminal non_terminal, 
                                  Terminal terminal, 
                                  const std::pair<NonTerminal, Expansion>& grammar_rule) {
        if (parser_table.contains({non_terminal, terminal})) {
            std::cerr << "Parser table already contains entry at: {" << non_terminal << ", " << terminal << "}" << std::endl;
            throw std::runtime_error("Parser error");
        }
        parser_table[{non_terminal, terminal}] = grammar_rule;
    }

    bool symbol_equals_terminal(SymbolType st, Terminal terminal) {
        return std::holds_alternative<Terminal> (st) && std::get<Terminal> (st) == terminal;
    }
}

LLParser::LLParser(const Grammar& grammar) : grammar{grammar} {
    populate_symbols();
    compute_tables();
}

std::unique_ptr<ParserNode> LLParser::get_result() {
    return std::move(tree);
}

void LLParser::populate_symbols() {
    for (int i = 0; i < END_NON_TERMINALS; i++) non_terminals.push_back(static_cast<NonTerminal> (i));
    for (int i = 0; i < END_TERMINALS; i++) terminals.push_back(static_cast<Terminal> (i));
}

void LLParser::compute_tables() {
    // PRE: populate_symbols are called 
    compute_epsilon_reachable();
    compute_first_table();
    compute_follow_table();
    validate_grammar();
    compute_parser_table();
}

/* 
 * compute all the non-terminals that can derive epsilon 
 * PRE: populate_symbols is called 
 */
void LLParser::compute_epsilon_reachable() {
    
    // use a fixpoint method: start with an empty set of non-terminals,
    // then go through each production rule and see if epsilon can be reached 
    // continue this step until the epsilon_reachable set is fixed 

    std::unordered_set<SymbolType> prev_epsilon_reachable;
    do {
        prev_epsilon_reachable = epsilon_reachable;
        for (const auto& [non_terminal, expansions] : grammar) {
            for (const auto& expansion : expansions) {
                // if the expansion contains only the epsilon character then it means the non-terminal directly 
                if (expansion.size() == 1 && symbol_equals_terminal(expansion[0], EPSILON)) epsilon_reachable.insert(non_terminal);

                // if expansion contains only non-terminals that can derive epsilon, then also add the non-terminal in 
                if (unordered_set_contains_all(prev_epsilon_reachable, expansion)) epsilon_reachable.insert(non_terminal); 
            }
        }
    } while (prev_epsilon_reachable != epsilon_reachable);
}

/* 
 * compute the first table, with keys X, non-terminals, and value FIRST(X)
 * FIRST(X) is the set of first terminals of all strings that can be derived from X, where X is non-terminal
 * 
 * PRE: compute_epsilon_reachable is called 
 */
void LLParser::compute_first_table() {
    
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
        for (const auto non_terminal : non_terminals) {
            if (!grammar.contains(non_terminal)) continue;
            
            const auto& cur_expansions = grammar.at(non_terminal);
            auto& cur_first_table_entry = first_table.at(non_terminal);
            for (const auto& expansion : cur_expansions) {
                if (symbol_equals_terminal(expansion[0], EPSILON)) cur_first_table_entry.insert(EPSILON);
                if (epsilon_reachable.contains(non_terminal)) cur_first_table_entry.insert(EPSILON);
                for (SymbolType child_symbol : expansion) {
                    if (std::holds_alternative<Terminal> (child_symbol)) {
                        cur_first_table_entry.insert(std::get<Terminal> (child_symbol));
                        break;
                    }
                    for (Terminal child_derived_symbol : prev_first_table.at(std::get<NonTerminal> (child_symbol))) {
                        cur_first_table_entry.insert(child_derived_symbol);
                    }
                    if (!epsilon_reachable.contains(child_symbol)) break; 
                }
            }
        }
    } while (prev_first_table != first_table);
}

/* 
 * compute the follow table, with keys X, non-terminals, and values FOLLOW(X)
 * FOLLOW(X) is the set of all terminals that can immediately follow X 
 * 
 * PRE: compute_first_table is called 
 */
void LLParser::compute_follow_table() {
    
    // use a fixpoint method: start with empty table 
    // continue to expand the follow table until it cannot be expanded anymore 

    // rule 1: if X is the start symbol then FOLLOW(X) = {$}, just the end symbol 
    // rule 2: if X -> a B c where c is a terminal then we add c to the FOLLOW(B)
    // rule 3: if X -> a B Y1 Y2 ... Yn where all of Y1 to Yn can lead to epsilon then we include FOLLOW(X) to FOLLOW(B)

    for (const auto non_terminal : non_terminals) {
        follow_table[non_terminal] = {};
    }

    // rule 1
    follow_table[START_SYMBOL] = {END_SYMBOL};
    
    SymbolMappingTable prev_follow_table;
    do {
        prev_follow_table = follow_table;
        for (const auto& [non_terminal, expansions] : grammar) {
            for (const auto& expansion : expansions) {
                for (int cur_idx = 0; cur_idx < expansion.size(); cur_idx++) {
                    const SymbolType cur_st = expansion[cur_idx];

                    // we only consider the case where cur_st is a nonterminal 
                    if (!std::holds_alternative<NonTerminal> (cur_st)) continue;

                    const NonTerminal cur_nt = std::get<NonTerminal> (cur_st);

                    // checking if rule 3 can be applied while going through the symbols after cur_st
                    bool can_follow_empty = true;
                    for (int follow_idx = cur_idx + 1; follow_idx < expansion.size(); follow_idx++) {
                        const SymbolType follow_st = expansion[follow_idx];

                        // rule 2
                        if (std::holds_alternative<Terminal>(follow_st)) {
                            follow_table[cur_nt].insert(std::get<Terminal> (follow_st));
                            can_follow_empty = false;
                            break;
                        }

                        // adding everything from FIRST(follow_st) to FOLLOW(cur_st), since everything between cur_st and follow_st can be derived to epsilon
                        for (const auto first_st : first_table[std::get<NonTerminal> (follow_st)]) {
                            if (symbol_equals_terminal(first_st, EPSILON)) continue;
                            follow_table[cur_nt].insert(first_st);
                        }

                        // if the follow_st cannot reach epsilon then we don't have to care what comes after 
                        // terminal after cur_st can only be derived by anything on or before follow_st
                        if (!epsilon_reachable.contains(follow_st)) {
                            can_follow_empty = false;
                            break;
                        }
                    }
                    if (can_follow_empty) {
                        // that means non_terminal -> (something) cur_st (list of nonterminals that can all reach epsilon)
                        for (const auto follow_st : prev_follow_table[non_terminal]) {
                            follow_table[cur_nt].insert(follow_st);
                        }
                    }
                }
            }
        }
    } while (prev_follow_table != follow_table);
}

/* 
 * validate whether the grammar is LL(1) compatible 
 * PRE: compute_first_table and compute_follow_table are executed 
 */

void LLParser::validate_grammar() {

    // get the predict sets for each production rule 
    // PREDICT(A -> a) would be FIRST(a) if a cannot derive epsilon, else FIRST(a) - {epsilon} U FOLLOW(A) if a can derive epsilon 
    // the grammar is LL1 comptaible if and only if for all nonterminals A, no two production rules of A would have overlapping PREDICT sets 

    for (const auto& [non_terminal, expansions] : grammar) {
        if (expansions.size() < 2) continue;
        std::vector<std::unordered_set<Terminal>> predict_sets;
        for (const auto& expansion : expansions) {
            std::unordered_set<Terminal> cur_predict_set;
            bool can_derive_epsilon = true;
            for (const SymbolType derived_st : expansion) {
                if (std::holds_alternative<Terminal> (derived_st)) {
                    cur_predict_set.insert(std::get<Terminal> (derived_st));
                    can_derive_epsilon = false;
                    break;
                }
                for (const Terminal first_of_derived : first_table[std::get<NonTerminal> (derived_st)]) {
                    if (first_of_derived != EPSILON) cur_predict_set.insert(first_of_derived);
                }
                if (!epsilon_reachable.contains(derived_st)) {
                    can_derive_epsilon = false;
                    break;
                }
            }
            if (can_derive_epsilon) {
                for (const Terminal follow_st : follow_table[non_terminal]) {
                    cur_predict_set.insert(follow_st);
                }
            }
            predict_sets.push_back(std::move(cur_predict_set));
        }
        for (int i = 0; i < predict_sets.size(); i++) {
            for (int j = i + 1; j < predict_sets.size(); j++) {
                const auto& predict_set_1 = predict_sets[i];
                const auto& predict_set_2 = predict_sets[j];
                for (const auto predict_st : predict_set_1) {
                    if (predict_set_2.contains(predict_st)) {
                        throw std::runtime_error(std::format("Error in validating grammar, non-terminal involved: {}, with rules {} and {}", static_cast<int> (non_terminal), i, j));
                    }
                }
            }
        }
    }
}

/* 
 * compute the parser table, with keys {X, y} where X nonterminal and y terminal, and values {A, b} where A -> b is a grammar rule 
 * PRE: compute_first_table and compute_follow_table are executed 
 * 
 */

void LLParser::compute_parser_table() {

    // for each parsing rule A -> a where a is a string of symbols, possibly terminals and nonterminals 
    // rule 1: for each terminal b in FIRST(a) we put the rule A -> a in position {A,b}
    // rule 2: if a can derive epsilon then for each t in FOLLOW(A) we put A -> a in position {A,t}

    for (const auto& [non_terminal, expansions] : grammar) {
        for (const auto& expansion : expansions) {
            
            bool can_derive_epsilon = true;

            for (const SymbolType derived_symbol : expansion) {

                // apply rule 1 
                if (std::holds_alternative<Terminal> (derived_symbol)) {
                    Terminal derived_terminal = std::get<Terminal> (derived_symbol);
                    insert_into_parser_table(parser_table, non_terminal, derived_terminal, {non_terminal, expansion});
                    if (derived_terminal != EPSILON) {
                        can_derive_epsilon = false;
                    }
                    break;
                }
                
                for (const Terminal first_symbol_of_derived : first_table[std::get<NonTerminal> (derived_symbol)]) {
                    insert_into_parser_table(parser_table, non_terminal, first_symbol_of_derived, {non_terminal, expansion});
                }

                if (!epsilon_reachable.contains(derived_symbol)) {
                    can_derive_epsilon = false;
                    break;
                }
            }

            // apply rule 2
            if (can_derive_epsilon && follow_table.contains(non_terminal)) {
                for (const Terminal follow_st : follow_table[non_terminal]) {
                    insert_into_parser_table(parser_table, non_terminal, follow_st, {non_terminal, expansion});
                }
            }
        }
    }
}

/*
 * takes the output from the lexer in the form of tokens, and construct the parse tree
 * PRE: compute_parser_table is executed (which it is, by the constructor of LLParser)
 */

void LLParser::parse(const Tokens& tokens) {
    
    // keep track of two stacks, the node stack and the symbol stack
    // node stack is used for keeping track of which node to expand on 
    // symbol stack is used for keeping track of which symbols should be consumed / derived next, depending on whether it's terminal or nonterminal

    std::stack<ParserNode*> node_stack;
    std::stack<SymbolType> symbol_stack;

    // start with root on the node stack 
    // start with {START_SYMBOL, $} on the symbol_stack

    tree.reset(new ParserNode {START_SYMBOL});
    node_stack.push(tree.get());
    symbol_stack.push(END_SYMBOL);
    symbol_stack.push(START_SYMBOL);

    int token_idx = 0;

    // go through the stream of tokens 
    // if seeing the current token is the same as whatever is on the top of the symbol_stack, then pop it
    // otherwise, find out entry in the parser table on position {nonterminal on top of symbol stack, current token}
    // find the expansion, pop the symbol stack and node stack and create same number of children as symbols in expansion
    // push children nodes onto the node stack, the symbols onto the symbol stack, BOTH IN REVERSE ORDER (basically the first token of the expansion should be on top)
    // continue until going through the whole stream of tokens
    // or throw an error if seeing missing entry in parser table or exhausted stream of tokens / symbol stack before the other one

    while (token_idx < tokens.size()) {
        auto cur_token = tokens[token_idx];
        SymbolType top_symbol = symbol_stack.top();
        ParserNode *parent_node = node_stack.top();
        symbol_stack.pop();
        node_stack.pop();
        if (std::holds_alternative<Terminal> (top_symbol)) {
            Terminal top_terminal = std::get<Terminal> (top_symbol);
            if (top_terminal == EPSILON) {
                continue;
            }
            if (top_terminal != cur_token.token_type) {
                throw std::runtime_error(std::format("Expected symbol {}, instead got {}", to_string(top_terminal), to_string(cur_token.token_type)));
            }
            parent_node->lexeme = cur_token.lexeme;
            token_idx++;
        }
        else {
            NonTerminal top_non_terminal = std::get<NonTerminal> (top_symbol);
            if (!parser_table.contains({top_non_terminal, cur_token.token_type})) {
                // find the possible terminals that can go with it 
                std::vector<std::string> expected_terminals;
                for (const Terminal terminal : terminals) {
                    if (parser_table.contains({top_non_terminal, terminal})) {
                        expected_terminals.push_back(to_string(terminal));
                    }
                }
                throw std::runtime_error(std::format("Unexpected symbol: {}, expected: {}", to_string(cur_token.token_type), join(expected_terminals)));
            }
            const auto& [_, expansion] = parser_table.at({top_non_terminal, cur_token.token_type});
            for (const SymbolType expansion_symbol : expansion | std::views::reverse) {
                // put it onto the symbol stack 
                symbol_stack.push(expansion_symbol);
                // create node and push it onto the node stack 
                auto new_node = std::make_unique<ParserNode> (expansion_symbol);
                node_stack.push(new_node.get());
                parent_node->children.push_back(std::move(new_node));
            }
            
            std::ranges::reverse(parent_node->children);
        }
    }

    while (true) {
        SymbolType top_symbol = symbol_stack.top();
        if (std::holds_alternative<NonTerminal> (top_symbol) && epsilon_reachable.contains(std::get<NonTerminal> (top_symbol))) {
            symbol_stack.pop();
            ParserNode *top_node = node_stack.top();
            top_node->children.push_back(std::make_unique<ParserNode> (EPSILON));
            node_stack.pop();
        }
        else break;
    }

    // validation: the symbol stack should only have the end symbol, and the node stack should be empty
    if (symbol_stack.size() > 1) {
        throw std::runtime_error(std::format("Error: finished parsing but symbols remaining"));
    }

}
