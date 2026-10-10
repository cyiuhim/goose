#ifndef __TYPE_CHECK_VISITOR_H__
#define __TYPE_CHECK_VISITOR_H__

#include <visitor.h>

class TypeCheckVisitor : public Visitor
{
public:
    void visit(ProgramNode &program_node) override;
    void visit(FunctionDfnNode &func_dfn_node) override;
    void visit(AssignStatementNode &assign_statement_node) override;
    void visit(FuncCallStatementNode &func_call_statement_node) override;
    void visit(ReturnStatementNode &return_statement_node) override;
    void visit(BinExprNode &bin_expr_node) override;
    void visit(FuncCallNode &func_call_node) override;
    void visit(NumberLiteralNode &number_literal_node) override;
    void visit(StringLiteralNode &string_literal_node) override;
    void visit(IdentifierNode &identifier_node) override;

    Type get_result()
    {
        return result;
    }

private:
    Type result;
    TypeTable type_table_result;
};

#endif
