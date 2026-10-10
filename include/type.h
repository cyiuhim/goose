#ifndef __TYPE_H__
#define __TYPE_H__

#include <utility>
#include <vector>
#include <optional>
#include <unordered_map>

enum PrimitiveType
{
    Number,
    Word
};

// extendable in the future, if supporting more types, can change this
// this will trigger compile time checks
using Type = PrimitiveType;

using FunctionType = std::pair<std::vector<Type>, std::optional<Type>>;

using FunctionTypeTable = std::unordered_map<std::string, Type>;

// so the ProgramTypeTable should be storing the types of functions with global scope
// each function would be storing the names of the local variables (FunctionTypeTable)
struct ProgramTypeTable
{
    std::unordered_map<std::string, FunctionTypeTable> tables_per_function;
    std::unordered_map<std::string, FunctionType> function_types;
};

#endif
