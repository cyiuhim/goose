#include <iostream>
#include <fstream>
#include <string>

#include <lexer/dfa.h>
#include <parser/parser.h>
#include <parser/grammar.h>

int main(int argc, char* argv[]) {
    // usage: goose basic.goose
    if (argc != 2) {
        std::cerr << "Usage: goose basic.goose" << std::endl;
        return 1;
    }
    std::string file_name {argv[1]};
    if (file_name.length() < 7 || file_name.substr(file_name.length() - 6) != ".goose") {
        std::cerr << "Invalid file name provided" << std::endl;
        return 1;
    }

    // stage 1: lexer
    DFA dfa;
    try {
        dfa.parse(file_name);
    }
    catch (std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 2;
    }
    Tokens tokens = dfa.get_result();
    
    // stage 2: parser 
    LLParser parser {goose_grammar};
    
    return 0;
}