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
    for (int i = 0; i < static_cast<int> (vec.size()) - 1; i++)
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

bool symbol_equals_terminal(SymbolType st, Terminal terminal)
{
    return std::holds_alternative<Terminal>(st) && std::get<Terminal>(st) == terminal;
}

std::string type_to_string(Type type)
{
    switch (type)
    {
    case Word:
        return "Word";
        break;
    case Number:
        return "Number";
        break;
    }
}

void replace_one_instance(std::string &str,
                          const std::string &from,
                          const std::string &to)
{
    size_t pos = str.find(from);
    if (pos == std::string::npos)
        return;
    str.replace(pos, from.length(), to);
}

std::string bin_op_to_string(BinOp op)
{
    switch (op)
    {
    case Add:
        return "+";
        break;
    case Subtract:
        return "-";
        break;
    case Multiply:
        return "*";
        break;
    case Divide:
        return "/";
        break;
    case Mod:
        return "%";
        break;
    }
}
