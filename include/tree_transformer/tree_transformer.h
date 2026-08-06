#ifndef __TREE_TRANSFORMER_H__
#define __TREE_TRANSFORMER_H__

#include <parser/parser_node.h>
#include <program_node.h>
#include <function_node.h>
#include <statement_node.h>

class ProgramNode;

class TreeTransformer
{
public:
    std::unique_ptr<ProgramNode> transform_program(const ParserNode &parse_tree);

private:
    /* main functions for parsing different nodes */
    std::unique_ptr<FunctionDfnNode> transform_function(const ParserNode &parse_tree);
    std::vector<FunctionParameter> transform_params(const ParserNode &parse_tree);
    FunctionParameter transform_param(const ParserNode &parse_tree);

    std::unique_ptr<StatementNode> transform_statement(const ParserNode &parse_tree);
    std::unique_ptr<AssignStatementNode> transform_declare_statement(const ParserNode &parse_tree);
    std::unique_ptr<AssignStatementNode> transform_assign_statement(const ParserNode &parse_tree, const std::string identifier);
    std::unique_ptr<FuncCallStatementNode> transform_func_call_statement(const ParserNode &parse_tree, const std::string identifier);
    std::unique_ptr<ReturnStatementNode> transform_return_statement(const ParserNode &parse_tree);

    std::vector<std::unique_ptr<ExprNode>> transform_args(const ParserNode &parse_tree);
    std::unique_ptr<ExprNode> transform_expr(const ParserNode &parse_tree);
    std::unique_ptr<ExprNode> transform_term(const ParserNode &parse_tree);
    std::unique_ptr<ExprNode> transform_factor(const ParserNode &parse_tree);
};

#endif
