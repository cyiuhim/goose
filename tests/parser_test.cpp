#include <gtest/gtest.h>

#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>
#include <variant>

#include <lexer/dfa.h>
#include <parser/grammar.h>
#include <parser/parser.h>
#include <symbol.h>

namespace
{
    namespace fs = std::filesystem;

    std::string fixture_path(const std::string &name)
    {
        return (fs::path(__FILE__).parent_path() / "fixtures" / name).string();
    }

    Grammar simple_grammar()
    {
        return {{START_SYMBOL, {{FLAP, IDENTIFIER, ASSIGN, NUMBER_LITERAL}}}};
    }

    Grammar choice_grammar()
    {
        return {{START_SYMBOL, {{FLAP, IDENTIFIER}, {START, IDENTIFIER}}}};
    }

    Grammar nested_grammar()
    {
        return {
            {START_SYMBOL, {{EXPR, NUMBER_LITERAL}}},
            {EXPR, {{FLAP, IDENTIFIER}}},
        };
    }

    Grammar expression_grammar()
    {
        Grammar grammar = goose_grammar;
        grammar[START_SYMBOL] = {{EXPR}};
        return grammar;
    }

    Terminal terminal_of(const ParserNode &node)
    {
        return std::get<Terminal>(node.symbol_type);
    }

    NonTerminal non_terminal_of(const ParserNode &node)
    {
        return std::get<NonTerminal>(node.symbol_type);
    }

    std::string to_sexpr(const ParserNode *node)
    {
        if (std::holds_alternative<Terminal>(node->symbol_type))
        {
            std::string result = to_string(std::get<Terminal>(node->symbol_type));
            if (node->lexeme)
                result += ":" + *node->lexeme;
            return result;
        }

        std::string result = "(" + to_string(std::get<NonTerminal>(node->symbol_type));
        for (const auto &child : node->children)
            result += " " + to_sexpr(child.get());
        return result + ")";
    }

    bool has_childless_nonterminal(const ParserNode *node)
    {
        if (std::holds_alternative<NonTerminal>(node->symbol_type) && node->children.empty())
            return true;
        for (const auto &child : node->children)
        {
            if (has_childless_nonterminal(child.get()))
                return true;
        }
        return false;
    }

    std::unique_ptr<ParserNode> parse_expression(const Tokens &tokens)
    {
        Grammar grammar = expression_grammar();
        LLParser parser{grammar};
        parser.parse(tokens);
        return parser.get_result();
    }
}

// ---------------------------------------------------------------------------
// Construction and LL(1) validation
// ---------------------------------------------------------------------------

TEST(ParserConstruction, GooseGrammarIsLL1)
{
    EXPECT_NO_THROW({ LLParser parser{goose_grammar}; });
}

TEST(ParserConstruction, DirectFirstSetConflictThrows)
{
    Grammar grammar = {
        {START_SYMBOL, {{FLAP, IDENTIFIER}, {FLAP, NUMBER_LITERAL}}},
    };
    EXPECT_THROW({ LLParser parser{grammar}; }, std::runtime_error);
}

TEST(ParserConstruction, IndirectFirstSetConflictThrows)
{
    Grammar grammar = {
        {START_SYMBOL, {{EXPR}}},
        {EXPR, {{TERM}, {FACTOR}}},
        {TERM, {{IDENTIFIER}}},
        {FACTOR, {{IDENTIFIER}}},
    };
    EXPECT_THROW({ LLParser parser{grammar}; }, std::runtime_error);
}

TEST(ParserConstruction, NullableFirstFollowConflictThrows)
{
    Grammar grammar = {
        {START_SYMBOL, {{EXPR, IDENTIFIER}}},
        {EXPR, {{IDENTIFIER}, {EPSILON}}},
    };
    EXPECT_THROW({ LLParser parser{grammar}; }, std::runtime_error);
}

TEST(ParserConstruction, ValidNullableChoiceSelectsEpsilon)
{
    Grammar grammar = {
        {START_SYMBOL, {{EXPR, PERIOD}}},
        {EXPR, {{IDENTIFIER}, {EPSILON}}},
    };
    LLParser parser{grammar};
    ASSERT_NO_THROW(parser.parse({{PERIOD, "."}}));

    auto tree = parser.get_result();
    ASSERT_NE(tree, nullptr);
    EXPECT_EQ(to_sexpr(tree.get()),
              "(START_SYMBOL (EXPR epsilon) .:.)");
}

