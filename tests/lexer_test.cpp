#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <symbol.h>
#include <lexer/dfa.h>
#include <lexer/state.h>

namespace fs = std::filesystem;

// Resolve a fixture file living under tests/fixtures
std::string fixture_path(const std::string& name) {
    fs::path base = fs::path(__FILE__).parent_path() / "fixtures" / name;
    return base.string();
}

class DFATest : public ::testing::Test {
protected:
    DFA dfa;
};

// Test basic identifier recognition
TEST_F(DFATest, ParseSimpleIdentifier) {
    std::string filename = fixture_path("simple_identifier.goose");
    
    dfa.parse(filename);
    auto tokens = dfa.get_result();
    
    // Should have: SOF, FLAP, IDENTIFIER, ASSIGN, NUMBER, PERIOD, EOF
    EXPECT_GT(tokens.size(), 0);
    EXPECT_EQ(tokens[0].token_type, SOF);
}

// Test keyword recognition (FLAP)
TEST_F(DFATest, RecognizeFlapKeyword) {
    std::string filename = fixture_path("keyword_flap.goose");
    
    dfa.parse(filename);
    auto tokens = dfa.get_result();
    
    // After processing, "flap" should be recognized as FLAP keyword
    bool found_flap = false;
    for (const auto& token : tokens) {
        if (token.token_type == FLAP && token.lexeme == "flap") {
            found_flap = true;
            break;
        }
    }
    EXPECT_TRUE(found_flap);
}

// Test keyword recognition (START)
TEST_F(DFATest, RecognizeStartKeyword) {
    std::string filename = fixture_path("keyword_start.goose");
    
    dfa.parse(filename);
    auto tokens = dfa.get_result();
    
    bool found_start = false;
    for (const auto& token : tokens) {
        if (token.token_type == START && token.lexeme == "start") {
            found_start = true;
            break;
        }
    }
    EXPECT_TRUE(found_start);
}

// Test keyword recognition (END)
TEST_F(DFATest, RecognizeEndKeyword) {
    std::string filename = fixture_path("keyword_end.goose");
    
    dfa.parse(filename);
    auto tokens = dfa.get_result();
    
    bool found_end = false;
    for (const auto& token : tokens) {
        if (token.token_type == END && token.lexeme == "end") {
            found_end = true;
            break;
        }
    }
    EXPECT_TRUE(found_end);
}

// Test number recognition
TEST_F(DFATest, RecognizeNumbers) {
    std::string filename = fixture_path("numbers.goose");
    
    dfa.parse(filename);
    auto tokens = dfa.get_result();
    
    // Count number tokens
    int number_count = 0;
    for (const auto& token : tokens) {
        if (token.token_type == NUMBER) {
            number_count++;
        }
    }
    EXPECT_EQ(number_count, 3);
}

// Test assignment operator
TEST_F(DFATest, RecognizeAssignmentOperator) {
    std::string filename = fixture_path("assignment.goose");
    
    dfa.parse(filename);
    auto tokens = dfa.get_result();
    
    bool found_assign = false;
    for (const auto& token : tokens) {
        if (token.token_type == ASSIGN) {
            found_assign = true;
            break;
        }
    }
    EXPECT_TRUE(found_assign);
}

// Test period (statement terminator)
TEST_F(DFATest, RecognizePeriod) {
    std::string filename = fixture_path("period.goose");
    
    dfa.parse(filename);
    auto tokens = dfa.get_result();
    
    bool found_period = false;
    for (const auto& token : tokens) {
        if (token.token_type == PERIOD) {
            found_period = true;
            break;
        }
    }
    EXPECT_TRUE(found_period);
}

// Test function decorator (@)
TEST_F(DFATest, RecognizeFunctionDecorator) {
    std::string filename = fixture_path("func_decorator.goose");
    
    dfa.parse(filename);
    auto tokens = dfa.get_result();
    
    // Should contain FUNC_AT tokens
    int func_at_count = 0;
    for (const auto& token : tokens) {
        if (token.token_type == FUNC_AT) {
            func_at_count++;
        }
    }
    EXPECT_EQ(func_at_count, 2); // @ at both sides
}

