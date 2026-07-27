#ifndef __STATEMENT_NODE_H__
#define __STATEMENT_NODE_H__

#include <ast_node.h>
#include <variant>
#include <vector>
#include <string>

class ExprNode;

class StatementNode : public ASTNode
{
};

// contains both declare and assign
class AssignStatementNode : public StatementNode
{
public:
    AssignStatementNode(bool is_declare) : is_declare{is_declare} {}
    void accept(Visitor *visitor);
    void set_expression_node(std::unique_ptr<ExprNode> expression)
    {
        this->expression = std::move(expression);
    }

private:
    std::string identifier;
    std::unique_ptr<ExprNode> expression;
    bool is_declare;
};

class FuncCallStatementNode : public StatementNode
{
public:
    void accept(Visitor *visitor);

private:
    std::string func_name;
    std::vector<std::variant<ExprNode *, FuncCallStatementNode *>> params;
};

#endif
