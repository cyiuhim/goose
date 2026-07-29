#ifndef __EXPR_NODE_H__
#define __EXPR_NODE_H__

#include <ast_node.h>
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
    void accept(Visitor *visitor);

private:
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
    void accept(Visitor *visitor);

private:
    std::string func_name;
    std::vector<std::unique_ptr<ExprNode>> args;
};

#endif
