#include <gtest/gtest.h>

#include <algorithm>
#include <filesystem>
#include <initializer_list>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>

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

    struct ExpectedParserNode
    {
        SymbolType symbol_type;
        std::optional<std::string> lexeme;
        std::optional<std::vector<ExpectedParserNode>> children;
    };

    ExpectedParserNode expect_node(NonTerminal symbol)
    {
        return {symbol, std::nullopt, std::nullopt};
    }

    ExpectedParserNode expect_node(
        NonTerminal symbol,
        std::initializer_list<ExpectedParserNode> children)
    {
        return {symbol, std::nullopt,
                std::vector<ExpectedParserNode>{children}};
    }

    ExpectedParserNode expect_token(
        Terminal symbol,
        std::optional<std::string> lexeme = std::nullopt)
    {
        return {symbol, std::move(lexeme),
                std::vector<ExpectedParserNode>{}};
    }

    std::string symbol_name(SymbolType symbol)
    {
        if (std::holds_alternative<Terminal>(symbol))
            return to_string(std::get<Terminal>(symbol));
        return to_string(std::get<NonTerminal>(symbol));
    }

    testing::AssertionResult matches_tree(
        const ParserNode &actual,
        const ExpectedParserNode &expected,
        const std::string &path = "root")
    {
        if (actual.symbol_type != expected.symbol_type)
        {
            return testing::AssertionFailure()
                   << path << ": expected " << symbol_name(expected.symbol_type)
                   << ", got " << symbol_name(actual.symbol_type);
        }

        if (actual.lexeme != expected.lexeme)
        {
            const std::string expected_lexeme =
                expected.lexeme ? *expected.lexeme : "<none>";
            const std::string actual_lexeme =
                actual.lexeme ? *actual.lexeme : "<none>";
            return testing::AssertionFailure()
                   << path << ": expected lexeme " << expected_lexeme
                   << ", got " << actual_lexeme;
        }

        // Omitting children intentionally matches only this node's symbol and
        // lexeme. Supplying children requests an exact recursive match.
        if (!expected.children)
            return testing::AssertionSuccess();

        if (actual.children.size() != expected.children->size())
        {
            return testing::AssertionFailure()
                   << path << " (" << symbol_name(actual.symbol_type)
                   << "): expected " << expected.children->size()
                   << " children, got " << actual.children.size();
        }

        for (size_t index = 0; index < expected.children->size(); ++index)
        {
            const std::string child_path =
                path + ".children[" + std::to_string(index) + "]";
            auto result = matches_tree(*actual.children[index],
                                       expected.children->at(index), child_path);
            if (!result)
                return result;
        }
        return testing::AssertionSuccess();
    }

    void expect_tree(const ParserNode &actual,
                     const ExpectedParserNode &expected)
    {
        EXPECT_TRUE(matches_tree(actual, expected));
    }

    std::vector<const ParserNode *> find_nodes(const ParserNode &root,
                                               SymbolType symbol)
    {
        std::vector<const ParserNode *> result;
        if (root.symbol_type == symbol)
            result.push_back(&root);
        for (const auto &child : root.children)
        {
            auto child_results = find_nodes(*child, symbol);
            result.insert(result.end(), child_results.begin(), child_results.end());
        }
        return result;
    }

    const ParserNode *find_identifier_factor(const ParserNode &root,
                                             const std::string &identifier)
    {
        for (const ParserNode *factor : find_nodes(root, FACTOR))
        {
            if (factor->children.empty())
                continue;
            const ParserNode &first_child = *factor->children.front();
            if (first_child.symbol_type == SymbolType{IDENTIFIER} &&
                first_child.lexeme == identifier)
                return factor;
        }
        return nullptr;
    }

    const ParserNode *find_expansion(const ParserNode &root,
                                     NonTerminal parent,
                                     Terminal leading_token)
    {
        for (const ParserNode *node : find_nodes(root, parent))
        {
            if (!node->children.empty() &&
                node->children.front()->symbol_type == SymbolType{leading_token})
                return node;
        }
        return nullptr;
    }

    ExpectedParserNode number_factor(const std::string &lexeme)
    {
        return expect_node(
            FACTOR,
            {expect_token(
                NUMBER_LITERAL,
                lexeme)});
    }

    ExpectedParserNode number_term(const std::string &lexeme)
    {
        return expect_node(
            TERM,
            {
                number_factor(lexeme),
                expect_node(
                    TERM_TAIL,
                    {expect_token(
                        EPSILON)}),
            });
    }

    ExpectedParserNode atomic_expression(ExpectedParserNode factor)
    {
        return expect_node(
            EXPR,
            {
                expect_node(
                    TERM,
                    {
                        std::move(factor),
                        expect_node(
                            TERM_TAIL,
                            {expect_token(
                                EPSILON)}),
                    }),
                expect_node(
                    EXPR_TAIL,
                    {expect_token(
                        EPSILON)}),
            });
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
    expect_tree(
        *tree,
        expect_node(
            START_SYMBOL,
            {
                expect_node(
                    EXPR,
                    {expect_token(
                        EPSILON)}),
                expect_token(
                    PERIOD,
                    "."),
            }));
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
    expect_tree(
        *tree,
        expect_node(
            START_SYMBOL,
            {
                expect_token(
                    FLAP,
                    "flap"),
                expect_token(
                    IDENTIFIER,
                    "x"),
                expect_token(
                    ASSIGN,
                    "="),
                expect_token(
                    NUMBER_LITERAL,
                    "42"),
            }));
    EXPECT_FALSE(has_childless_nonterminal(tree.get()));
}

