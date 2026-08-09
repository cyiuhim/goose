#include <expr_node.h>
#include <visitor.h>

void BinExprNode::accept(Visitor *visitor)
{
    visitor->visit(*this);
}

void FuncCallNode::accept(Visitor *visitor)
{
    visitor->visit(*this);
}

void NumberLiteralNode::accept(Visitor *visitor)
{
    visitor->visit(*this);
}

void StringLiteralNode::accept(Visitor *visitor)
{
    visitor->visit(*this);
}

void IdentifierNode::accept(Visitor *visitor)
{
    visitor->visit(*this);
}