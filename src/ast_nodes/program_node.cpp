#include <ast_nodes/program_node.h>
#include <ast_nodes/function_node.h>
#include <visitor.h>

ProgramNode::ProgramNode(std::vector<std::unique_ptr<FunctionDfnNode>> functions)
    : ASTNode{0, 0}, functions{std::move(functions)}
{
}

void ProgramNode::accept(Visitor *visitor)
{
    visitor->visit(*this);
}