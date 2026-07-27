#include <tree_transformer.h>
#include <program_node.h>
#include <statement_node.h>
#include <function_node.h>

/*
 * transforms a tree rooted at the START node into an AST rooted at a program node
 * PRE: parse tree is rooted at START
 * grammar rules:
 * START -> FUNC_DFNS
 * FUNC_DFNS -> FUNC_DFN FUNC_DFNS | EPSILON
 */

std::unique_ptr<ProgramNode> TreeTransformer::transform(const ParserNode &parse_tree)
{
    auto result = std::make_unique<ProgramNode>();
    const ParserNode &start_node = parse_tree;
    ParserNode &func_dfns_node = *start_node.children[0];
    while (func_dfns_node.children.size() > 1)
    {
        result->add_child(transform_function(*func_dfns_node.children[0]));
        func_dfns_node = *func_dfns_node.children[1];
    }
    return result;
}

/*
 * transform a tree rooted at the FUNC_DFN node into an AST rooted at a function node
 * PRE: parse tree is rooted at FUNC_DFN
 * grammar rules:
 * FUNC_DFN -> START FUNC_AT IDENTIFIER FUNC_AT STATEMENTS END FUNC_AT IDENTIFIER FUNC_AT
 * STATEMENTS -> STATEMENT STATEMENTS | EPSILON
 */

std::unique_ptr<FunctionNode> TreeTransformer::transform_function(const ParserNode &parse_tree)
{
    auto result = std::make_unique<FunctionNode>();
    const ParserNode &func_dfn_node = parse_tree;
    ParserNode &statement_node = *func_dfn_node.children[4];
    while (statement_node.children.size() > 1)
    {
        result->add_child(transform_statement(*statement_node.children[0]));
        statement_node = *statement_node.children[1];
    }
    return result;
}

/*
 * transform a tree rooted at the STATEMENT node into an AST rooted at a subclass of statement node
 * PRE: parse tree is rooted at STATEMENT
 * grammar rules:
 * STATEMENT -> RAW_STATEMENT PERIOD
 * RAW_STATEMENT -> DECLARE_STATEMENT | IDENTIFIER ID_START_STATEMENT
 * ID_START_STATEMENT -> ASSIGN_STATEMENT | FUNC_CALL_STATEMENT
 */

std::unique_ptr<StatementNode> TreeTransformer::transform_statement(const ParserNode &parse_tree)
{
    const ParserNode &statement_node = parse_tree;
    const ParserNode &raw_statement_node = *statement_node.children[0];
    if (raw_statement_node.children.size() == 1)
    {
        return transform_declare_statement(*raw_statement_node.children[0]);
    }
    else if (raw_statement_node.children.size() == 2)
    {
        const ParserNode &identifier_node = *raw_statement_node.children[0];
        std::string identifier = identifier_node.lexeme.value();
        const ParserNode &id_start_statement_node = *identifier_node.children[1];
        const ParserNode &id_start_statement_child = *id_start_statement_node.children[0];
        switch (std::get<Terminal>(id_start_statement_child.symbol_type))
        {
        case ASSIGN_STATEMENT:
            return transform_assign_statement(id_start_statement_child, identifier);
            break;
        case FUNC_CALL_STATEMENT:
            return transform_func_call_statement(id_start_statement_child, identifier);
            break;
        }
    }
}

/*
 * transform a tree rooted at the DECLARE_STATEMENT node into an AST rooted at a declare statement node
 * PRE: parse tree is rooted at DECLARE_STATEMENT
 * grammar rules:
 * DECLARE_STATEMENT -> FLAP IDENTIFIER ASSIGN EXPR
 */

std::unique_ptr<AssignStatementNode> TreeTransformer::transform_declare_statement(const ParserNode &parse_tree)
{
}