// Test parameter brackets << >>
TEST_F(DFATest, RecognizeParameterBrackets) {
    std::string filename = fixture_path("param_open.goose");
    
    dfa.parse(filename);
    auto tokens = dfa.get_result();
    
    bool found_open = false;
    for (const auto& token : tokens) {
        if (token.token_type == PARAM_OPEN_PAREN) {
            found_open = true;
            break;
        }
    }
    EXPECT_TRUE(found_open);
}

// Test comma separator
TEST_F(DFATest, RecognizeComma) {
    std::string filename = fixture_path("comma.goose");
    
    dfa.parse(filename);
    auto tokens = dfa.get_result();
    
    int comma_count = 0;
    for (const auto& token : tokens) {
        if (token.token_type == COMMA) {
            comma_count++;
        }
    }
    EXPECT_EQ(comma_count, 2);
}

// Test string with quotes
TEST_F(DFATest, RecognizeQuotedString) {
    std::string filename = fixture_path("quoted_string.goose");
    
    dfa.parse(filename);
    auto tokens = dfa.get_result();
    
    bool found_quote = false;
    for (const auto& token : tokens) {
        if (token.token_type == STRING_LITERAL) {
            found_quote = true;
            break;
        }
    }
    EXPECT_TRUE(found_quote);
}

// Test comment removal
TEST_F(DFATest, RemoveComments) {
    std::string filename = fixture_path("comment.goose");
    
    dfa.parse(filename);
    auto tokens = dfa.get_result();
    
    // Comments should be removed, so no COMMENT tokens after removal
    for (const auto& token : tokens) {
        EXPECT_NE(token.token_type, COMMENT);
    }
}

// Test complex program structure
TEST_F(DFATest, ParseComplexProgram) {
    std::string filename = fixture_path("complex_program.goose");
    
    dfa.parse(filename);
    auto tokens = dfa.get_result();
    
    // Verify we have tokens and they include key elements
    EXPECT_GT(tokens.size(), 0);
    
    bool has_start = false, has_flap = false, has_honk = false, has_end = false;
    for (const auto& token : tokens) {
        if (token.token_type == START) has_start = true;
        if (token.token_type == FLAP) has_flap = true;
        if (token.token_type == END) has_end = true;
    }
    
    EXPECT_TRUE(has_start);
    EXPECT_TRUE(has_flap);
    EXPECT_TRUE(has_end);
}

// Test file not found error
TEST_F(DFATest, FileNotFoundThrowsError) {
    EXPECT_THROW(dfa.parse("/nonexistent/file.goose"), std::runtime_error);
}

// Test multiple identifiers
TEST_F(DFATest, ParseMultipleIdentifiers) {
    std::string filename = fixture_path("multiple_identifiers.goose");
    
    dfa.parse(filename);
    auto tokens = dfa.get_result();
    
    // Count identifier tokens
    int identifier_count = 0;
    for (const auto& token : tokens) {
        if (token.token_type == IDENTIFIER) {
            identifier_count++;
        }
    }
    EXPECT_EQ(identifier_count, 3);
}

// Test empty file handling
TEST_F(DFATest, HandleEmptyFile) {
    std::string filename = fixture_path("empty.goose");
    
    dfa.parse(filename);
    auto tokens = dfa.get_result();
    
    // Should at least have SOF and EOF
    EXPECT_GE(tokens.size(), 1);
}

// Test whitespace handling
TEST_F(DFATest, WhitespaceTokens) {
    std::string filename = fixture_path("whitespace.goose");
    
    dfa.parse(filename);
    auto tokens = dfa.get_result();
    
    // Should have multiple tokens (at least identifiers and spaces/separators)
    EXPECT_GT(tokens.size(), 1);
    
    // Verify we have identifiers
    int identifier_count = 0;
    for (const auto& token : tokens) {
        if (token.token_type == IDENTIFIER) {
            identifier_count++;
        }
    }
    EXPECT_EQ(identifier_count, 3);
}

// Test consecutive operators
TEST_F(DFATest, ConsecutiveOperators) {
    std::string filename = fixture_path("consecutive_operators.goose");
    
    dfa.parse(filename);
    auto tokens = dfa.get_result();
    
    bool has_open = false, has_close = false;
    for (const auto& token : tokens) {
        if (token.token_type == PARAM_OPEN_PAREN) has_open = true;
        if (token.token_type == PARAM_CLOSE_PAREN) has_close = true;
    }
    
    EXPECT_TRUE(has_open);
    EXPECT_TRUE(has_close);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