TEST(ParserParse, SelectsProductionByLookahead)
{
    Grammar grammar = choice_grammar();
    LLParser parser{grammar};
    ASSERT_NO_THROW(parser.parse({{START, "start"}, {IDENTIFIER, "y"}}));

    auto tree = parser.get_result();
    ASSERT_NE(tree, nullptr);
    expect_tree(
        *tree,
        expect_node(
            START_SYMBOL,
            {
                expect_token(
                    START,
                    "start"),
                expect_token(
                    IDENTIFIER,
                    "y"),
            }));
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
    expect_tree(
        *tree,
        expect_node(
            START_SYMBOL,
            {
                expect_node(
                    EXPR,
                    {
                        expect_token(
                            FLAP,
                            "flap"),
                        expect_token(
                            IDENTIFIER,
                            "n"),
                    }),
                expect_token(
                    NUMBER_LITERAL,
                    "5"),
            }));
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
        {START, "start"},
        {FUNC_AT, "@"},
        {IDENTIFIER, "foo"},
        {FUNC_AT, "@"},
        {OPEN_PAREN, "("},
        {CLOSE_PAREN, ")"},
        {END, "end"},
        {FUNC_AT, "@"},
        {IDENTIFIER, "foo"},
        {FUNC_AT, "@"},
    };
    ASSERT_NO_THROW(parser.parse(tokens));

    auto tree = parser.get_result();
    ASSERT_NE(tree, nullptr);
    expect_tree(
        *tree,
        expect_node(
            START_SYMBOL,
            {
                expect_node(
                    FUNC_DFNS,
                    {
                        expect_node(
                            FUNC_DFN,
                            {
                                expect_token(
                                    START,
                                    "start"),
                                expect_token(
                                    FUNC_AT,
                                    "@"),
                                expect_token(
                                    IDENTIFIER,
                                    "foo"),
                                expect_token(
                                    FUNC_AT,
                                    "@"),
                                expect_token(
                                    OPEN_PAREN,
                                    "("),
                                expect_node(
                                    PARAMS,
                                    {expect_token(
                                        EPSILON)}),
                                expect_token(
                                    CLOSE_PAREN,
                                    ")"),
                                expect_node(
                                    STATEMENTS,
                                    {expect_token(
                                        EPSILON)}),
                                expect_token(
                                    END,
                                    "end"),
                                expect_token(
                                    FUNC_AT,
                                    "@"),
                                expect_token(
                                    IDENTIFIER,
                                    "foo"),
                                expect_token(
                                    FUNC_AT,
                                    "@"),
                            }),
                        expect_node(
                            FUNC_DFNS,
                            {expect_token(
                                EPSILON)}),
                    }),
            }));
    EXPECT_FALSE(has_childless_nonterminal(tree.get()));
}

