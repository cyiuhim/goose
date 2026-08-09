#include <tree_transformer/ast_printer.h>
#include <helper.h>

#include <format>
#include "ast_printer.h"

// (program func1 func2 ...)
void ASTPrinter::visit(ProgramNode &program_node)
{
    std::vector<std::string> func_strings;
    for (auto &function_node : program_node.functions)
    {
        function_node->accept(this);
        func_strings.push_back(this->result);
    }
    std::string program_string = std::format(
        "(program {})",
        join(func_strings, false));
    this->result = program_string;
}

// (func (param1, param2, ...) (statement1 statement2 ...))
void ASTPrinter::visit(FunctionDfnNode &function_node)
{
    std::vector<std::string> param_strings;
    for (auto &[type, param_name] : function_node.parameters)
    {
        std::string param_string = std::format("{} {}", type_to_string(type), param_name);
        param_strings.push_back(param_string);
    }
    std::vector<std::string> statement_strings;
    for (auto &statement_node : function_node.statements)
    {
        statement_node->accept(this);
        statement_strings.push_back(this->result);
    }
    std::string function_string = std::format(
        "(func ({}) {})",
        join(param_strings, false, ", "),
        join(statement_strings, false));
    this->result = function_string;
}

// (assign identifier expr) or (declare identifier expr)
void ASTPrinter::visit(AssignStatementNode &assign_statement_node)
{
    assign_statement_node.expression->accept(this);
    std::string expr_string = this->result;
    std::string assign_string = std::format(
        "({} {} {})",
        assign_statement_node.is_declare ? "declare" : "assign",
        assign_statement_node.identifier,
        expr_string);
    this->result = assign_string;
}

// (func-call-statement identifier arg1 arg2 ...)
void ASTPrinter::visit(FuncCallStatementNode &func_call_statement_node)
{
    func_call_statement_node.func_call->accept(this);
    std::string func_call_string = this->result;
    replace_one_instance(func_call_string, "func-call", "func-call-statement");
    this->result = func_call_string;
}

// (return expr)
void ASTPrinter::visit(ReturnStatementNode &return_statement_node)
{
    return_statement_node.return_val->accept(this);
    std::string ret_val_string = this->result;
    std::string return_string = std::format("(return {})", ret_val_string);
    this->result = return_string;
}

// (op expr1 expr2)
void ASTPrinter::visit(BinExprNode &bin_expr_node)
{
    bin_expr_node.left_child->accept(this);
    std::string left_child_string = this->result;
    bin_expr_node.right_child->accept(this);
    std::string right_child_string = this->result;
    std::string op_string = bin_op_to_string(bin_expr_node.op);
    std::string bin_expr_string = std::format("({} {} {})", op_string, left_child_string, right_child_string);
}

// (func-call identifier arg1 arg2 ...)
void ASTPrinter::visit(FuncCallNode &func_call_node)
{
    std::vector<std::string> arg_strings;
    for (auto &arg_node : func_call_node.args)
    {
        arg_node->accept(this);
        arg_strings.push_back(this->result);
    }
    std::string func_call_string = std::format(
        "(func-call {} {})",
        func_call_node.func_name,
        join(arg_strings));
    this->result = func_call_string;
}

// num
void ASTPrinter::visit(NumberLiteralNode &number_literal_node)
{
    this->result = std::to_string(number_literal_node.val);
}

// "val"
void ASTPrinter::visit(StringLiteralNode &string_literal_node)
{
    this->result = std::format("\"{}\"", string_literal_node.val);
}

// identifier
void ASTPrinter::visit(IdentifierNode &identifier_node)
{
    this->result = identifier_node.identifier_name;
}
