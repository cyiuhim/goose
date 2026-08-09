#include <gtest/gtest.h>

#include <algorithm>
#include <filesystem>
#include <initializer_list>
#include <string>
#include <utility>
#include <vector>

#include <lexer/dfa.h>
#include <symbol.h>

namespace
{
    namespace fs = std::filesystem;

    std::string fixture_path(const std::string &name)
    {
        return (fs::path(__FILE__).parent_path() / "fixtures" / "lexer" / name).string();
    }

    void expect_tokens(const Tokens &actual,
                       std::initializer_list<std::pair<Terminal, std::string>> expected)
    {
        ASSERT_EQ(actual.size(), expected.size());
        size_t index = 0;
        for (const auto &[type, lexeme] : expected)
        {
            EXPECT_EQ(actual[index].token_type, type) << "token index " << index;
            EXPECT_EQ(actual[index].lexeme, lexeme) << "token index " << index;
            ++index;
        }
    }

    size_t count_token(const Tokens &tokens, Terminal type)
    {
        return static_cast<size_t>(std::count_if(tokens.begin(), tokens.end(),
                                                 [type](const LexerToken &token)
                                                 { return token.token_type == type; }));
    }

    bool contains_subsequence(const Tokens &tokens,
                              std::initializer_list<Terminal> expected)
    {
        if (expected.size() > tokens.size())
            return false;

        for (size_t start = 0; start + expected.size() <= tokens.size(); ++start)
        {
            size_t offset = 0;
            bool matches = true;
            for (Terminal type : expected)
            {
                if (tokens[start + offset].token_type != type)
                {
                    matches = false;
                    break;
                }
                ++offset;
            }
            if (matches)
                return true;
        }
        return false;
    }

    class DFATest : public ::testing::Test
    {
    protected:
        Tokens lex(const std::string &fixture)
        {
            dfa.parse(fixture_path(fixture));
            return dfa.get_result();
        }

        DFA dfa;
    };
}

TEST_F(DFATest, TokenizesDeclarationExactly)
{
    expect_tokens(lex("simple_identifier.goose"), {
                                                      {FLAP, "flap"},
                                                      {IDENTIFIER, "myVar"},
                                                      {ASSIGN, "="},
                                                      {NUMBER_LITERAL, "5"},
                                                      {PERIOD, "."},
                                                  });
}

TEST_F(DFATest, RecognizesLanguageKeywords)
{
    expect_tokens(lex("keyword_flap.goose"), {{FLAP, "flap"}});
    expect_tokens(lex("keyword_start.goose"), {{START, "start"}});
    expect_tokens(lex("keyword_end.goose"), {{END, "end"}});
}

TEST_F(DFATest, DistinguishesNumberLiteralsFromNumberTypeKeyword)
{
    expect_tokens(lex("numbers.goose"), {
                                            {NUMBER_LITERAL, "42"},
                                            {NUMBER_LITERAL, "123"},
                                            {NUMBER_LITERAL, "999"},
                                        });

    const auto tokens = lex("function_calls_and_arithmetic.goose");
    EXPECT_EQ(count_token(tokens, NUMBER), 2u);
    EXPECT_GT(count_token(tokens, NUMBER_LITERAL), 2u);
}

TEST_F(DFATest, RecognizesAssignmentPeriodAndFunctionDecorator)
{
    expect_tokens(lex("assignment.goose"), {
                                               {IDENTIFIER, "x"},
                                               {ASSIGN, "="},
                                               {NUMBER_LITERAL, "10"},
                                           });
    expect_tokens(lex("period.goose"), {
                                           {IDENTIFIER, "x"},
                                           {ASSIGN, "="},
                                           {NUMBER_LITERAL, "10"},
                                           {PERIOD, "."},
                                       });
    expect_tokens(lex("func_decorator.goose"), {
                                                   {FUNC_AT, "@"},
                                                   {IDENTIFIER, "myFunc"},
                                                   {FUNC_AT, "@"},
                                               });
}

