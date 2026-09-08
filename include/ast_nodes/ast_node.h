#ifndef __AST_NODE_H__
#define __AST_NODE_H__

class Visitor;

class ASTNode
{
public:
    ASTNode(unsigned int line_no, unsigned int char_no)
        : line_no{line_no}, char_no{char_no} {}
    virtual ~ASTNode() = default;
    virtual void accept(Visitor *visitor) = 0;

protected:
    unsigned int line_no;
    unsigned int char_no;
};

#endif
