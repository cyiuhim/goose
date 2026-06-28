#include <helper.h>

std::string join (const std::vector<std::string>& vec, 
                  bool add_quotes, 
                  std::string delimiter) {
    if (vec.empty()) return "";
    std::string result;
    for (int i = 0; i < vec.size() - 1; i++) {
        if (add_quotes) result += '\"';
        result += vec[i];
        if (add_quotes) result += '\"';
        result += delimiter;
    }
    if (add_quotes) result += '\"';
    result += vec.back();
    if (add_quotes) result += '\"';
    return result;
}