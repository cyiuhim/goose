#include <gtest/gtest.h>

#include <parser/parser.h>
#include <parser/grammar.h>
#include <symbol.h>

#include <stdexcept>
#include <variant>

// NOTE ON TOKEN STREAMS
// ---------------------
// The lexer never emits the END_SYMBOL ($) sentinel; it only produces the
// real terminals. The parser seeds its symbol stack with END_SYMBOL as an
// internal bottom-of-stack marker and simply stops once the token stream is
// exhausted (its loop runs `while (token_idx < tokens.size())`), leaving that
// sentinel unconsumed. Therefore the token streams below deliberately do NOT
// include END_SYMBOL — appending it makes the parser try to match the
// sentinel and dereference an empty node stack (segfault).

namespace {

// A tiny, epsilon-free grammar for deterministic positive parsing tests:
//
//   START_SYMBOL -> FLAP IDENTIFIER ASSIGN NUMBER
//
Grammar simple_grammar() {
    return {
        {START_SYMBOL, {
            {FLAP, IDENTIFIER, ASSIGN, NUMBER}
        }}
    };
}

// A grammar with a genuine LL(1) choice (distinct FIRST terminals):
//
//   START_SYMBOL -> FLAP IDENTIFIER
//                 | START IDENTIFIER
//
Grammar choice_grammar() {
    return {
        {START_SYMBOL, {
            {FLAP, IDENTIFIER},
            {START, IDENTIFIER},
        }}
    };
}

// A grammar with a nested non-terminal, producing a multi-level tree:
//
//   START_SYMBOL -> EXPR NUMBER
//   EXPR         -> FLAP IDENTIFIER
//
Grammar nested_grammar() {
    return {
        {START_SYMBOL, {
            {EXPR, NUMBER}
        }},
        {EXPR, {
            {FLAP, IDENTIFIER}
        }},
    };
}

Terminal terminal_of(const ParserNode& node) {
    return std::get<Terminal>(node.symbol_type);
}

NonTerminal non_terminal_of(const ParserNode& node) {
    return std::get<NonTerminal>(node.symbol_type);
}

} // namespace

// ---------------------------------------------------------------------------
// Construction / grammar validation
// ---------------------------------------------------------------------------

TEST(ParserConstruction, GooseGrammarIsLL1) {
    EXPECT_NO_THROW({ LLParser parser{goose_grammar}; });
}

TEST(ParserConstruction, AmbiguousGrammarThrows) {
    Grammar ambiguous = {
        {START_SYMBOL, {
            {FLAP, IDENTIFIER},
            {FLAP, NUMBER},   // overlapping FIRST terminal (FLAP)
        }}
    };
    EXPECT_THROW({ LLParser parser{ambiguous}; }, std::runtime_error);
}

// ---------------------------------------------------------------------------
// Parsing: happy path
// ---------------------------------------------------------------------------

TEST(ParserParse, ValidStreamProducesExpectedTree) {
    Grammar g = simple_grammar();
    LLParser parser{g};

    Tokens tokens = {
        {FLAP, "flap"},
        {IDENTIFIER, "x"},
        {ASSIGN, "="},
        {NUMBER, "42"},
    };

    ASSERT_NO_THROW(parser.parse(tokens));

    auto tree = parser.get_result();
    ASSERT_NE(tree, nullptr);

    ASSERT_TRUE(std::holds_alternative<NonTerminal>(tree->symbol_type));
    EXPECT_EQ(non_terminal_of(*tree), START_SYMBOL);

    ASSERT_EQ(tree->children.size(), 4u);
    EXPECT_EQ(terminal_of(*tree->children[0]), FLAP);
    EXPECT_EQ(terminal_of(*tree->children[1]), IDENTIFIER);
    EXPECT_EQ(terminal_of(*tree->children[2]), ASSIGN);
    EXPECT_EQ(terminal_of(*tree->children[3]), NUMBER);

    ASSERT_TRUE(tree->children[1]->lexeme.has_value());
    EXPECT_EQ(*tree->children[1]->lexeme, "x");
    ASSERT_TRUE(tree->children[3]->lexeme.has_value());
    EXPECT_EQ(*tree->children[3]->lexeme, "42");
}

