#ifndef __EXPR_NODE_H__
#define __EXPR_NODE_H__

#include <ast_nodes/ast_node.h>
#include <memory>
#include <string>
#include <vector>

class ExprNode : public ASTNode
{
};

enum BinOp
{
    Add,
    Subtract,
    Multiply,
    Divide,
    Mod
};

class BinExprNode final : public ExprNode
{
public:
    BinExprNode(std::unique_ptr<ExprNode> left_child,
                std::unique_ptr<ExprNode> right_child,
                BinOp op)
        : left_child{std::move(left_child)},
          right_child{std::move(right_child)},
          op{op}
    {
    }
    void accept(Visitor *visitor) override;

    std::unique_ptr<ExprNode> left_child;
    std::unique_ptr<ExprNode> right_child;
    BinOp op;
};

class FuncCallNode final : public ExprNode
{
public:
    FuncCallNode(std::string func_name,
                 std::vector<std::unique_ptr<ExprNode>> args)
        : func_name{std::move(func_name)},
          args{std::move(args)}
    {
    }
    void accept(Visitor *visitor) override;

    std::string func_name;
    std::vector<std::unique_ptr<ExprNode>> args;
};

class NumberLiteralNode final : public ExprNode
{
public:
    explicit NumberLiteralNode(long long val) : val{val} {}

    void accept(Visitor *visitor) override;

    long long val;
};

// NOTE: the val contains the double quotes before and after the string literal
class StringLiteralNode final : public ExprNode
{
public:
    StringLiteralNode(std::string val) : val{std::move(val)} {}

    void accept(Visitor *visitor) override;

    std::string val;
};

class IdentifierNode final : public ExprNode
{
public:
    IdentifierNode(std::string identifier_name) : identifier_name{std::move(identifier_name)} {}

    void accept(Visitor *visitor) override;

    std::string identifier_name;
};

#endif
