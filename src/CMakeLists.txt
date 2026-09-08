set(GOOSE_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")

function(goose_enable_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

add_library(goose_common
    helper.cpp
)
target_include_directories(goose_common
    PUBLIC
        "${GOOSE_INCLUDE_DIR}"
)
goose_enable_warnings(goose_common)

add_library(goose_lexer
    lexer/dfa.cpp
    lexer/state.cpp
    lexer/state_list.cpp
    lexer/trie_node.cpp
)
target_link_libraries(goose_lexer
    PUBLIC
        goose_common
)
target_include_directories(goose_lexer
    PUBLIC
        "${GOOSE_INCLUDE_DIR}"
        "${GOOSE_INCLUDE_DIR}/lexer"
)
goose_enable_warnings(goose_lexer)

add_library(goose_parser
    parser/parser.cpp
)
target_link_libraries(goose_parser
    PUBLIC
        goose_common
)
target_include_directories(goose_parser
    PUBLIC
        "${GOOSE_INCLUDE_DIR}"
        "${GOOSE_INCLUDE_DIR}/parser"
)
goose_enable_warnings(goose_parser)

add_library(goose_ast_nodes
    ast_nodes/program_node.cpp
    ast_nodes/function_node.cpp
    ast_nodes/statement_node.cpp
    ast_nodes/expr_node.cpp
)
target_link_libraries(goose_ast_nodes
    PUBLIC
        goose_common
)
target_include_directories(goose_ast_nodes
    PUBLIC
        "${GOOSE_INCLUDE_DIR}"
        "${GOOSE_INCLUDE_DIR}/ast_nodes"
)
goose_enable_warnings(goose_ast_nodes)

add_library(goose_tree_transformer
    tree_transformer/tree_transformer.cpp
    tree_transformer/ast_printer.cpp
)
target_link_libraries(goose_tree_transformer
    PUBLIC
        goose_ast_nodes
)
target_include_directories(goose_tree_transformer
    PUBLIC
        "${GOOSE_INCLUDE_DIR}"
        "${GOOSE_INCLUDE_DIR}/tree_transformer"
)
goose_enable_warnings(goose_tree_transformer)

add_library(goose_semantic_analyzer
    semantic_analyzer/semantic_analyzer.cpp
)
target_link_libraries(goose_semantic_analyzer
    PUBLIC
        goose_ast_nodes
)
target_include_directories(goose_semantic_analyzer
    PUBLIC
        "${GOOSE_INCLUDE_DIR}"
        "${GOOSE_INCLUDE_DIR}/semantic_analyzer"
)
goose_enable_warnings(goose_semantic_analyzer)

add_executable(goose
    main.cpp
)
target_link_libraries(goose
    PRIVATE
        goose_lexer
        goose_parser
        goose_tree_transformer
        goose_semantic_analyzer
)
set_target_properties(goose PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
goose_enable_warnings(goose)
