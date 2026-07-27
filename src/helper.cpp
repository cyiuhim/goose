#include <helper.h>
#include <ranges>

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