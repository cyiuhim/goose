#include <tree_transformer.h>
#include <program_node.h>
#include <statement_node.h>
#include <function_node.h>

#include <format>
#include <utility>

/*
 * transforms a tree rooted at the START node into an AST rooted at a program node
 * PRE: parse tree is rooted at START
 * grammar rules:
 * START -> FUNC_DFNS
 * FUNC_DFNS -> FUNC_DFN FUNC_DFNS | EPSILON
 */

std::unique_ptr<ProgramNode> TreeTransformer::transform_program(const ParserNode &parse_tree)
{
    const ParserNode &start_node = parse_tree;
    ParserNode *func_dfns_node = start_node.children[0].get();
    std::vector<std::unique_ptr<FunctionDfnNode>> func_dfn_children;
    while (func_dfns_node->children.size() > 1)
    {
        func_dfn_children.push_back(transform_function(*func_dfns_node->children[0]));
        func_dfns_node = func_dfns_node->children[1].get();
    }
    return std::make_unique<ProgramNode>(std::move(func_dfn_children));
}

/*
 * transform a tree rooted at the FUNC_DFN node into an AST rooted at a function node
 * PRE: parse tree is rooted at FUNC_DFN
 * grammar rules:
 * FUNC_DFN -> START FUNC_AT IDENTIFIER FUNC_AT OPEN_PAREN PARAMS CLOSE_PAREN STATEMENTS END FUNC_AT IDENTIFIER FUNC_AT
 * STATEMENTS -> STATEMENT STATEMENTS | EPSILON
 * PARAMS -> PARAM PARAMS_TAIL | EPSILON
 */

std::unique_ptr<FunctionDfnNode> TreeTransformer::transform_function(const ParserNode &parse_tree)
{
    const ParserNode &func_dfn_node = parse_tree;

    // parse function name
    const ParserNode &start_identifier_node = *func_dfn_node.children[2];
    const ParserNode &end_identifier_node = *func_dfn_node.children[10];
    const std::string start_func_name = start_identifier_node.lexeme.value();
    const std::string end_func_name = end_identifier_node.lexeme.value();
    if (start_identifier_node.lexeme != end_identifier_node.lexeme)
    {
        throw std::runtime_error(std::format("Inconsistent function name, start: {}, end: {}", start_func_name, end_func_name));
    }

    // parse parameters
    const ParserNode &params_node = *func_dfn_node.children[5];
    const auto parameters = transform_params(params_node);

    // parse statements
    ParserNode *statement_node = func_dfn_node.children[7].get();
    std::vector<std::unique_ptr<StatementNode>> statement_children;
    while (statement_node->children.size() > 1)
    {
        statement_children.push_back(transform_statement(*statement_node->children[0]));
        statement_node = statement_node->children[1].get();
    }
    return std::make_unique<FunctionDfnNode>(std::move(start_func_name),
                                             std::move(parameters),
                                             std::move(statement_children));
}

/*
 * transform a tree rooted at the PARAMS node into a list of typed parameters
 * PRE: parse_tree rooted at the PARAMS node
 * grammar rules:
 * PARAMS -> PARAM PARAMS_TAIL | EPSILON
 * PARAMS_TAIL -> COMMA PARAM PARAMS_TAIL | EPSILON
 */

std::vector<FunctionParameter> TreeTransformer::transform_params(const ParserNode &parse_tree)
{
    // TODO: implement
    return {};
}

/*
 * transform a tree rooted at the PARAM node into a typed parameter
 * PRE: parse_tree is rooted at the PARAM node
 * grammar rules:
 * PARAM -> TYPE_NAME IDENTIFIER
 * TYPE_NAME -> NUMBER | WORD
 */

FunctionParameter transform_param(const ParserNode &parse_tree)
{
    // TODO: implement
    return {Number, ""};
}

/*
 * transform a tree rooted at the STATEMENT node into an AST rooted at a subclass of statement node
 * PRE: parse tree is rooted at STATEMENT
 * grammar rules:
 * STATEMENT -> RAW_STATEMENT PERIOD
 * RAW_STATEMENT -> DECLARE_STATEMENT | IDENTIFIER ID_START_STATEMENT | RETURN_STATEMENT
 * ID_START_STATEMENT -> ASSIGN_STATEMENT | FUNC_CALL_STATEMENT
 */

