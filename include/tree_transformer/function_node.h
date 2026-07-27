#ifndef __FUNCTION_NODE_H__
#define __FUNCTION_NODE_H__

#include <ast_node.h>
#include <vector>

class StatementNode;

class FunctionNode : public ASTNode
{
public:
    void accept(Visitor *visitor);
    void add_child(std::unique_ptr<StatementNode> child)
    {
        statement_children.push_back(std::move(child));
    }

private:
    std::string func_name;
    std::vector<std::unique_ptr<StatementNode>> statement_children;
};

#endif
