#ifndef __PARSER_NODE_H__
#define __PARSER_NODE_H__

#include <memory>
#include "symbol.h"

struct ParserNode {
    SymbolType symbol_type;
    std::optional<std::string> lexeme;
    std::vector<std::unique_ptr<ParserNode>> children;
};

#endif 