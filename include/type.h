#ifndef __TYPE_H__
#define __TYPE_H__

#include <utility>
#include <vector>
#include <optional>
#include <unordered_map>

struct Type
{
public:
    virtual ~Type() {}
};

struct PrimitiveType : public Type
{
};

inline PrimitiveType Number;
inline PrimitiveType Word;

// recursive type
struct FunctionType : public Type
{
    std::vector<std::unique_ptr<Type>> parameters;
    std::optional<std::unique_ptr<Type>> return_type;
};

struct TypeTable
{
    std::string scope_name;

    // this map includes the types of local variables, including function names
    std::unordered_map<std::string, Type> types;

    // this map provides the tree structured for the nested symbol tables
    std::unordered_map<std::string, std::unique_ptr<TypeTable>> sub_function_tables;
};

#endif
