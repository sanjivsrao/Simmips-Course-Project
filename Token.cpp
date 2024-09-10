#include "Token.hpp"
namespace simmips {
    Token::Token(TokenType type, std::size_t line) : type_(type), lineNumber_(line) {}

    Token::Token(TokenType type, std::size_t line, const std::string& value) : type_(type), lineNumber_(line), value_(value) {}

    TokenType Token::type() const {
        return this->type_;
    }

    std::size_t Token::line() const {
        return this->lineNumber_;
    }

    const std::string& Token::value() const {
        return this->value_;
    }

    std::ostream& operator<<(std::ostream& oss, TokenType t) {
        switch (t) {
        case TokenType::EOL:
            oss << "EOL";
            break;
        case TokenType::SEP:
            oss << "SEP";
            break;
        case TokenType::OPEN_PAREN:
            oss << "OPEN_PAREN";
            break;
        case TokenType::CLOSE_PAREN:
            oss << "CLOSE_PAREN";
            break;
        case TokenType::STRING_DELIM:
            oss << "STRING_DELIM";
            break;
        case TokenType::EQUAL:
            oss << "EQUAL";
            break;
        case TokenType::STRING:
            oss << "STRING";
            break;
        case TokenType::ERROR:
            oss << "ERROR";
            break;
        }
        return oss;
    }

    std::ostream& operator<<(std::ostream& oss, const Token& t) {
        oss << "Token: type(" << t.type() << ") " << "value (" << t.value() << ") " << "source line (" << t.line() << ")";

        return oss;
    }

    bool operator==(const Token& t1, const Token& t2) {
        return (t1.type() == t2.type()) && (t1.line() == t2.line()) && (t1.value() == t2.value());
    }

    bool operator!=(const Token& t1, const Token& t2) {
        return !(t1 == t2);
    }


}

