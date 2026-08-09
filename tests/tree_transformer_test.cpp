#include <gtest/gtest.h>

#include <filesystem>

#include <lexer/dfa.h>
#include <parser/grammar.h>
#include <parser/parser.h>
#include <ast_nodes/program_node.h>
#include <tree_transformer/tree_transformer.h>
#include <tree_transformer/ast_printer.h>

namespace
{
    namespace fs = std::filesystem;

    std::string fixture_path(const std::string &name)
    {
        return (fs::path(__FILE__).parent_path() / "fixtures" / "tree-transformer" / name).string();
    }

    bool has_childless_nonterminal(const ParserNode *node)
    {
        if (std::holds_alternative<NonTerminal>(node->symbol_type) && node->children.empty())
            return true;
        for (const auto &child : node->children)
        {
            if (has_childless_nonterminal(child.get()))
                return true;
        }
        return false;
    }

    void check_file_output_with_tree(std::string file_name,
                                     std::unique_ptr<ProgramNode> expected_ast)
    {
        DFA dfa;
        ASSERT_NO_THROW(dfa.parse(fixture_path(file_name)));
        const Tokens tokens = dfa.get_result();
        ASSERT_FALSE(tokens.empty());

        LLParser parser{goose_grammar};
        ASSERT_NO_THROW(parser.parse(tokens));
        auto tree = parser.get_result();
        ASSERT_NE(tree, nullptr);
        EXPECT_FALSE(has_childless_nonterminal(tree.get()));

        TreeTransformer transformer;
        auto actual_ast = transformer.transform_program(*tree);

        ASTPrinter ast_printer;
        actual_ast->accept(&ast_printer);
        std::string actual_ast_string = ast_printer.get_result();
        expected_ast->accept(&ast_printer);
        std::string expected_ast_string = ast_printer.get_result();
        EXPECT_EQ(actual_ast_string, expected_ast_string);
    }

    template <class Base, class... Ts>
    std::vector<std::unique_ptr<Base>> make_ptr_vec(Ts &&...ts)
    {
        std::vector<std::unique_ptr<Base>> v;
        v.reserve(sizeof...(Ts));
        (v.push_back(std::forward<Ts>(ts)), ...); // moves, no initializer_list
        return v;
    }
}

TEST(TreeTransformerIntegration, BasicTest)
{
    const std::string file_name = "basic.goose";
    auto expected_basic_ast = std::make_unique<ProgramNode>(
        make_ptr_vec<FunctionDfnNode>(
            std::make_unique<FunctionDfnNode>(
                "fly",
                std::vector<FunctionParameter>{},
                make_ptr_vec<StatementNode>(
                    std::make_unique<AssignStatementNode>(
                        "a",
                        std::make_unique<NumberLiteralNode>(5),
                        true),
                    std::make_unique<AssignStatementNode>(
                        "a",
                        std::make_unique<NumberLiteralNode>(6),
                        false),
                    std::make_unique<FuncCallStatementNode>(
                        std::make_unique<FuncCallNode>(
                            "honk",
                            make_ptr_vec<ExprNode>(
                                std::make_unique<StringLiteralNode>("\"a is: \""),
                                std::make_unique<IdentifierNode>("a"))))))));

    check_file_output_with_tree(file_name, std::move(expected_basic_ast));
}
