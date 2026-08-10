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
            std::make_unique<FunctionDfnNode>(
                "honk_something",
                std::vector<FunctionParameter>{
                    {Word, "str"},
                    {Number, "n"}},
                make_ptr_vec<StatementNode>(
                    std::make_unique<FuncCallStatementNode>(
                        std::make_unique<FuncCallNode>(
                            "honk",
                            make_ptr_vec<ExprNode>(
                                std::make_unique<StringLiteralNode>("\"a goose once honked: \""),
                                std::make_unique<IdentifierNode>("str"),
                                std::make_unique<StringLiteralNode>("\" \""),
                                std::make_unique<IdentifierNode>("n"),
                                std::make_unique<StringLiteralNode>("\" times.\"")))))),
            std::make_unique<FunctionDfnNode>(
                "addition",
                std::vector<FunctionParameter>{
                    {Number, "a"},
                    {Number, "b"}},
                make_ptr_vec<StatementNode>(
                    std::make_unique<ReturnStatementNode>(
                        std::make_unique<BinExprNode>(
                            std::make_unique<IdentifierNode>("a"),
                            std::make_unique<IdentifierNode>("b"),
                            Add)))),
            std::make_unique<FunctionDfnNode>(
                "fly",
                std::vector<FunctionParameter>{},
                make_ptr_vec<StatementNode>(
                    std::make_unique<FuncCallStatementNode>(
                        std::make_unique<FuncCallNode>(
                            "honk_something",
                            make_ptr_vec<ExprNode>(
                                std::make_unique<StringLiteralNode>("\"long long man\""),
                                std::make_unique<NumberLiteralNode>(8)))),
                    std::make_unique<AssignStatementNode>(
                        "a",
                        std::make_unique<NumberLiteralNode>(3),
                        true),
                    std::make_unique<AssignStatementNode>(
                        "b",
                        std::make_unique<NumberLiteralNode>(5),
                        true),
                    std::make_unique<FuncCallStatementNode>(
                        std::make_unique<FuncCallNode>(
                            "honk",
                            make_ptr_vec<ExprNode>(
                                std::make_unique<IdentifierNode>("a"),
                                std::make_unique<StringLiteralNode>("\" plus \""),
                                std::make_unique<IdentifierNode>("b"),
                                std::make_unique<StringLiteralNode>("\" is \""),
                                std::make_unique<FuncCallNode>(
                                    "addition",
                                    make_ptr_vec<ExprNode>(
                                        std::make_unique<IdentifierNode>("a"),
                                        std::make_unique<IdentifierNode>("b"))))))))

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
            std::make_unique<FunctionDfnNode>(
                "func1",
                std::vector<FunctionParameter>(),
                make_ptr_vec<StatementNode>()),
            std::make_unique<FunctionDfnNode>(
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
            std::make_unique<FunctionDfnNode>(
                "increment",
                std::vector<FunctionParameter>{
                    {Number, "n"}},
                make_ptr_vec<StatementNode>(
                    std::make_unique<ReturnStatementNode>(
                        std::make_unique<BinExprNode>(
                            std::make_unique<IdentifierNode>("n"),
                            std::make_unique<NumberLiteralNode>(1),
                            Add)))),
            std::make_unique<FunctionDfnNode>(
                "fly",
                std::vector<FunctionParameter>{},
                make_ptr_vec<StatementNode>(
                    std::make_unique<AssignStatementNode>(
                        "a",
                        std::make_unique<NumberLiteralNode>(5),
                        true),
                    std::make_unique<FuncCallStatementNode>(
                        std::make_unique<FuncCallNode>(
                            "honk",
                            make_ptr_vec<ExprNode>(
                                std::make_unique<StringLiteralNode>("\"increment of increment of increment of a is: \""),
                                std::make_unique<FuncCallNode>(
                                    "increment",
                                    make_ptr_vec<ExprNode>(
                                        std::make_unique<FuncCallNode>(
                                            "increment",
                                            make_ptr_vec<ExprNode>(
                                                std::make_unique<FuncCallNode>(
                                                    "increment",
                                                    make_ptr_vec<ExprNode>(
                                                        std::make_unique<IdentifierNode>("a"))))))))))))));
    check_file_output_with_tree(file_name, std::move(expected_ast));
}

TEST(TreeTransformerIntegration, FunctionCallWithNoArg)
{
    const std::string file_name = "honks.goose";
    auto expected_ast = std::make_unique<ProgramNode>(
        make_ptr_vec<FunctionDfnNode>(
            std::make_unique<FunctionDfnNode>(
                "honk3",
                std::vector<FunctionParameter>{},
                make_ptr_vec<StatementNode>(
                    std::make_unique<FuncCallStatementNode>(
                        std::make_unique<FuncCallNode>(
                            "honk",
                            make_ptr_vec<ExprNode>(
                                std::make_unique<StringLiteralNode>("\"one goose honks\"")))

                            ),
                    std::make_unique<FuncCallStatementNode>(
                        std::make_unique<FuncCallNode>(
                            "honk",
                            make_ptr_vec<ExprNode>(
                                std::make_unique<StringLiteralNode>("\"two geese honk\"")))

                            ),
                    std::make_unique<FuncCallStatementNode>(
                        std::make_unique<FuncCallNode>(
                            "honk",
                            make_ptr_vec<ExprNode>(
                                std::make_unique<StringLiteralNode>("\"three geese honk\"")))

                            ))),
            std::make_unique<FunctionDfnNode>(
                "fly",
                std::vector<FunctionParameter>{},
                make_ptr_vec<StatementNode>(
                    std::make_unique<FuncCallStatementNode>(
                        std::make_unique<FuncCallNode>(
                            "honk3",
                            make_ptr_vec<ExprNode>()))))));
    check_file_output_with_tree(file_name, std::move(expected_ast));
}

TEST(TreeTransformerIntegration, ComplexExpression)
{
    const std::string file_name = "complex-expression.goose";
    auto expected_ast = std::make_unique<ProgramNode>(
        make_ptr_vec<FunctionDfnNode>(
            std::make_unique<FunctionDfnNode>(
                "fly",
                std::vector<FunctionParameter>{},
                make_ptr_vec<StatementNode>(
                    std::make_unique<AssignStatementNode>(
                        "a",
                        std::make_unique<BinExprNode>(
                            std::make_unique<BinExprNode>(
                                std::make_unique<BinExprNode>(
                                    std::make_unique<NumberLiteralNode>(1),
                                    std::make_unique<NumberLiteralNode>(234),
                                    Add),
                                std::make_unique<NumberLiteralNode>(5),
                                Mod),
                            std::make_unique<BinExprNode>(
                                std::make_unique<NumberLiteralNode>(67),
                                std::make_unique<BinExprNode>(
                                    std::make_unique<NumberLiteralNode>(89),
                                    std::make_unique<NumberLiteralNode>(34),
                                    Divide),
                                Subtract),
                            Multiply),
                        true),
                    std::make_unique<AssignStatementNode>(
                        "b",
                        std::make_unique<BinExprNode>(
                            std::make_unique<NumberLiteralNode>(3),
                            std::make_unique<NumberLiteralNode>(3),
                            Add),
                        true),
                    std::make_unique<FuncCallStatementNode>(
                        std::make_unique<FuncCallNode>(
                            "honk",
                            make_ptr_vec<ExprNode>(
                                std::make_unique<BinExprNode>(
                                    std::make_unique<IdentifierNode>("a"),
                                    std::make_unique<IdentifierNode>("b"),
                                    Add))))))));

    check_file_output_with_tree(file_name, std::move(expected_ast));
}