TEST(ParserParse, TypedParametersAndStatementCallsParse)
{
    LLParser parser{goose_grammar};
    Tokens tokens = {
        {START, "start"},
        {FUNC_AT, "@"},
        {IDENTIFIER, "show"},
        {FUNC_AT, "@"},
        {OPEN_PAREN, "("},
        {NUMBER, "number"},
        {IDENTIFIER, "value"},
        {COMMA, ","},
        {WORD, "word"},
        {IDENTIFIER, "label"},
        {CLOSE_PAREN, ")"},
        {IDENTIFIER, "honk"},
        {ARG_OPEN_PAREN, "<<"},
        {STRING_LITERAL, "\"value: \""},
        {COMMA, ","},
        {IDENTIFIER, "value"},
        {ARG_CLOSE_PAREN, ">>"},
        {PERIOD, "."},
        {END, "end"},
        {FUNC_AT, "@"},
        {IDENTIFIER, "show"},
        {FUNC_AT, "@"},
    };
    ASSERT_NO_THROW(parser.parse(tokens));

    auto tree = parser.get_result();
    ASSERT_NE(tree, nullptr);
    const auto params = find_nodes(*tree, PARAMS);
    ASSERT_EQ(params.size(), 1u);
    expect_tree(
        *params.front(),
        expect_node(
            PARAMS,
            {
                expect_node(
                    PARAM,
                    {
                        expect_node(
                            TYPE_NAME,
                            {expect_token(
                                NUMBER,
                                "number")}),
                        expect_token(
                            IDENTIFIER,
                            "value"),
                    }),
                expect_node(
                    PARAMS_TAIL,
                    {
                        expect_token(
                            COMMA,
                            ","),
                        expect_node(
                            PARAM,
                            {
                                expect_node(
                                    TYPE_NAME,
                                    {expect_token(
                                        WORD,
                                        "word")}),
                                expect_token(
                                    IDENTIFIER,
                                    "label"),
                            }),
                        expect_node(
                            PARAMS_TAIL,
                            {expect_token(
                                EPSILON)}),
                    }),
            }));

    const auto raw_statements = find_nodes(*tree, RAW_STATEMENT);
    ASSERT_EQ(raw_statements.size(), 1u);
    expect_tree(
        *raw_statements.front(),
        expect_node(
            RAW_STATEMENT,
            {
                expect_token(
                    IDENTIFIER,
                    "honk"),
                expect_node(
                    ID_START_STATEMENT,
                    {
                        expect_node(
                            FUNC_CALL_STATEMENT,
                            {
                                expect_token(
                                    ARG_OPEN_PAREN,
                                    "<<"),
                                expect_node(
                                    ARGS),
                                expect_token(
                                    ARG_CLOSE_PAREN,
                                    ">>"),
                            }),
                    }),
            }));

    const auto args = find_nodes(*tree, ARGS);
    ASSERT_EQ(args.size(), 1u);
    expect_tree(
        *args.front(),
        expect_node(
            ARGS,
            {
                expect_node(
                    ARG,
                    {atomic_expression(
                        expect_node(
                            FACTOR,
                            {expect_token(
                                STRING_LITERAL,
                                "\"value: \"")}))}),
                expect_node(
                    ARGS_TAIL,
                    {
                        expect_token(
                            COMMA,
                            ","),
                        expect_node(
                            ARG,
                            {atomic_expression(
                                expect_node(
                                    FACTOR,
                                    {
                                        expect_token(
                                            IDENTIFIER,
                                            "value"),
                                        expect_node(
                                            FACTOR_TAIL,
                                            {expect_token(
                                                EPSILON)}),
                                    }))}),
                        expect_node(
                            ARGS_TAIL,
                            {expect_token(
                                EPSILON)}),
                    }),
            }));
}

