#include "symbol.h"
#include "type.h"
#include <helper.h>

#include <format>
#include <variant>

std::string join(const std::vector<std::string> &vec,
                 bool add_quotes,
                 std::string delimiter)
{
    if (vec.empty())
        return "";
    std::string result;
    for (int i = 0; i < vec.size() - 1; i++)
    {
        if (add_quotes)
            result += '\"';
        result += vec[i];
        if (add_quotes)
            result += '\"';
        result += delimiter;
    }
    if (add_quotes)
        result += '\"';
    result += vec.back();
    if (add_quotes)
        result += '\"';
    return result;
}

std::function<bool(char)> build_transition_function(std::string transition_chars)
{
    return [transition_chars = std::move(transition_chars)](char c) -> bool
    {
        return std::ranges::find(transition_chars, c) != transition_chars.end();
    };
}

PrimitiveType symbol_to_primitive_type(SymbolType st)
{
    if (!std::holds_alternative<Terminal>(st))
    {
        throw std::runtime_error(std::format("Called symbol_to_primitive_type with non terminal symbol: {}", to_string(std::get<NonTerminal>(st))));
    }
    switch (std::get<Terminal>(st))
    {
    case WORD:
        return Word;
        break;
    case NUMBER:
        return Number;
        break;
    default:
        throw std::runtime_error(std::format("Called symbol_to_primitive_type with invalid terminal symbol: {}", to_string(std::get<Terminal>(st))));
    }
}