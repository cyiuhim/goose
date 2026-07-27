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
    std::unique_ptr<ProgramNode> transform(const ParserNode &parse_tree);

private:
    std::unique_ptr<FunctionNode> transform_function(const ParserNode &parse_tree);
    std::unique_ptr<StatementNode> transform_statement(const ParserNode &parse_tree);
    std::unique_ptr<AssignStatementNode> transform_declare_statement(const ParserNode &parse_tree);
    std::unique_ptr<AssignStatementNode> transform_assign_statement(const ParserNode &parse_tree, const std::string &identifier);
    std::unique_ptr<FuncCallStatementNode> transform_func_call_statement(const ParserNode &parse_tree, const std::string &identifier);
};

#endif