TEST(ParserParse, EmptyCallArgumentsParse)
{
    LLParser parser{goose_grammar};
    Tokens tokens = {
        {START, "start"},
        {FUNC_AT, "@"},
        {IDENTIFIER, "fly"},
        {FUNC_AT, "@"},
        {OPEN_PAREN, "("},
        {CLOSE_PAREN, ")"},
        {IDENTIFIER, "tick"},
        {ARG_OPEN_PAREN, "<<"},
        {ARG_CLOSE_PAREN, ">>"},
        {PERIOD, "."},
        {END, "end"},
        {FUNC_AT, "@"},
        {IDENTIFIER, "fly"},
        {FUNC_AT, "@"},
    };
    ASSERT_NO_THROW(parser.parse(tokens));

    auto tree = parser.get_result();
    ASSERT_NE(tree, nullptr);
    const auto calls = find_nodes(*tree, FUNC_CALL_STATEMENT);
    ASSERT_EQ(calls.size(), 1u);
    expect_tree(
        *calls.front(),
        expect_node(
            FUNC_CALL_STATEMENT,
            {
                expect_token(
                    ARG_OPEN_PAREN,
                    "<<"),
                expect_node(
                    ARGS,
                    {expect_token(
                        EPSILON)}),
                expect_token(
                    ARG_CLOSE_PAREN,
                    ">>"),
            }));
}

TEST(ParserExpression, CallsUseAngleDelimitersAndCanNest)
{
    auto tree = parse_expression({
        {IDENTIFIER, "outer"},
        {ARG_OPEN_PAREN, "<<"},
        {NUMBER_LITERAL, "1"},
        {COMMA, ","},
        {IDENTIFIER, "inner"},
        {ARG_OPEN_PAREN, "<<"},
        {NUMBER_LITERAL, "2"},
        {PLUS, "+"},
        {NUMBER_LITERAL, "3"},
        {ARG_CLOSE_PAREN, ">>"},
        {ARG_CLOSE_PAREN, ">>"},
        {TIMES, "*"},
        {NUMBER_LITERAL, "4"},
    });
    ASSERT_NE(tree, nullptr);

    const ParserNode *outer = find_identifier_factor(*tree, "outer");
    const ParserNode *inner = find_identifier_factor(*tree, "inner");
    ASSERT_NE(outer, nullptr);
    ASSERT_NE(inner, nullptr);
    expect_tree(
        *outer,
        expect_node(
            FACTOR,
            {
                expect_token(
                    IDENTIFIER,
                    "outer"),
                expect_node(
                    FACTOR_TAIL,
                    {
                        expect_token(
                            ARG_OPEN_PAREN,
                            "<<"),
                        expect_node(
                            ARGS),
                        expect_token(
                            ARG_CLOSE_PAREN,
                            ">>"),
                    }),
            }));
    expect_tree(
        *inner,
        expect_node(
            FACTOR,
            {
                expect_token(
                    IDENTIFIER,
                    "inner"),
                expect_node(
                    FACTOR_TAIL,
                    {
                        expect_token(
                            ARG_OPEN_PAREN,
                            "<<"),
                        expect_node(
                            ARGS),
                        expect_token(
                            ARG_CLOSE_PAREN,
                            ">>"),
                    }),
            }));

    const auto term_tails = find_nodes(*tree, TERM_TAIL);
    const auto multiplication = std::ranges::find_if(
        term_tails,
        [](const ParserNode *tail)
        {
            return !tail->children.empty() &&
                   tail->children.front()->symbol_type == SymbolType{TIMES};
        });
    ASSERT_NE(multiplication, term_tails.end());
    expect_tree(
        **multiplication,
        expect_node(
            TERM_TAIL,
            {
                expect_token(
                    TIMES,
                    "*"),
                number_factor("4"),
                expect_node(
                    TERM_TAIL,
                    {expect_token(
                        EPSILON)}),
            }));
}

