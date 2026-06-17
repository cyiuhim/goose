#ifndef __TOKEN_H__
#define __TOKEN_H__

enum TokenType {
    SOF, // start of file
    _EOF, // end of file
    IDENTIFIER, // any identifier
    NUMBER, // number
    ASSIGN, // =
    PERIOD, // end of statement
    PARAM_OPEN_PARAN, // <<
    PARAM_CLOSE_PARAN, // >>
    FUNC_AT, // function at symbol, @
    COMMENT, // comment
    SPACES, // one or more spaces
    STRING_LITERAL, // wrapped inside a pair of double quotes
    COMMA, // ,

    // keywords
    FLAP, // flap (let)
    START, // start
    END, // end
};

typedef std::vector<std::pair<TokenType, std::string>> Tokens;

#endif 
