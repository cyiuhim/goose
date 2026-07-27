#ifndef __AST_NODE_H__
#define __AST_NODE_H__

class Visitor;

class ASTNode
{
public:
    virtual void accept(Visitor *visitor) = 0;
};

#endif