// The parser table must pick the correct production based on the lookahead.
TEST(ParserParse, SelectsProductionByLookahead) {
    Grammar g = choice_grammar();
    LLParser parser{g};

    Tokens tokens = {
        {START, "start"},
        {IDENTIFIER, "y"},
    };

    ASSERT_NO_THROW(parser.parse(tokens));
    auto tree = parser.get_result();
    ASSERT_NE(tree, nullptr);

    ASSERT_EQ(tree->children.size(), 2u);
    EXPECT_EQ(terminal_of(*tree->children[0]), START);
    EXPECT_EQ(terminal_of(*tree->children[1]), IDENTIFIER);
    ASSERT_TRUE(tree->children[0]->lexeme.has_value());
    EXPECT_EQ(*tree->children[0]->lexeme, "start");
}

// A nested non-terminal should yield a nested subtree.
TEST(ParserParse, NestedNonTerminalProducesSubtree) {
    Grammar g = nested_grammar();
    LLParser parser{g};

    Tokens tokens = {
        {FLAP, "flap"},
        {IDENTIFIER, "n"},
        {NUMBER, "5"},
    };

    ASSERT_NO_THROW(parser.parse(tokens));
    auto tree = parser.get_result();
    ASSERT_NE(tree, nullptr);

    // root: START_SYMBOL -> [ EXPR, NUMBER ]
    EXPECT_EQ(non_terminal_of(*tree), START_SYMBOL);
    ASSERT_EQ(tree->children.size(), 2u);

    const ParserNode& expr = *tree->children[0];
    ASSERT_TRUE(std::holds_alternative<NonTerminal>(expr.symbol_type));
    EXPECT_EQ(non_terminal_of(expr), EXPR);

    // EXPR -> [ FLAP, IDENTIFIER ]
    ASSERT_EQ(expr.children.size(), 2u);
    EXPECT_EQ(terminal_of(*expr.children[0]), FLAP);
    EXPECT_EQ(terminal_of(*expr.children[1]), IDENTIFIER);

    // trailing NUMBER on the root
    EXPECT_EQ(terminal_of(*tree->children[1]), NUMBER);
    ASSERT_TRUE(tree->children[1]->lexeme.has_value());
    EXPECT_EQ(*tree->children[1]->lexeme, "5");
}

// get_result() transfers ownership; a second call yields nullptr.
TEST(ParserParse, GetResultMovesTree) {
    Grammar g = simple_grammar();
    LLParser parser{g};

    Tokens tokens = {
        {FLAP, "flap"},
        {IDENTIFIER, "y"},
        {ASSIGN, "="},
        {NUMBER, "7"},
    };
    parser.parse(tokens);

    auto first = parser.get_result();
    EXPECT_NE(first, nullptr);
    auto second = parser.get_result();
    EXPECT_EQ(second, nullptr);
}

// ---------------------------------------------------------------------------
// Parsing: error paths
// ---------------------------------------------------------------------------

// A terminal on the stack that does not match the current token throws.
TEST(ParserParse, TerminalMismatchThrows) {
    Grammar g = simple_grammar();
    LLParser parser{g};

    Tokens tokens = {
        {FLAP, "flap"},
        {NUMBER, "42"},   // grammar expects IDENTIFIER here
    };
    EXPECT_THROW(parser.parse(tokens), std::runtime_error);
}

// A leading token with no entry in the parser table throws.
TEST(ParserParse, UnexpectedLeadingSymbolThrows) {
    Grammar g = simple_grammar();
    LLParser parser{g};

    Tokens tokens = {
        {NUMBER, "42"},   // START_SYMBOL can only begin with FLAP
    };
    EXPECT_THROW(parser.parse(tokens), std::runtime_error);
}

TEST(ParserParse, GooseEmptyFunctionParses) {
    LLParser parser{goose_grammar};

    Tokens tokens = {
        {START, "start"}, {FUNC_AT, "@"}, {IDENTIFIER, "foo"}, {FUNC_AT, "@"},
        {END, "end"},     {FUNC_AT, "@"}, {IDENTIFIER, "foo"}, {FUNC_AT, "@"},
    };
    EXPECT_NO_THROW(parser.parse(tokens));
}