// ---------------------------------------------------------------------------
// Generic parser behavior
// ---------------------------------------------------------------------------

TEST(ParserParse, ValidStreamProducesExpectedTree)
{
    Grammar grammar = simple_grammar();
    LLParser parser{grammar};
    Tokens tokens = {
        {FLAP, "flap"},
        {IDENTIFIER, "x"},
        {ASSIGN, "="},
        {NUMBER_LITERAL, "42"},
    };

    ASSERT_NO_THROW(parser.parse(tokens));
    auto tree = parser.get_result();
    ASSERT_NE(tree, nullptr);
    EXPECT_EQ(non_terminal_of(*tree), START_SYMBOL);
    ASSERT_EQ(tree->children.size(), 4u);
    EXPECT_EQ(terminal_of(*tree->children[3]), NUMBER_LITERAL);
    EXPECT_EQ(to_sexpr(tree.get()),
              "(START_SYMBOL flap:flap identifier:x =:= number literal:42)");
    EXPECT_FALSE(has_childless_nonterminal(tree.get()));
}

TEST(ParserParse, SelectsProductionByLookahead)
{
    Grammar grammar = choice_grammar();
    LLParser parser{grammar};
    ASSERT_NO_THROW(parser.parse({{START, "start"}, {IDENTIFIER, "y"}}));

    auto tree = parser.get_result();
    ASSERT_NE(tree, nullptr);
    EXPECT_EQ(to_sexpr(tree.get()),
              "(START_SYMBOL start:start identifier:y)");
}

TEST(ParserParse, NestedNonTerminalProducesSubtree)
{
    Grammar grammar = nested_grammar();
    LLParser parser{grammar};
    ASSERT_NO_THROW(parser.parse({
        {FLAP, "flap"},
        {IDENTIFIER, "n"},
        {NUMBER_LITERAL, "5"},
    }));

    auto tree = parser.get_result();
    ASSERT_NE(tree, nullptr);
    EXPECT_EQ(to_sexpr(tree.get()),
              "(START_SYMBOL (EXPR flap:flap identifier:n) number literal:5)");
}

TEST(ParserParse, GetResultMovesTree)
{
    Grammar grammar = simple_grammar();
    LLParser parser{grammar};
    parser.parse({
        {FLAP, "flap"},
        {IDENTIFIER, "y"},
        {ASSIGN, "="},
        {NUMBER_LITERAL, "7"},
    });

    EXPECT_NE(parser.get_result(), nullptr);
    EXPECT_EQ(parser.get_result(), nullptr);
}

TEST(ParserParse, TerminalMismatchThrows)
{
    Grammar grammar = simple_grammar();
    LLParser parser{grammar};
    EXPECT_THROW(parser.parse({{FLAP, "flap"}, {NUMBER_LITERAL, "42"}}),
                 std::runtime_error);
}

TEST(ParserParse, UnexpectedLeadingSymbolThrows)
{
    Grammar grammar = simple_grammar();
    LLParser parser{grammar};
    EXPECT_THROW(parser.parse({{NUMBER_LITERAL, "42"}}), std::runtime_error);
}

// ---------------------------------------------------------------------------
// Current Goose grammar
// ---------------------------------------------------------------------------

TEST(ParserParse, EmptyFunctionIncludesEmptyParameterList)
{
    LLParser parser{goose_grammar};
    Tokens tokens = {
        {START, "start"}, {FUNC_AT, "@"}, {IDENTIFIER, "foo"}, {FUNC_AT, "@"},
        {OPEN_PAREN, "("}, {CLOSE_PAREN, ")"},
        {END, "end"}, {FUNC_AT, "@"}, {IDENTIFIER, "foo"}, {FUNC_AT, "@"},
    };
    ASSERT_NO_THROW(parser.parse(tokens));

    auto tree = parser.get_result();
    ASSERT_NE(tree, nullptr);
    EXPECT_EQ(to_sexpr(tree.get()),
              "(START_SYMBOL (FUNC_DFNS (FUNC_DFN start:start @:@ identifier:foo @:@ "
              "(:( (PARAMS epsilon) ):) (STATEMENTS epsilon) end:end @:@ "
              "identifier:foo @:@) (FUNC_DFNS epsilon)))");
    EXPECT_FALSE(has_childless_nonterminal(tree.get()));
}

