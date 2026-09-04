#ifndef __FUNCTION_NODE_H__
#define __FUNCTION_NODE_H__

#include <ast_node.h>
#include <type.h>
#include <statement_node.h>

#include <vector>

struct FunctionParameter
{
    Type type;
    std::string identifier;
};

class FunctionDfnNode final : public ASTNode
{
public:
    FunctionDfnNode(std::string func_name,
                    std::vector<FunctionParameter> parameters,
                    std::vector<std::unique_ptr<StatementNode>> statements,
                    std::optional<Type> return_type = std::nullopt);

    ~FunctionDfnNode() override;

    void accept(Visitor *visitor) override;

    std::string func_name;
    std::vector<FunctionParameter> parameters;
    std::vector<std::unique_ptr<StatementNode>> statements;
    std::optional<Type> return_type;
};

#endif
