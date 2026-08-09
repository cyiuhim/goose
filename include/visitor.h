#ifndef __VISITOR_H__
#define __VISITOR_H__

#include <ast_nodes/program_node.h>
#include <ast_nodes/function_node.h>
#include <ast_nodes/statement_node.h>
#include <ast_nodes/expr_node.h>

class Visitor
{
public:
    virtual void visit(ProgramNode &program_node) = 0;
    virtual void visit(FunctionDfnNode &function_node) = 0;
    virtual void visit(AssignStatementNode &assign_statement_node) = 0;
    virtual void visit(FuncCallStatementNode &func_call_statement_node) = 0;
    virtual void visit(ReturnStatementNode &return_statement_node) = 0;
    virtual void visit(BinExprNode &bin_expr_node) = 0;
    virtual void visit(FuncCallNode &func_call_node) = 0;
    virtual void visit(NumberLiteralNode &number_literal_node) = 0;
    virtual void visit(StringLiteralNode &string_literal_node) = 0;
    virtual void visit(IdentifierNode &identifier_node) = 0;
};

#endif
