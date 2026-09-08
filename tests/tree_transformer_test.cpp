#include <gtest/gtest.h>

#include <filesystem>

#include <lexer/dfa.h>
#include <parser/grammar.h>
#include <parser/parser.h>
#include <ast_nodes/program_node.h>
#include <tree_transformer/tree_transformer.h>
#include <tree_transformer/ast_printer.h>

constexpr unsigned int TEST_LINE = 0;
constexpr unsigned int TEST_CHAR = 0;

template <class Node, class... Args>
std::unique_ptr<Node> make_test_node(Args &&...args)
{
    return std::make_unique<Node>(
        std::forward<Args>(args)...,
        TEST_LINE,
        TEST_CHAR);
}

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
            make_test_node<FunctionDfnNode>(
                "fly",
                std::vector<FunctionParameter>{},
                make_ptr_vec<StatementNode>(
                    make_test_node<AssignStatementNode>(
                        "a",
                        make_test_node<NumberLiteralNode>(5),
                        true),
                    make_test_node<AssignStatementNode>(
                        "a",
                        make_test_node<NumberLiteralNode>(6),
                        false),
                    make_test_node<FuncCallStatementNode>(
                        make_test_node<FuncCallNode>(
                            "honk",
                            make_ptr_vec<ExprNode>(
                                make_test_node<StringLiteralNode>("\"a is: \""),
                                make_test_node<IdentifierNode>("a"))))))));

    check_file_output_with_tree(file_name, std::move(expected_basic_ast));
}

TEST(TreeTransformerIntegration, ProgramWithNoFunctions)
{
    const std::string file_name = "empty.goose";
    auto expected_ast = std::make_unique<ProgramNode>(
        make_ptr_vec<FunctionDfnNode>());
    check_file_output_with_tree(file_name, std::move(expected_ast));
}

TEST(TreeTransformerIntegration, ProgramWithMultipleFunctions)
{
    const std::string file_name = "function-calls.goose";
    auto expected_ast = std::make_unique<ProgramNode>(
        make_ptr_vec<FunctionDfnNode>(
            make_test_node<FunctionDfnNode>(
                "honk_something",
                std::vector<FunctionParameter>{
                    {Word, "str"},
                    {Number, "n"}},
                make_ptr_vec<StatementNode>(
                    make_test_node<FuncCallStatementNode>(
                        make_test_node<FuncCallNode>(
                            "honk",
                            make_ptr_vec<ExprNode>(
                                make_test_node<StringLiteralNode>("\"a goose once honked: \""),
                                make_test_node<IdentifierNode>("str"),
                                make_test_node<StringLiteralNode>("\" \""),
                                make_test_node<IdentifierNode>("n"),
                                make_test_node<StringLiteralNode>("\" times.\"")))))),
            make_test_node<FunctionDfnNode>(
                "addition",
                std::vector<FunctionParameter>{
                    {Number, "a"},
                    {Number, "b"}},
                make_ptr_vec<StatementNode>(
                    make_test_node<ReturnStatementNode>(
                        make_test_node<BinExprNode>(
                            make_test_node<IdentifierNode>("a"),
                            make_test_node<IdentifierNode>("b"),
                            Add)))),
            make_test_node<FunctionDfnNode>(
                "fly",
                std::vector<FunctionParameter>{},
                make_ptr_vec<StatementNode>(
                    make_test_node<FuncCallStatementNode>(
                        make_test_node<FuncCallNode>(
                            "honk_something",
                            make_ptr_vec<ExprNode>(
                                make_test_node<StringLiteralNode>("\"long long man\""),
                                make_test_node<NumberLiteralNode>(8)))),
                    make_test_node<AssignStatementNode>(
                        "a",
                        make_test_node<NumberLiteralNode>(3),
                        true),
                    make_test_node<AssignStatementNode>(
                        "b",
                        make_test_node<NumberLiteralNode>(5),
                        true),
                    make_test_node<FuncCallStatementNode>(
                        make_test_node<FuncCallNode>(
                            "honk",
                            make_ptr_vec<ExprNode>(
                                make_test_node<IdentifierNode>("a"),
                                make_test_node<StringLiteralNode>("\" plus \""),
                                make_test_node<IdentifierNode>("b"),
                                make_test_node<StringLiteralNode>("\" is \""),
                                make_test_node<FuncCallNode>(
                                    "addition",
                                    make_ptr_vec<ExprNode>(
                                        make_test_node<IdentifierNode>("a"),
                                        make_test_node<IdentifierNode>("b"))))))))

                ));

    check_file_output_with_tree(file_name, std::move(expected_ast));
}

TEST(TreeTransformerIntegration, InvalidFunctionWithInconsistentStartEndNames)
{
    const std::string file_name = "invalid-function.goose";
    EXPECT_THROW({ check_file_output_with_tree(file_name, nullptr); }, std::runtime_error);
}

TEST(TreeTransformerIntegration, FunctionsWithNoStatement)
{
    const std::string file_name = "noops.goose";
    auto expected_ast = std::make_unique<ProgramNode>(
        make_ptr_vec<FunctionDfnNode>(
            make_test_node<FunctionDfnNode>(
                "func1",
                std::vector<FunctionParameter>(),
                make_ptr_vec<StatementNode>()),
            make_test_node<FunctionDfnNode>(
                "func2",
                std::vector<FunctionParameter>{
                    {Word, "str"},
                    {Number, "n"}},
                make_ptr_vec<StatementNode>())));
    check_file_output_with_tree(file_name, std::move(expected_ast));
}

