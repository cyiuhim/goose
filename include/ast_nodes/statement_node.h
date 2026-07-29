#ifndef __STATEMENT_NODE_H__
#define __STATEMENT_NODE_H__

#include <ast_node.h>
#include <variant>
#include <vector>
#include <string>

class ExprNode;
class FuncCallNode;

class StatementNode : public ASTNode
{
};

// contains both declare and assign
class AssignStatementNode final : public StatementNode
{
public:
    AssignStatementNode(std::string identifier,
                        std::unique_ptr<ExprNode> expression,
                        bool is_declare) : identifier{std::move(identifier)},
                                           expression{std::move(expression)},
                                           is_declare{is_declare} {}
    void accept(Visitor *visitor);

private:
    std::string identifier;
    std::unique_ptr<ExprNode> expression;
    bool is_declare;
};

class FuncCallStatementNode final : public StatementNode
{
public:
    FuncCallStatementNode(std::unique_ptr<FuncCallNode> func_call)
        : func_call{std::move(func_call)} {}
    void accept(Visitor *visitor);

private:
    std::unique_ptr<FuncCallNode> func_call;
};

class ReturnStatementNode final : public StatementNode
{
public:
    ReturnStatementNode(std::unique_ptr<ExprNode> return_val)
        : return_val{std::move(return_val)} {}
    void accept(Visitor *visitor);

private:
    std::unique_ptr<ExprNode> return_val;
};

#endif
