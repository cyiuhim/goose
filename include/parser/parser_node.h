#ifndef __PARSER_NODE_H__
#define __PARSER_NODE_H__

#include <memory>
#include <symbol.h>

struct ParserNode {
    ParserNode(SymbolType st) : symbol_type {st}, line_no{0}, char_no{0} {}
    SymbolType symbol_type;
    std::optional<std::string> lexeme;
    std::vector<std::unique_ptr<ParserNode>> children;
    unsigned int line_no;
    unsigned int char_no;
};

#endif 