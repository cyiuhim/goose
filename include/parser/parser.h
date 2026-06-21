#ifndef __PARSER_H__
#define __PARSER_H__

#include "symbol.h"
#include "parser_node.h"
#include "grammar.h"

#include <lexer/dfa.h>

#include <map>
#include <unordered_set>

typedef std::map<SymbolType, std::unordered_set<SymbolType>> SymbolMappingTable;
typedef std::map<std::pair<SymbolType, SymbolType>, std::pair<SymbolType, Expansion>> ParserTable;

class LLParser {
public: 
    LLParser(const Grammar& grammar);

    void parse(const Tokens& tokens);

    std::unique_ptr<ParserNode> get_result();

private: 
    void compute_tables();

    void separate_symbols();

    void compute_epsilon_reachable();
    void compute_first_table();
    void compute_follow_table();

    void compute_parser_table();

    // member variables 
    const Grammar& grammar;

   
    std::unordered_set<SymbolType> non_terminals;
    std::unordered_set<SymbolType> terminals;

     /* all the non-terminals that can derive epsilon */
    std::unordered_set<SymbolType> epsilon_reachable;

    SymbolMappingTable first_table;
    SymbolMappingTable follow_table;

    ParserTable parser_table;

    std::unique_ptr<ParserNode> tree;
};

#endif 
