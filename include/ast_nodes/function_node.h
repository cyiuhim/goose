#ifndef __FUNCTION_NODE_H__
#define __FUNCTION_NODE_H__

#include <ast_node.h>
#include <type.h>

#include <vector>

class StatementNode;

struct FunctionParameter
{
    PrimitiveType type;
    std::string identifier;
};

class FunctionDfnNode final : public ASTNode
{
public:
    FunctionDfnNode(std::string func_name,
                    std::vector<FunctionParameter> parameters,
                    std::vector<std::unique_ptr<StatementNode>> statements)
        : func_name{std::move(func_name)},
          parameters{std::move(parameters)},
          statements{std::move(statements)} {}

    void accept(Visitor *visitor);

private:
    std::string func_name;
    std::vector<FunctionParameter> parameters;
    std::vector<std::unique_ptr<StatementNode>> statements;
};

#endif