TEST(ParserParse, TypedParametersAndStatementCallsParse)
{
    LLParser parser{goose_grammar};
    Tokens tokens = {
        {START, "start"}, {FUNC_AT, "@"}, {IDENTIFIER, "show"}, {FUNC_AT, "@"},
        {OPEN_PAREN, "("},
        {NUMBER, "number"}, {IDENTIFIER, "value"}, {COMMA, ","},
        {WORD, "word"}, {IDENTIFIER, "label"},
        {CLOSE_PAREN, ")"},
        {IDENTIFIER, "honk"}, {ARG_OPEN_PAREN, "<<"},
        {STRING_LITERAL, "\"value: \""}, {COMMA, ","}, {IDENTIFIER, "value"},
        {ARG_CLOSE_PAREN, ">>"}, {PERIOD, "."},
        {END, "end"}, {FUNC_AT, "@"}, {IDENTIFIER, "show"}, {FUNC_AT, "@"},
    };
    ASSERT_NO_THROW(parser.parse(tokens));

    auto tree = parser.get_result();
    ASSERT_NE(tree, nullptr);
    const std::string tree_text = to_sexpr(tree.get());
    EXPECT_NE(tree_text.find("(PARAM (TYPE_NAME number:number) identifier:value)"),
              std::string::npos);
    EXPECT_NE(tree_text.find("(PARAMS_TAIL ,:, (PARAM (TYPE_NAME word:word) identifier:label)"),
              std::string::npos);
    EXPECT_NE(tree_text.find("identifier:honk (ID_START_STATEMENT (FUNC_CALL_STATEMENT <<:<<"),
              std::string::npos);
    EXPECT_NE(tree_text.find("(ARGS_TAIL ,:, (ARG (EXPR"), std::string::npos);
}

TEST(ParserParse, EmptyCallArgumentsParse)
{
    LLParser parser{goose_grammar};
    Tokens tokens = {
        {START, "start"}, {FUNC_AT, "@"}, {IDENTIFIER, "fly"}, {FUNC_AT, "@"},
        {OPEN_PAREN, "("}, {CLOSE_PAREN, ")"},
        {IDENTIFIER, "tick"}, {ARG_OPEN_PAREN, "<<"}, {ARG_CLOSE_PAREN, ">>"},
        {PERIOD, "."},
        {END, "end"}, {FUNC_AT, "@"}, {IDENTIFIER, "fly"}, {FUNC_AT, "@"},
    };
    ASSERT_NO_THROW(parser.parse(tokens));

    auto tree = parser.get_result();
    ASSERT_NE(tree, nullptr);
    EXPECT_NE(to_sexpr(tree.get()).find("(FUNC_CALL_STATEMENT <<:<< (ARGS epsilon) >>:>>"),
              std::string::npos);
}

TEST(ParserExpression, CallsUseAngleDelimitersAndCanNest)
{
    auto tree = parse_expression({
        {IDENTIFIER, "outer"}, {ARG_OPEN_PAREN, "<<"},
        {NUMBER_LITERAL, "1"}, {COMMA, ","},
        {IDENTIFIER, "inner"}, {ARG_OPEN_PAREN, "<<"},
        {NUMBER_LITERAL, "2"}, {PLUS, "+"}, {NUMBER_LITERAL, "3"},
        {ARG_CLOSE_PAREN, ">>"},
        {ARG_CLOSE_PAREN, ">>"},
        {TIMES, "*"}, {NUMBER_LITERAL, "4"},
    });
    ASSERT_NE(tree, nullptr);

    const std::string tree_text = to_sexpr(tree.get());
    EXPECT_NE(tree_text.find("(FACTOR identifier:outer (FACTOR_TAIL <<:<<"), std::string::npos);
    EXPECT_NE(tree_text.find("(FACTOR identifier:inner (FACTOR_TAIL <<:<<"), std::string::npos);
    EXPECT_NE(tree_text.find("(TERM_TAIL *:* (FACTOR number literal:4)"), std::string::npos);
}

