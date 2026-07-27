#ifndef __VISITOR_H__
#define __VISITOR_H__

class ProgramNode;

class Visitor
{
    virtual void visit(ProgramNode &program_node) = 0;
};

#endif
