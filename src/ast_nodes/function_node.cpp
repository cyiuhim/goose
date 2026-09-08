#include <ast_nodes/function_node.h>
#include <ast_nodes/statement_node.h>
#include <visitor.h>

FunctionDfnNode::FunctionDfnNode(std::string func_name,
                                 std::vector<FunctionParameter> parameters,
                                 std::vector<std::unique_ptr<StatementNode>> statements,
                                 unsigned int line_no,
                                 unsigned int char_no,
                                 std::optional<Type> return_type)
    : func_name{std::move(func_name)},
      parameters{std::move(parameters)},
      statements{std::move(statements)},
      return_type{std::move(return_type)},
      ASTNode{line_no, char_no}
{
}

FunctionDfnNode::~FunctionDfnNode() = default;

void FunctionDfnNode::accept(Visitor *visitor)
{
    visitor->visit(*this);
}