TEST(ParserExpression, ArithmeticPrecedenceAndOperatorOrderArePreserved)
{
    auto tree = parse_expression({
        {NUMBER_LITERAL, "1"},
        {PLUS, "+"},
        {NUMBER_LITERAL, "2"},
        {TIMES, "*"},
        {NUMBER_LITERAL, "3"},
        {MINUS, "-"},
        {NUMBER_LITERAL, "4"},
        {DIVIDE, "/"},
        {NUMBER_LITERAL, "2"},
        {MOD, "%"},
        {NUMBER_LITERAL, "5"},
    });
    ASSERT_NE(tree, nullptr);

    expect_tree(
        *tree,
        expect_node(
            START_SYMBOL,
            {
                expect_node(
                    EXPR,
                    {
                        number_term("1"),
                        expect_node(
                            EXPR_TAIL,
                            {
                                expect_token(
                                    PLUS,
                                    "+"),
                                expect_node(
                                    TERM,
                                    {
                                        number_factor("2"),
                                        expect_node(
                                            TERM_TAIL,
                                            {
                                                expect_token(
                                                    TIMES,
                                                    "*"),
                                                number_factor("3"),
                                                expect_node(
                                                    TERM_TAIL,
                                                    {expect_token(
                                                        EPSILON)}),
                                            }),
                                    }),
                                expect_node(
                                    EXPR_TAIL,
                                    {
                                        expect_token(
                                            MINUS,
                                            "-"),
                                        expect_node(
                                            TERM,
                                            {
                                                number_factor("4"),
                                                expect_node(
                                                    TERM_TAIL,
                                                    {
                                                        expect_token(
                                                            DIVIDE,
                                                            "/"),
                                                        number_factor("2"),
                                                        expect_node(
                                                            TERM_TAIL,
                                                            {
                                                                expect_token(
                                                                    MOD,
                                                                    "%"),
                                                                number_factor("5"),
                                                                expect_node(
                                                                    TERM_TAIL,
                                                                    {expect_token(
                                                                        EPSILON)}),
                                                            }),
                                                    }),
                                            }),
                                        expect_node(
                                            EXPR_TAIL,
                                            {expect_token(
                                                EPSILON)}),
                                    }),
                            }),
                    }),
            }));
}

TEST(ParserExpression, SubtractionTailSupportsLeftToRightAstFolding)
{
    auto tree = parse_expression({
        {NUMBER_LITERAL, "10"},
        {MINUS, "-"},
        {NUMBER_LITERAL, "3"},
        {MINUS, "-"},
        {NUMBER_LITERAL, "2"},
    });
    ASSERT_NE(tree, nullptr);

    expect_tree(
        *tree,
        expect_node(
            START_SYMBOL,
            {
                expect_node(
                    EXPR,
                    {
                        number_term("10"),
                        expect_node(
                            EXPR_TAIL,
                            {
                                expect_token(
                                    MINUS,
                                    "-"),
                                number_term("3"),
                                expect_node(
                                    EXPR_TAIL,
                                    {
                                        expect_token(
                                            MINUS,
                                            "-"),
                                        number_term("2"),
                                        expect_node(
                                            EXPR_TAIL,
                                            {expect_token(
                                                EPSILON)}),
                                    }),
                            }),
                    }),
            }));
}

TEST(ParserExpression, MixedMultiplicativeTailSupportsLeftToRightAstFolding)
{
    auto tree = parse_expression({
        {NUMBER_LITERAL, "48"},
        {DIVIDE, "/"},
        {NUMBER_LITERAL, "6"},
        {TIMES, "*"},
        {NUMBER_LITERAL, "3"},
        {MOD, "%"},
        {NUMBER_LITERAL, "5"},
    });
    ASSERT_NE(tree, nullptr);

    expect_tree(
        *tree,
        expect_node(
            START_SYMBOL,
            {
                expect_node(
                    EXPR,
                    {
                        expect_node(
                            TERM,
                            {
                                number_factor("48"),
                                expect_node(
                                    TERM_TAIL,
                                    {
                                        expect_token(
                                            DIVIDE,
                                            "/"),
                                        number_factor("6"),
                                        expect_node(
                                            TERM_TAIL,
                                            {
                                                expect_token(
                                                    TIMES,
                                                    "*"),
                                                number_factor("3"),
                                                expect_node(
                                                    TERM_TAIL,
                                                    {
                                                        expect_token(
                                                            MOD,
                                                            "%"),
                                                        number_factor("5"),
                                                        expect_node(
                                                            TERM_TAIL,
                                                            {expect_token(
                                                                EPSILON)}),
                                                    }),
                                            }),
                                    }),
                            }),
                        expect_node(
                            EXPR_TAIL,
                            {expect_token(
                                EPSILON)}),
                    }),
            }));
}

