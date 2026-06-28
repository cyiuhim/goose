cmake_minimum_required(VERSION 3.20)

project(goose LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)

add_library(goose_lexer
    src/lexer/dfa.cpp
    src/lexer/state.cpp
    src/lexer/state_list.cpp
    src/lexer/trie_node.cpp
)

target_include_directories(goose_lexer
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/lexer
)

add_library(goose_parser
    src/helper.cpp
    src/parser/parser.cpp
)

target_include_directories(goose_parser
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/lexer
        ${CMAKE_CURRENT_SOURCE_DIR}/include/parser
)

add_executable(goose
    src/main.cpp
)

target_link_libraries(goose
    PRIVATE
        goose_lexer
        goose_parser
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