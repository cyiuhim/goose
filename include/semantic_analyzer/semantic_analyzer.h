#ifndef __SEMANTIC_ANALYZER_H__
#define __SEMANTIC_ANALYZER_H__

#include <ast_nodes/program_node.h>

#include <type.h>

#include <memory>
#include <unordered_map>
#include <string>

class SemanticAnalyzer
{
public:
    void check_program(const std::unique_ptr<ProgramNode> ast);
    const ProgramTypeTable &get_type_table();

private:
    ProgramTypeTable type_table;
};

#endif