std::unique_ptr<StatementNode> TreeTransformer::transform_statement(const ParserNode &parse_tree)
{
    const ParserNode &statement_node = parse_tree;
    const ParserNode &raw_statement_node = *statement_node.children[0];
    if (raw_statement_node.children.size() == 1)
    {
        const ParserNode &typed_statement_node = *raw_statement_node.children[0];
        switch (std::get<NonTerminal>(typed_statement_node.symbol_type))
        {
        case DECLARE_STATEMENT:
            return transform_declare_statement(typed_statement_node);
            break;
        case RETURN_STATEMENT:
            return transform_return_statement(typed_statement_node);
            break;
        default:
            throw std::runtime_error("unreachable in transform statement");
        }
    }
    else if (raw_statement_node.children.size() == 2)
    {
        const ParserNode &identifier_node = *raw_statement_node.children[0];
        std::string identifier = identifier_node.lexeme.value();
        const ParserNode &id_start_statement_node = *identifier_node.children[1];
        const ParserNode &id_start_statement_child = *id_start_statement_node.children[0];
        switch (std::get<NonTerminal>(id_start_statement_child.symbol_type))
        {
        case ASSIGN_STATEMENT:
            return transform_assign_statement(id_start_statement_child, identifier);
            break;
        case FUNC_CALL_STATEMENT:
            return transform_func_call_statement(id_start_statement_child, identifier);
            break;
        default:
            throw std::runtime_error("unreachable in transform statement");
        }
    }
    return nullptr;
}

/*
 * transform a tree rooted at the DECLARE_STATEMENT node into an AST rooted at an assign statement node
 * PRE: parse tree is rooted at DECLARE_STATEMENT
 * grammar rules:
 * DECLARE_STATEMENT -> FLAP IDENTIFIER ASSIGN EXPR
 */

std::unique_ptr<AssignStatementNode> TreeTransformer::transform_declare_statement(const ParserNode &parse_tree)
{
    // TODO: implement
    return nullptr;
}

/*
 * transform a tree rooted at the ASSIGN_STATEMENT node into an AST rooted at an assign statement node
 * PRE: parse tree is rooted at ASSIGN_STATEMENT, with the function name passed in as identifier prop
 * grammar rules:
 * ASSIGN_STATEMENT -> ASSIGN EXPR
 */

std::unique_ptr<AssignStatementNode> TreeTransformer::transform_assign_statement(
    const ParserNode &parse_tree,
    const std::string identifier)
{
    // TODO: implement
    return nullptr;
}

/*
 * transform a tree rooted at the FUNC_CALL_STATEMENT node into an AST rooted at a func call statement node
 * PRE: parse tree rooted at FUNC_CALL_STATEMENT node, and identifier is the function name
 * grammar rules:
 * FUNC_CALL_STATEMENT -> ARG_OPEN_PAREN ARGS ARG_CLOSE_PAREN
 */

std::unique_ptr<FuncCallStatementNode> TreeTransformer::transform_func_call_statement(
    const ParserNode &parse_tree,
    const std::string identifier)
{
    // TODO: implement
    return nullptr;
}

/*
 * transform a tree rooted at the RETURN_STATEMENT node into an AST rooted at a return statement node
 * PRE: parse tree rooted at the RETURN_STATEMENT node
 * grammar rules:
 * RETURN_STATEMENT -> RETURN EXPr
 */

std::unique_ptr<ReturnStatementNode> TreeTransformer::transform_return_statement(const ParserNode &parse_tree)
{
    // TODO: implement
    return nullptr;
}

/*
 * transform a tree rooted at the ARGS node into a vector of ASTs rooted at expression nodes
 * PRE: parse tree rooted at the ARGS node
 * grammar rules:
 * ARGS -> ARG ARGS_TAIL | EPSILON
 * ARG -> EXPR
 */

std::vector<std::unique_ptr<ExprNode>> TreeTransformer::transform_args(const ParserNode &parse_tree)
{
    // TODO: implement
    return {};
}

/*
 * transform a tree rooted at the EXPR node into an AST rooted at a subclass of expression node
 * PRE: parse tree rooted at the EXPR node
 * grammar rules:
 * EXPR -> TERM EXPR_TAIL
 * EXPR_TAIL -> EPSILON | PLUS TERM EXPR_TAIL | MINUS TERM EXPR_TAIL
 */

std::unique_ptr<ExprNode> TreeTransformer::transform_expr(const ParserNode &parse_tree)
{
    // TODO: implement
    return nullptr;
}

/*
 * transform a tree rooted at the TERM node into an AST rooted at a subclass of expression node
 * PRE: parse tree rooted at the TERM node
 * grammar rules:
 * TERM -> FACTOR TERM_TAIL
 * TERM_TAIL -> EPSILON | TIMES FACTOR TERM_TAIL | DIVIDE FACTOR TERM_TAIL | MOD FACTOR TERM_TAIL
 */

std::unique_ptr<ExprNode> TreeTransformer::transform_term(const ParserNode &parse_tree)
{
    // TODO: implement
    return nullptr;
}

/*
 * transform a tree rooted at the FACTOR node into an AST rooted at a subclass of expression node
 * PRE: parse tree rooted at the FACTOR node
 * grammar rules:
 * FACTOR -> NUMBER_LITERAL | STRING_LITERAL | IDENTIFIER FACTOR_TAIL | OPEN_PAREN EXPR CLOSE_PAREN
 * FACTOR_TAIL -> EPSILON | ARG_OPEN_PAREN ARGS ARG_CLOSE_PAREN
 */

std::unique_ptr<ExprNode> TreeTransformer::transform_factor(const ParserNode &parse_tree)
{
    // TODO: implement
    return nullptr;
}
