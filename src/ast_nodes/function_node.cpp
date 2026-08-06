#include <ast_nodes/function_node.h>
#include <ast_nodes/statement_node.h>

FunctionDfnNode::FunctionDfnNode(std::string func_name,
                                 std::vector<FunctionParameter> parameters,
                                 std::vector<std::unique_ptr<StatementNode>> statements)
    : func_name{std::move(func_name)},
      parameters{std::move(parameters)},
      statements{std::move(statements)}
{
}

FunctionDfnNode::~FunctionDfnNode() = default;

void FunctionDfnNode::accept(Visitor *visitor)
{
    // TODO: implement
}