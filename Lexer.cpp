// Necessary includes
#include "Lexer.hpp"
#include <iostream>

namespace simmips {

    TokenList tokenize(std::istream& input) {
        // List of tokens
        TokenList tokens;
        char character;
        std::string temp;
        std::string empty = "";
        bool enclosed = false;
        bool error = false;
        std::size_t line_num = 1;

        
        std::string line;

        // Loops through file until end of file is reached or error encountered
        while ((input.peek() != EOF) && !error) {
            line.clear();

            // Obtains 1 line of the text file
            while (input.get(character)) {
                if (character != '\n') {
                    line.push_back(character);
                }
                else {
                    break;
                }
            }

            // Loops through each character of line
            for (std::size_t i=0; i < line.length(); i++) {

                // Counter for counting the # of parenthesis
                int paren_counter = 0;
                if (!error) {
                    switch (line[i]) {
                    case ',':
                        if (!temp.empty()) {
                            tokens.emplace_back(STRING, line_num, temp);
                            temp.clear();
                        }
                        tokens.emplace_back(SEP, line_num, empty);
                        break;
                    case '"':
                        if (!temp.empty()) {
                            tokens.emplace_back(STRING, line_num, temp);
                            temp.clear();
                        }
                        tokens.emplace_back(STRING_DELIM, line_num, empty);
                        enclosed = true;
                        while (enclosed) {
                            if ((i + 1) < line.length()) { 
                                i++; 
                                if (line[i] == '"') {
                                    if (temp.empty()) {
                                        tokens.emplace_back(STRING, line_num, empty);
                                    }
                                    else {
                                        tokens.emplace_back(STRING, line_num, temp);
                                    }
                                    temp.clear();
                                    tokens.emplace_back(STRING_DELIM, line_num, empty);
                                    enclosed = false;
                                    break;
                                }
                                else {
                                    temp.push_back(line[i]);
                                }
                            }
                            else {
                                i++;
                                tokens.emplace_back(ERROR, line_num, ("Error: unmatched string delimeter on line " + std::to_string(line_num) + " due to new line"));
                                enclosed = false;
                                error = true;
                                break;
                            }
                        }
                        break;
                    case ')':
                        if (!temp.empty()) {
                            tokens.emplace_back(STRING, line_num, temp);
                            temp.clear();
                        }
                        tokens.emplace_back(ERROR, line_num, ("Error: unmatched close parenthesis on line " + std::to_string(line_num)));
                        error = true;
                        break;
                    case '(':
                        if (!temp.empty()) {
                            tokens.emplace_back(STRING, line_num, temp);
                            temp.clear();
                        }
                        tokens.emplace_back(OPEN_PAREN, line_num, empty);
                        enclosed = true;
                        paren_counter++;
                        while (enclosed) {
                            if ((i + 1) < line.length()) { i++; }
                            else {
                                tokens.emplace_back(ERROR, line_num, ("Error: unmatched parenthesis on line " + std::to_string(line_num) + " due to new line"));
                                temp.clear();
                                enclosed = false;
                                error = true;
                                break;
                            }
                            if (line[i] == ')') {
                                if (!temp.empty()) { tokens.emplace_back(STRING, line_num, temp); }
                                temp.clear();
                                paren_counter--;
                                tokens.emplace_back(CLOSE_PAREN, line_num, empty);
                                if (paren_counter == 0) { enclosed = false; }
                            }
                            else if (line[i] == '(') {
                                if (!temp.empty()) { tokens.emplace_back(STRING, line_num, temp); }
                                temp.clear();
                                paren_counter++;
                                tokens.emplace_back(OPEN_PAREN, line_num, empty);
                            }
                            else if (line[i] == '#') {
                                tokens.emplace_back(ERROR, line_num, ("Error: unmatched parenthesis on line " + std::to_string(line_num) + " due to comment"));
                                temp.clear();
                                enclosed = false;
                                error = true;
                            }
                            else if (line[i] == ' ') {
                                if (!temp.empty()) {
                                    tokens.emplace_back(STRING, line_num, temp);
                                    temp.clear();
                                }
                            }
                            else {
                                temp.push_back(line[i]);
                            }
                        }
                        break;
                    case '=':
                        if (!temp.empty()) {
                            tokens.emplace_back(STRING, line_num, temp);
                            temp.clear();
                        }
                        tokens.emplace_back(EQUAL, line_num, empty);
                        break;
                    case '#':
                        if (!temp.empty()) {
                            tokens.emplace_back(STRING, line_num, temp);
                            temp.clear();
                        }

                        enclosed = true;
                        while (enclosed) {
                            if ((i + 1) < line.length()) { i++; }
                            else{
                                temp.clear();
                                enclosed = false;
                                break;
                            }
                            temp.push_back(line[i]);
                        }
                        break;
                    case ' ':
                        if (!temp.empty()) {
                            tokens.emplace_back(STRING, line_num, temp);
                            temp.clear();
                        }
                        break;
                    case '\t':
                        if (!temp.empty()) {
                            tokens.emplace_back(STRING, line_num, temp);
                            temp.clear();
                        }
                        break;
                    case '\r':
                        if (!temp.empty()) {
                            tokens.emplace_back(STRING, line_num, temp);
                            temp.clear();
                        }
                        break;
                    default:
                        temp.push_back(line[i]);
                        break;
                    }
                }
            }
            // Forcefully continues loop if lexer error is found to halt lexing
            if (error) {
                continue;
            }
            else if (!temp.empty()) {
                tokens.emplace_back(STRING, line_num, temp);
                temp.clear();
            }

            if (!tokens.empty()) { // To prevent calling back() on an empty list
                if (tokens.back().type() != ERROR && tokens.back().type() != EOL) {
                    tokens.emplace_back(EOL, line_num, empty);
                }
            }
            line_num++;
            
        }
        return tokens;
    }

}