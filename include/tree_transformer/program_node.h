#ifndef __PROGRAM_NODE_H__
#define __PROGRAM_NODE_H__

#include <ast_node.h>
#include <vector>

class FunctionNode;

class ProgramNode : public ASTNode
{
public:
    void accept(Visitor *visitor);
    void add_child(std::unique_ptr<FunctionNode> function_node);

private:
    std::vector<std::unique_ptr<FunctionNode>> function_children;
};

#endif