TEST(ParserExpression, ArithmeticPrecedenceAndOperatorOrderArePreserved)
{
    auto tree = parse_expression({
        {NUMBER_LITERAL, "1"}, {PLUS, "+"},
        {NUMBER_LITERAL, "2"}, {TIMES, "*"}, {NUMBER_LITERAL, "3"},
        {MINUS, "-"},
        {NUMBER_LITERAL, "4"}, {DIVIDE, "/"}, {NUMBER_LITERAL, "2"},
        {MOD, "%"}, {NUMBER_LITERAL, "5"},
    });
    ASSERT_NE(tree, nullptr);

    EXPECT_EQ(to_sexpr(tree.get()),
              "(START_SYMBOL (EXPR (TERM (FACTOR number literal:1) (TERM_TAIL epsilon)) "
              "(EXPR_TAIL +:+ (TERM (FACTOR number literal:2) "
              "(TERM_TAIL *:* (FACTOR number literal:3) (TERM_TAIL epsilon))) "
              "(EXPR_TAIL -:- (TERM (FACTOR number literal:4) "
              "(TERM_TAIL /:/ (FACTOR number literal:2) "
              "(TERM_TAIL %:% (FACTOR number literal:5) (TERM_TAIL epsilon)))) "
              "(EXPR_TAIL epsilon)))))");
}

TEST(ParserExpression, SubtractionTailSupportsLeftToRightAstFolding)
{
    auto tree = parse_expression({
        {NUMBER_LITERAL, "10"}, {MINUS, "-"},
        {NUMBER_LITERAL, "3"}, {MINUS, "-"},
        {NUMBER_LITERAL, "2"},
    });
    ASSERT_NE(tree, nullptr);

    EXPECT_EQ(to_sexpr(tree.get()),
              "(START_SYMBOL (EXPR (TERM (FACTOR number literal:10) (TERM_TAIL epsilon)) "
              "(EXPR_TAIL -:- (TERM (FACTOR number literal:3) (TERM_TAIL epsilon)) "
              "(EXPR_TAIL -:- (TERM (FACTOR number literal:2) (TERM_TAIL epsilon)) "
              "(EXPR_TAIL epsilon)))))");
}

TEST(ParserExpression, ParenthesesOverrideArithmeticPrecedence)
{
    auto tree = parse_expression({
        {OPEN_PAREN, "("}, {NUMBER_LITERAL, "1"}, {PLUS, "+"},
        {NUMBER_LITERAL, "2"}, {CLOSE_PAREN, ")"},
        {TIMES, "*"}, {NUMBER_LITERAL, "3"},
    });
    ASSERT_NE(tree, nullptr);

    const std::string tree_text = to_sexpr(tree.get());
    const size_t grouped_expression = tree_text.find("(FACTOR (:( (EXPR");
    const size_t outer_multiplication =
        tree_text.find("(TERM_TAIL *:* (FACTOR number literal:3)");
    ASSERT_NE(grouped_expression, std::string::npos);
    ASSERT_NE(outer_multiplication, std::string::npos);
    EXPECT_LT(grouped_expression, outer_multiplication);
}

TEST(ParserExpression, ParenthesesAreNotFunctionCallDelimiters)
{
    Grammar grammar = expression_grammar();
    LLParser parser{grammar};
    EXPECT_THROW(parser.parse({
                     {IDENTIFIER, "foo"}, {OPEN_PAREN, "("}, {CLOSE_PAREN, ")"},
                 }),
                 std::runtime_error);
}

TEST(ParserExpression, MissingCallCloseDelimiterThrows)
{
    Grammar grammar = expression_grammar();
    LLParser parser{grammar};
    EXPECT_THROW(parser.parse({
                     {IDENTIFIER, "foo"}, {ARG_OPEN_PAREN, "<<"},
                     {NUMBER_LITERAL, "1"},
                 }),
                 std::runtime_error);
}

TEST(ParserIntegration, LexerOutputParsesCurrentCallsParametersAndArithmetic)
{
    DFA dfa;
    ASSERT_NO_THROW(dfa.parse(fixture_path("function_calls_and_arithmetic.goose")));
    const Tokens tokens = dfa.get_result();
    ASSERT_FALSE(tokens.empty());

    LLParser parser{goose_grammar};
    ASSERT_NO_THROW(parser.parse(tokens));
    auto tree = parser.get_result();
    ASSERT_NE(tree, nullptr);
    EXPECT_FALSE(has_childless_nonterminal(tree.get()));

    const std::string tree_text = to_sexpr(tree.get());
    EXPECT_NE(tree_text.find("identifier:sum (FACTOR_TAIL <<:<<"), std::string::npos);
    EXPECT_NE(tree_text.find("(TERM_TAIL *:*"), std::string::npos);
    EXPECT_NE(tree_text.find("(EXPR_TAIL -:-"), std::string::npos);
    EXPECT_NE(tree_text.find("(TERM_TAIL /:/"), std::string::npos);
    EXPECT_NE(tree_text.find("(TERM_TAIL %:%"), std::string::npos);
}
