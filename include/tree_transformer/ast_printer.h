#ifndef __AST_PRINTER_H__
#define __AST_PRINTER_H__

#include <visitor.h>

class ASTPrinter : public Visitor
{
public:
    void visit(ProgramNode &program_node) override;
    void visit(FunctionDfnNode &function_node) override;
    void visit(AssignStatementNode &assign_statement_node) override;
    void visit(FuncCallStatementNode &func_call_statement_node) override;
    void visit(ReturnStatementNode &return_statement_node) override;
    void visit(BinExprNode &bin_expr_node) override;
    void visit(FuncCallNode &func_call_node) override;
    void visit(NumberLiteralNode &number_literal_node) override;
    void visit(StringLiteralNode &string_literal_node) override;
    void visit(IdentifierNode &identifier_node) override;

    std::string get_result();

private:
    std::string result;
};

#endif
