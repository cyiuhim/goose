#include <parser/parser.h>

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
    // PRE: compute_tables are called 
    compute_first_table(grammar);
    compute_follow_table(grammar);
    compute_parser_table();
}

void LLParser::compute_first_table(const Grammar& grammar) {
    // TODO 
}

void LLParser::compute_follow_table(const Grammar& grammar) {
    // TODO
}

void LLParser::compute_parser_table() {
    // PRE: compute_first_table and compute_follow_table are called
    // TODO
}

