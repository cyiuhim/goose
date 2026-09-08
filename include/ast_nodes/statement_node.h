#ifndef __STATEMENT_NODE_H__
#define __STATEMENT_NODE_H__

#include <ast_node.h>
#include <expr_node.h>

#include <memory>
#include <string>
#include <utility>

class StatementNode : public ASTNode
{
protected:
    StatementNode(unsigned int line_no,
                  unsigned int char_no)
        : ASTNode{line_no, char_no} {}
};

// contains both declare and assign
class AssignStatementNode final : public StatementNode
{
public:
    AssignStatementNode(std::string identifier,
                        std::unique_ptr<ExprNode> expression,
                        bool is_declare,
                        unsigned int line_no,
                        unsigned int char_no);
    ~AssignStatementNode() override;
    void accept(Visitor *visitor) override;

    std::string identifier;
    std::unique_ptr<ExprNode> expression;
    bool is_declare;
};

class FuncCallStatementNode final : public StatementNode
{
public:
    FuncCallStatementNode(std::unique_ptr<FuncCallNode> func_call,
                          unsigned int line_no,
                          unsigned int char_no);
    ~FuncCallStatementNode() override;
    void accept(Visitor *visitor) override;

    std::unique_ptr<FuncCallNode> func_call;
};

class ReturnStatementNode final : public StatementNode
{
public:
    ReturnStatementNode(std::unique_ptr<ExprNode> return_val,
                        unsigned int line_no,
                        unsigned int char_no);
    ~ReturnStatementNode() override;
    void accept(Visitor *visitor) override;

    std::unique_ptr<ExprNode> return_val;
};

#endif
