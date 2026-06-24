#ifndef __PARSER_H__
#define __PARSER_H__

#include <symbol.h>
#include "parser_node.h"
#include "grammar.h"

#include <map>
#include <unordered_set>

typedef std::map<NonTerminal, std::unordered_set<Terminal>> SymbolMappingTable;
typedef std::map<std::pair<NonTerminal, Terminal>, std::pair<NonTerminal, Expansion>> ParserTable;

struct TransformedToken {
    SymbolType symbol;
    std::string lexeme;
};

typedef std::vector<TransformedToken> ParserInput;

class LLParser {
public: 
    LLParser(const Grammar& grammar);

    void parse(const Tokens& tokens);

    std::unique_ptr<ParserNode> get_result();

private: 
    void compute_tables();

    void populate_symbols();

    void compute_epsilon_reachable();
    void compute_first_table();
    void compute_follow_table();

    void validate_grammar();

    void compute_parser_table();

    // member variables 
    const Grammar& grammar;
   
    std::vector<NonTerminal> non_terminals;
    std::vector<Terminal> terminals;

     /* all the non-terminals that can derive epsilon */
    std::unordered_set<SymbolType> epsilon_reachable;

    SymbolMappingTable first_table;
    SymbolMappingTable follow_table;

    ParserTable parser_table;

    std::unique_ptr<ParserNode> tree;
};

#endif 
