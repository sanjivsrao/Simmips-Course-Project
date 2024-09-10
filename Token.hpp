#ifndef SIMMIPS_TOKEN_HPP
#define SIMMIPS_TOKEN_HPP

#include <string>
#include <list>
#include <iostream>

namespace simmips {

    // Enumerated type for different tokens
    enum TokenType {
        EOL,
        SEP,
        OPEN_PAREN,
        CLOSE_PAREN,
        STRING_DELIM,
        EQUAL,
        STRING,
        ERROR
    };

    class Token {
    private:
        // Token type
        TokenType type_;

        // Line number of token
        std::size_t lineNumber_;

        // Token value
        std::string value_;

    public:
        // construct a token type on line with empty value
        Token(TokenType type, std::size_t line);

        // construct a token type on line  with value
        Token(TokenType type, std::size_t line, const std::string& value);

        // return the token type
        TokenType type() const;

        // return the token's originating source line
        std::size_t line() const;

        // return a reference to the token value
        const std::string& value() const;
    };

    // Operator for printing tokens to std::cout
    std::ostream& operator<<(std::ostream& oss, TokenType t);

    // Clone of operator
    std::ostream& operator<<(std::ostream& oss, const Token& t);

    // Operator for comparing tokens
    bool operator==(const Token& t1, const Token& t2);

    // Operator for comparing tokens
    bool operator!=(const Token& t1, const Token& t2);

    // List of tokens
    typedef std::list<Token> TokenList;

}


#endif