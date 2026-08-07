#ifndef __HELPER_H__
#define __HELPER_H__

#include <string>
#include <vector>
#include <functional>

#include "type.h"
#include "symbol.h"

std::string join(const std::vector<std::string> &vec, bool add_quotes = true, std::string delimiter = " ");

std::function<bool(char)> build_transition_function(std::string transition_chars);

PrimitiveType symbol_to_primitive_type(SymbolType st);

#endif
