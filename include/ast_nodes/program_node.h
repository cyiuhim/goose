#ifndef __PROGRAM_NODE_H__
#define __PROGRAM_NODE_H__

#include <ast_node.h>
#include <vector>

class FunctionDfnNode;

class ProgramNode : public ASTNode
{
public:
    ProgramNode(std::vector<std::unique_ptr<FunctionDfnNode>> functions)
        : functions{std::move(functions)} {}
    void accept(Visitor *visitor);

private:
    std::vector<std::unique_ptr<FunctionDfnNode>> functions;
};

#endif
