#ifndef __HELPER_H__
#define __HELPER_H__

#include <string>
#include <vector>
#include <functional>

#include "type.h"
#include "symbol.h"
#include <ast_nodes/expr_node.h>

std::string join(const std::vector<std::string> &vec, bool add_quotes, std::string delimiter = " ");

std::function<bool(char)> build_transition_function(std::string transition_chars);

PrimitiveType symbol_to_primitive_type(SymbolType st);

bool symbol_equals_terminal(SymbolType st, Terminal terminal);

std::string type_to_string(Type *type);

void replace_one_instance(std::string &str, const std::string &from, const std::string &to);

std::string bin_op_to_string(BinOp op);

#endif
