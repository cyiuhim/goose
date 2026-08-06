#include <iostream>
#include <fstream>
#include <string>

#include <lexer/dfa.h>
#include <parser/parser.h>
#include <parser/grammar.h>
#include <tree_transformer/tree_transformer.h>

int main(int argc, char *argv[])
{
    // usage: goose basic.goose
    if (argc != 2)
    {
        std::cerr << "Usage: goose basic.goose" << std::endl;
        return 1;
    }
    std::string file_name{argv[1]};
    if (file_name.length() < 7 || file_name.substr(file_name.length() - 6) != ".goose")
    {
        std::cerr << "Invalid file name provided" << std::endl;
        return 1;
    }

    // stage 1: lexer
    DFA dfa;
    try
    {
        dfa.parse(file_name);
    }
    catch (std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        return 2;
    }
    Tokens tokens = dfa.get_result();

    // stage 2: parser
    LLParser parser{goose_grammar};
    try
    {
        parser.parse(tokens);
    }
    catch (std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        return 3;
    }

    std::unique_ptr<ParserNode> parse_tree = parser.get_result();

    // stage 3: tree transformer
    TreeTransformer transformer;
    std::unique_ptr<ProgramNode> ast;
    try
    {
        ast = transformer.transform_program(*parse_tree);
    }
    catch (std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        return 4;
    }

    return 0;
}