TEST(TreeTransformerIntegration, RepeatedFunctionCalls)
{
    const std::string file_name = "increments.goose";
    auto expected_ast = std::make_unique<ProgramNode>(
        make_ptr_vec<FunctionDfnNode>(
            make_test_node<FunctionDfnNode>(
                "increment",
                std::vector<FunctionParameter>{
                    {Number, "n"}},
                make_ptr_vec<StatementNode>(
                    make_test_node<ReturnStatementNode>(
                        make_test_node<BinExprNode>(
                            make_test_node<IdentifierNode>("n"),
                            make_test_node<NumberLiteralNode>(1),
                            Add)))),
            make_test_node<FunctionDfnNode>(
                "fly",
                std::vector<FunctionParameter>{},
                make_ptr_vec<StatementNode>(
                    make_test_node<AssignStatementNode>(
                        "a",
                        make_test_node<NumberLiteralNode>(5),
                        true),
                    make_test_node<FuncCallStatementNode>(
                        make_test_node<FuncCallNode>(
                            "honk",
                            make_ptr_vec<ExprNode>(
                                make_test_node<StringLiteralNode>("\"increment of increment of increment of a is: \""),
                                make_test_node<FuncCallNode>(
                                    "increment",
                                    make_ptr_vec<ExprNode>(
                                        make_test_node<FuncCallNode>(
                                            "increment",
                                            make_ptr_vec<ExprNode>(
                                                make_test_node<FuncCallNode>(
                                                    "increment",
                                                    make_ptr_vec<ExprNode>(
                                                        make_test_node<IdentifierNode>("a"))))))))))))));
    check_file_output_with_tree(file_name, std::move(expected_ast));
}

TEST(TreeTransformerIntegration, FunctionCallWithNoArg)
{
    const std::string file_name = "honks.goose";
    auto expected_ast = std::make_unique<ProgramNode>(
        make_ptr_vec<FunctionDfnNode>(
            make_test_node<FunctionDfnNode>(
                "honk3",
                std::vector<FunctionParameter>{},
                make_ptr_vec<StatementNode>(
                    make_test_node<FuncCallStatementNode>(
                        make_test_node<FuncCallNode>(
                            "honk",
                            make_ptr_vec<ExprNode>(
                                make_test_node<StringLiteralNode>("\"one goose honks\"")))

                            ),
                    make_test_node<FuncCallStatementNode>(
                        make_test_node<FuncCallNode>(
                            "honk",
                            make_ptr_vec<ExprNode>(
                                make_test_node<StringLiteralNode>("\"two geese honk\"")))

                            ),
                    make_test_node<FuncCallStatementNode>(
                        make_test_node<FuncCallNode>(
                            "honk",
                            make_ptr_vec<ExprNode>(
                                make_test_node<StringLiteralNode>("\"three geese honk\"")))

                            ))),
            make_test_node<FunctionDfnNode>(
                "fly",
                std::vector<FunctionParameter>{},
                make_ptr_vec<StatementNode>(
                    make_test_node<FuncCallStatementNode>(
                        make_test_node<FuncCallNode>(
                            "honk3",
                            make_ptr_vec<ExprNode>()))))));
    check_file_output_with_tree(file_name, std::move(expected_ast));
}

TEST(TreeTransformerIntegration, ComplexExpression)
{
    const std::string file_name = "complex-expression.goose";
    auto expected_ast = std::make_unique<ProgramNode>(
        make_ptr_vec<FunctionDfnNode>(
            make_test_node<FunctionDfnNode>(
                "fly",
                std::vector<FunctionParameter>{},
                make_ptr_vec<StatementNode>(
                    make_test_node<AssignStatementNode>(
                        "a",
                        make_test_node<BinExprNode>(
                            make_test_node<BinExprNode>(
                                make_test_node<BinExprNode>(
                                    make_test_node<NumberLiteralNode>(1),
                                    make_test_node<NumberLiteralNode>(234),
                                    Add),
                                make_test_node<NumberLiteralNode>(5),
                                Mod),
                            make_test_node<BinExprNode>(
                                make_test_node<NumberLiteralNode>(67),
                                make_test_node<BinExprNode>(
                                    make_test_node<NumberLiteralNode>(89),
                                    make_test_node<NumberLiteralNode>(34),
                                    Divide),
                                Subtract),
                            Multiply),
                        true),
                    make_test_node<AssignStatementNode>(
                        "b",
                        make_test_node<BinExprNode>(
                            make_test_node<NumberLiteralNode>(3),
                            make_test_node<NumberLiteralNode>(3),
                            Add),
                        true),
                    make_test_node<FuncCallStatementNode>(
                        make_test_node<FuncCallNode>(
                            "honk",
                            make_ptr_vec<ExprNode>(
                                make_test_node<BinExprNode>(
                                    make_test_node<IdentifierNode>("a"),
                                    make_test_node<IdentifierNode>("b"),
                                    Add))))))));

    check_file_output_with_tree(file_name, std::move(expected_ast));
}