TEST_F(DFATest, RecognizesCallDelimitersExactly)
{
    expect_tokens(lex("consecutive_operators.goose"), {
                                                          {ARG_OPEN_PAREN, "<<"},
                                                          {ARG_CLOSE_PAREN, ">>"},
                                                      });
}

TEST_F(DFATest, RecognizesStringsAndCommaSeparatedValues)
{
    const auto string_tokens = lex("quoted_string.goose");
    ASSERT_EQ(count_token(string_tokens, STRING_LITERAL), 1u);

    const auto comma_tokens = lex("comma.goose");
    EXPECT_EQ(count_token(comma_tokens, COMMA), 2u);
}

TEST_F(DFATest, RemovesCommentsAndWhitespaceTokens)
{
    const auto comment_tokens = lex("comment.goose");
    for (const auto &token : comment_tokens)
    {
        EXPECT_NE(token.token_type, COMMENT);
        EXPECT_NE(token.token_type, SPACES);
    }

    const auto whitespace_tokens = lex("whitespace.goose");
    EXPECT_EQ(count_token(whitespace_tokens, IDENTIFIER), 3u);
    EXPECT_EQ(count_token(whitespace_tokens, SPACES), 0u);
}

TEST_F(DFATest, TokenizesFunctionParametersCallsGroupingAndArithmetic)
{
    const auto tokens = lex("function_calls_and_arithmetic.goose");

    EXPECT_EQ(count_token(tokens, START), 2u);
    EXPECT_EQ(count_token(tokens, END), 2u);
    EXPECT_EQ(count_token(tokens, OPEN_PAREN), 3u);
    EXPECT_EQ(count_token(tokens, CLOSE_PAREN), 3u);
    EXPECT_EQ(count_token(tokens, ARG_OPEN_PAREN), 3u);
    EXPECT_EQ(count_token(tokens, ARG_CLOSE_PAREN), 3u);
    EXPECT_EQ(count_token(tokens, RETURN), 1u);
    EXPECT_EQ(count_token(tokens, PLUS), 2u);
    EXPECT_EQ(count_token(tokens, MINUS), 1u);
    EXPECT_EQ(count_token(tokens, TIMES), 1u);
    EXPECT_EQ(count_token(tokens, DIVIDE), 1u);
    EXPECT_EQ(count_token(tokens, MOD), 1u);

    EXPECT_TRUE(contains_subsequence(tokens, {START, FUNC_AT, IDENTIFIER, FUNC_AT,
                                              OPEN_PAREN, NUMBER, IDENTIFIER, COMMA,
                                              NUMBER, IDENTIFIER, CLOSE_PAREN}));
    EXPECT_TRUE(contains_subsequence(tokens, {IDENTIFIER, ARG_OPEN_PAREN,
                                              NUMBER_LITERAL, COMMA, NUMBER_LITERAL,
                                              TIMES, OPEN_PAREN}));
    EXPECT_TRUE(contains_subsequence(tokens, {IDENTIFIER, ARG_OPEN_PAREN,
                                              STRING_LITERAL, COMMA, IDENTIFIER,
                                              COMMA, IDENTIFIER, ARG_OPEN_PAREN}));
}

TEST_F(DFATest, TokenizesUpdatedComplexProgram)
{
    const auto tokens = lex("complex_program.goose");
    EXPECT_TRUE(contains_subsequence(tokens, {START, FUNC_AT, IDENTIFIER, FUNC_AT,
                                              OPEN_PAREN, CLOSE_PAREN}));
    EXPECT_TRUE(contains_subsequence(tokens, {IDENTIFIER, ARG_OPEN_PAREN,
                                              STRING_LITERAL, COMMA, IDENTIFIER,
                                              ARG_CLOSE_PAREN, PERIOD}));
}

TEST_F(DFATest, RecognizesMultipleIdentifiers)
{
    EXPECT_EQ(count_token(lex("multiple_identifiers.goose"), IDENTIFIER), 3u);
}

TEST_F(DFATest, FileNotFoundThrowsError)
{
    EXPECT_THROW(dfa.parse("/nonexistent/file.goose"), std::runtime_error);
}
