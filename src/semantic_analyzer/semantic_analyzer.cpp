#include <semantic_analyzer/semantic_analyzer.h>
#include <semantic_analyzer/type_check_visitor.h>

void SemanticAnalyzer::check_program(const std::unique_ptr<ProgramNode> ast)
{
    // use a visitor to gather the function types, then type tables within each function
    TypeCheckVisitor visitor;
    ast->accept(&visitor);
    // check for fly function and its correctness on types
}

const TypeTable &SemanticAnalyzer::get_type_table()
{
    return type_table;
}