TEST(ParserExpression, ParenthesesOverrideArithmeticPrecedence)
{
    auto tree = parse_expression({
        {OPEN_PAREN, "("},
        {NUMBER_LITERAL, "1"},
        {PLUS, "+"},
        {NUMBER_LITERAL, "2"},
        {CLOSE_PAREN, ")"},
        {TIMES, "*"},
        {NUMBER_LITERAL, "3"},
    });
    ASSERT_NE(tree, nullptr);

    const ExpectedParserNode grouped_expression = expect_node(
        EXPR,
        {
            number_term("1"),
            expect_node(
                EXPR_TAIL,
                {
                    expect_token(
                        PLUS,
                        "+"),
                    number_term("2"),
                    expect_node(
                        EXPR_TAIL,
                        {expect_token(
                            EPSILON)}),
                }),
        });
    expect_tree(
        *tree,
        expect_node(
            START_SYMBOL,
            {
                expect_node(
                    EXPR,
                    {
                        expect_node(
                            TERM,
                            {
                                expect_node(
                                    FACTOR,
                                    {
                                        expect_token(
                                            OPEN_PAREN,
                                            "("),
                                        grouped_expression,
                                        expect_token(
                                            CLOSE_PAREN,
                                            ")"),
                                    }),
                                expect_node(
                                    TERM_TAIL,
                                    {
                                        expect_token(
                                            TIMES,
                                            "*"),
                                        number_factor("3"),
                                        expect_node(
                                            TERM_TAIL,
                                            {expect_token(
                                                EPSILON)}),
                                    }),
                            }),
                        expect_node(
                            EXPR_TAIL,
                            {expect_token(
                                EPSILON)}),
                    }),
            }));
}

TEST(ParserExpression, ParenthesesAreNotFunctionCallDelimiters)
{
    Grammar grammar = expression_grammar();
    LLParser parser{grammar};
    EXPECT_THROW(parser.parse({
                     {IDENTIFIER, "foo"},
                     {OPEN_PAREN, "("},
                     {CLOSE_PAREN, ")"},
                 }),
                 std::runtime_error);
}

TEST(ParserExpression, MissingCallCloseDelimiterThrows)
{
    Grammar grammar = expression_grammar();
    LLParser parser{grammar};
    EXPECT_THROW(parser.parse({
                     {IDENTIFIER, "foo"},
                     {ARG_OPEN_PAREN, "<<"},
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

    const ParserNode *sum_call = find_identifier_factor(*tree, "sum");
    ASSERT_NE(sum_call, nullptr);
    expect_tree(
        *sum_call,
        expect_node(
            FACTOR,
            {
                expect_token(
                    IDENTIFIER,
                    "sum"),
                expect_node(
                    FACTOR_TAIL,
                    {
                        expect_token(
                            ARG_OPEN_PAREN,
                            "<<"),
                        expect_node(
                            ARGS),
                        expect_token(
                            ARG_CLOSE_PAREN,
                            ">>"),
                    }),
            }));
    const ParserNode *multiplication = find_expansion(*tree, TERM_TAIL, TIMES);
    const ParserNode *subtraction = find_expansion(*tree, EXPR_TAIL, MINUS);
    const ParserNode *division = find_expansion(*tree, TERM_TAIL, DIVIDE);
    const ParserNode *modulo = find_expansion(*tree, TERM_TAIL, MOD);
    ASSERT_NE(multiplication, nullptr);
    ASSERT_NE(subtraction, nullptr);
    ASSERT_NE(division, nullptr);
    ASSERT_NE(modulo, nullptr);

    expect_tree(
        *multiplication,
        expect_node(
            TERM_TAIL,
            {
                expect_token(
                    TIMES,
                    "*"),
                expect_node(
                    FACTOR),
                expect_node(
                    TERM_TAIL),
            }));
    expect_tree(
        *subtraction,
        expect_node(
            EXPR_TAIL,
            {
                expect_token(
                    MINUS,
                    "-"),
                expect_node(
                    TERM),
                expect_node(
                    EXPR_TAIL),
            }));
    expect_tree(
        *division,
        expect_node(
            TERM_TAIL,
            {
                expect_token(
                    DIVIDE,
                    "/"),
                expect_node(
                    FACTOR),
                expect_node(
                    TERM_TAIL),
            }));
    expect_tree(
        *modulo,
        expect_node(
            TERM_TAIL,
            {
                expect_token(
                    MOD,
                    "%"),
                expect_node(
                    FACTOR),
                expect_node(
                    TERM_TAIL),
            }));
}
