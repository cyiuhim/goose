cmake_minimum_required(VERSION 3.20)

project(goose LANGUAGES CXX)

# Default to a Debug build (adds -g) when the caller doesn't specify a build
# type, so debuggers get file/line info out of the box.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)

add_library(goose_common
    src/helper.cpp
)

target_include_directories(goose_common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_library(goose_lexer
    src/lexer/dfa.cpp
    src/lexer/state.cpp
    src/lexer/state_list.cpp
    src/lexer/trie_node.cpp
)

target_link_libraries(goose_lexer
    PUBLIC
        goose_common
)

target_include_directories(goose_lexer
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/lexer
)

add_library(goose_parser
    src/parser/parser.cpp
)

target_link_libraries(goose_parser
    PUBLIC
        goose_common
)

target_include_directories(goose_parser
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/lexer
        ${CMAKE_CURRENT_SOURCE_DIR}/include/parser
)

add_library(goose_ast_nodes
    src/ast_nodes/program_node.cpp
    src/ast_nodes/function_node.cpp
    src/ast_nodes/statement_node.cpp
    src/ast_nodes/expr_node.cpp
)

target_link_libraries(goose_ast_nodes
    PUBLIC
        goose_common
)

target_include_directories(goose_ast_nodes
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ast_nodes
)

add_library(goose_tree_transformer
    src/tree_transformer/tree_transformer.cpp
    src/tree_transformer/ast_printer.cpp
)

target_link_libraries(goose_tree_transformer
    PUBLIC
        goose_ast_nodes
)

target_include_directories(goose_tree_transformer
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ast_nodes
        ${CMAKE_CURRENT_SOURCE_DIR}/include/tree_transformer
)

add_executable(goose
    src/main.cpp
)

target_link_libraries(goose
    PRIVATE
        goose_lexer
        goose_parser
        goose_tree_transformer
)

target_include_directories(goose
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/lexer
        ${CMAKE_CURRENT_SOURCE_DIR}/include/parser
)

install(TARGETS goose RUNTIME DESTINATION bin)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
