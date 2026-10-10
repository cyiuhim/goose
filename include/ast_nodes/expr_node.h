#ifndef __EXPR_NODE_H__
#define __EXPR_NODE_H__

#include <ast_nodes/ast_node.h>
#include <memory>
#include <string>
#include <vector>

class ExprNode : public ASTNode
{
protected:
    ExprNode(unsigned int line_no, unsigned int char_no)
        : ASTNode{line_no, char_no} {}
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
                BinOp op,
                unsigned int line_no,
                unsigned int char_no)
        : ExprNode{line_no, char_no},
          left_child{std::move(left_child)},
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
                 std::vector<std::unique_ptr<ExprNode>> args,
                 unsigned int line_no,
                 unsigned int char_no)
        : ExprNode{line_no, char_no},
          func_name{std::move(func_name)},
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
    explicit NumberLiteralNode(long long val,
                               unsigned int line_no,
                               unsigned int char_no)
        : ExprNode{line_no, char_no}, val{val} {}

    void accept(Visitor *visitor) override;

    long long val;
};

// TODO: when changing the standard, remove this comment
// NOTE: the val contains the double quotes before and after the string literal
class StringLiteralNode final : public ExprNode
{
public:
    StringLiteralNode(std::string val,
                      unsigned int line_no,
                      unsigned int char_no)
        : ExprNode{line_no, char_no}, val{std::move(val)} {}

    void accept(Visitor *visitor) override;

    std::string val;
};

class IdentifierNode final : public ExprNode
{
public:
    IdentifierNode(std::string identifier_name,
                   unsigned int line_no,
                   unsigned int char_no)
        : ExprNode{line_no, char_no},
          identifier_name{std::move(identifier_name)} {}

    void accept(Visitor *visitor) override;

    std::string identifier_name;
};

#endif
