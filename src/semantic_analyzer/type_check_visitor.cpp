#include <type_check_visitor.h>

/* 
 * the visit function for type check visitor at the (root) program node 
 * goes through all function node children, and gather type information 
 * then it recurses into each function node and gets the function table 
 */
void TypeCheckVisitor::visit(ProgramNode &program_node) {

}

/* 
 * 
 */
void TypeCheckVisitor::visit(FunctionDfnNode &func_dfn_node) {

}

/* 
 *
 */
void TypeCheckVisitor::visit(AssignStatementNode &assign_statement_node) {

}

/* 
 * 
 */
void TypeCheckVisitor::visit(FuncCallStatementNode &func_call_statement_node) {

}

/* 
 * 
 */
void TypeCheckVisitor::visit(ReturnStatementNode &return_statement_node) {

}

/* 
 *
 */
void TypeCheckVisitor::visit(BinExprNode &bin_expr_node) {

}

/* 
 *
 */
void TypeCheckVisitor::visit(FuncCallNode &func_call_node) {

}

/* 
 * 
 */
void TypeCheckVisitor::visit(NumberLiteralNode &number_literal_node) {

}

/* 
 *
 */
void TypeCheckVisitor::visit(StringLiteralNode &string_literal_node) {

}

/* 
 *
 */
void TypeCheckVisitor::visit(IdentifierNode &identifier_node) {

}

