# Unit tests, built on GoogleTest (fetched automatically below) and run via CTest.
#
#   cmake -S . -B build -DBUILD_TESTING=ON
#   cmake --build build
#   ctest --test-dir build --output-on-failure
#
# Or just: sh test.sh

# Prefer a locally installed GoogleTest (e.g. `brew install googletest`) so the
# build works fully offline. Only fall back to downloading it when no local
# copy is found. Set GOOSE_FETCH_GTEST=ON to force the download path.
option(GOOSE_FETCH_GTEST "Always fetch GoogleTest instead of using a local install" OFF)

if(NOT GOOSE_FETCH_GTEST)
    find_package(GTest QUIET)
endif()

if(NOT GTest_FOUND)
    message(STATUS "GoogleTest not found locally; fetching it (requires network on first configure).")
    include(FetchContent)

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(googletest)
else()
    message(STATUS "Using locally installed GoogleTest: ${GTest_DIR}")
endif()

# Register a GoogleTest executable and hook it into CTest.
#   goose_add_test(<name> SOURCES <src>... LIBS <goose libs>...)
# Include directories come transitively from the linked goose_* libraries, so
# each test only needs to declare which components it exercises.
function(goose_add_test name)
    cmake_parse_arguments(ARG "" "" "SOURCES;LIBS" ${ARGN})

    add_executable(${name} ${ARG_SOURCES})
    target_link_libraries(${name} PRIVATE ${ARG_LIBS} GTest::gtest_main)
    add_test(NAME ${name} COMMAND ${name})
endfunction()

goose_add_test(lexer_test
    SOURCES lexer_test.cpp
    LIBS    goose_lexer
)

goose_add_test(parser_test
    SOURCES parser_test.cpp
    LIBS    goose_lexer goose_parser
)
