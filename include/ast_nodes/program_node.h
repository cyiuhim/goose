#ifndef __PROGRAM_NODE_H__
#define __PROGRAM_NODE_H__

#include <ast_node.h>
#include <function_node.h>

#include <vector>

class ProgramNode : public ASTNode
{
public:
    ProgramNode(std::vector<std::unique_ptr<FunctionDfnNode>> functions);
    void accept(Visitor *visitor);

    std::vector<std::unique_ptr<FunctionDfnNode>> functions;
};

#endif
