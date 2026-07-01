#include <iostream>
#include <string>
#include <parser/parser.h>
#include <parser/grammar.h>

std::string sexpr(const ParserNode* n) {
    if (std::holds_alternative<Terminal>(n->symbol_type)) {
        Terminal t = std::get<Terminal>(n->symbol_type);
        std::string s = to_string(t);
        if (n->lexeme) s += ":" + *n->lexeme;
        return s;
    }
    std::string s = "(" + to_string(std::get<NonTerminal>(n->symbol_type));
    for (const auto& c : n->children) s += " " + sexpr(c.get());
    return s + ")";
}

void run(const char* label, Tokens tokens) {
    LLParser parser{goose_grammar};
    parser.parse(tokens);
    auto tree = parser.get_result();
    std::cout << label << ":\n" << sexpr(tree.get()) << "\n\n";
}

int main() {
    run("empty", {
        {START,"start"},{FUNC_AT,"@"},{IDENTIFIER,"foo"},{FUNC_AT,"@"},
        {END,"end"},{FUNC_AT,"@"},{IDENTIFIER,"foo"},{FUNC_AT,"@"},
    });
    run("declaration", {
        {START,"start"},{FUNC_AT,"@"},{IDENTIFIER,"foo"},{FUNC_AT,"@"},
        {FLAP,"flap"},{IDENTIFIER,"x"},{ASSIGN,"="},{NUMBER,"5"},{PERIOD,"."},
        {END,"end"},{FUNC_AT,"@"},{IDENTIFIER,"foo"},{FUNC_AT,"@"},
    });
    return 0;
}
