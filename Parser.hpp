#ifndef SIMMIPS_PARSER_HPP
#define SIMMIPS_PARSER_HPP
#include "Lexer.hpp"
#include "Token.hpp"
#include "VirtualMachine.hpp"
#include <string>
#include <algorithm>
/// Class to hold the error code from parsing.
/// If operator bool() returns true, an error occurred during parsing
/// and message() returns a description of sthe error.
namespace simmips {
    class ParseError {
    public:
        ParseError() = default;
        /// set parse error with optional constant message
        void set(const std::string& message);

        /// convert to bool returns true if error is set, else false
        operator bool() const noexcept;

        /// returns string that contains the error message if set
        /// returns empty string otherwise
        std::string message();

    private:
        bool error_ = false;
        std::string message_;
    };

    // Given a TokenList attempt to parse as a MIPS assembly file.
    // If the returned ParseError object is set, then an error occurred
    std::pair<ParseError, VirtualMachine> parseVM(const TokenList& tokens);


    ParseError parse(const TokenList& tokens);
}

#endif
