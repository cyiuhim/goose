#ifndef __PARSER_NODE_H__
#define __PARSER_NODE_H__

#include <memory>
#include "symbol.h"

class ParserNode {
    SymbolType symbol_type;
    std::vector<std::unique_ptr<ParserNode>> children;
};

#endif 