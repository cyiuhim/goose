#include <expr_node.h>
#include <statement_node.h>
#include <visitor.h>

#include <utility>

AssignStatementNode::AssignStatementNode(
    std::string identifier,
    std::unique_ptr<ExprNode> expression,
    bool is_declare,
    unsigned int line_no,
    unsigned int char_no)
    : StatementNode{line_no, char_no},
      identifier{std::move(identifier)},
      expression{std::move(expression)},
      is_declare{is_declare}
{
}

AssignStatementNode::~AssignStatementNode() = default;

void AssignStatementNode::accept(Visitor *visitor)
{
    visitor->visit(*this);
}

FuncCallStatementNode::FuncCallStatementNode(
    std::unique_ptr<FuncCallNode> func_call,
    unsigned int line_no,
    unsigned int char_no)
    : StatementNode{line_no, char_no}, 
      func_call{std::move(func_call)}
      
{
}

FuncCallStatementNode::~FuncCallStatementNode() = default;

void FuncCallStatementNode::accept(Visitor *visitor)
{
    visitor->visit(*this);
}

ReturnStatementNode::ReturnStatementNode(
    std::unique_ptr<ExprNode> return_val,
    unsigned int line_no,
    unsigned int char_no)
    : StatementNode{line_no, char_no},
      return_val{std::move(return_val)}
{
}

ReturnStatementNode::~ReturnStatementNode() = default;

void ReturnStatementNode::accept(Visitor *visitor)
{
    visitor->visit(*this);
}
