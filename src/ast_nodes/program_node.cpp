#include <ast_nodes/program_node.h>
#include <ast_nodes/function_node.h>

ProgramNode::ProgramNode(std::vector<std::unique_ptr<FunctionDfnNode>> functions)
    : functions{std::move(functions)}
{
}

void ProgramNode::accept(Visitor *visitor)
{
    // TODO: implement
}