#include "Parser.hpp"
#include "VirtualMachine.hpp"
#include "ParserFunctions.cpp"
#include <iostream>
#include <algorithm>
#include <limits>
#include <cstdint>
#include <vector>
#include <unordered_map>


namespace simmips {
    void ParseError::set(const std::string& message) {
        error_ = true;
        message_ = message;
    }

    ParseError::operator bool() const noexcept {
        return error_;
    }

    std::string ParseError::message() {
        return message_;
    }

    

    std::pair<ParseError, VirtualMachine> parseVM(const TokenList& tokens) {
        std::cout << std::endl;
        ParseError error;
        VirtualMachine vm;
        
        bool parsing_data_section = false;
        bool parsing_text_section = false;
        bool labeled = false;
        bool ignoreEOL = false;

        int token_count = 0;
        std::vector < std::pair < std::string, std::string >> constants; // List of constants with values (used in text) (will likely be a bool or int)

        DeclarationType data_identifier = UNLABELED;
        InstructionType text_identifier = UNSORTED;
        auto end = tokens.end();

        if (tokens.empty()) {
            error.set("Error:1: Parsing empty file");
        }
        for (auto check = tokens.begin(); check != end; check++) {
            const auto& checkToken = *check;
            if (checkToken.type() == ERROR) {
                error.set("Error:" + std::to_string(checkToken.line()) + ": Lexer error detected");
            }
        }
        for (auto it = tokens.begin(); it != end; it++) {
            if (error) break;
            const auto& currentToken = *it;
            token_count++;

            // Exclusive to 1st call
            if (!parsing_data_section && !parsing_text_section) {
                if (currentToken.value() == ".data" && token_count == 1) {
                    parsing_data_section = true;
                    auto nextIt = std::next(it);
                    const auto& nextToken = *nextIt;
                    if (nextToken.type() != EOL) {
                        error.set("Error:" + std::to_string(currentToken.line()) + ": Invalid data section specifier");
                    }
                    continue;
                }
                else if (currentToken.value() == ".text") {
                    parsing_data_section = false;
                    parsing_text_section = true;
                    if (labeled) {
                        continue;
                    }
                }
                else {
                    error.set("Error:" + std::to_string(currentToken.line()) + ": Section specifier not found");
                }
            }
            if (currentToken.value() == "main:" && token_count == 2) {
                it--; parsing_text_section = false;
            }
            if (!labeled) {
                if (currentToken.value() == ".text") {
                    parsing_data_section = false;
                    parsing_text_section = true;
                    // Iterative process to identify all labels and consecutive instruction line numbers (checked)
                    for (auto it_ = it; it_ != end; it_++) {
                        const auto& current = *it_;
                        if (is_label(current.value())) {
                            if (!error) {
                                auto nextIt = std::next(it_);
                                const auto& nextToken = *nextIt;
                                if (is_operation(nextToken.value())) {
                                    vm.setLabel(current.value(), nextToken.line());
                                    //std::cout << "Identified label '" << current.value() << "' at line " << current.line() << std::endl;
                                    //std::cout << "Set destination label(alt1) '" << nextToken.value() << "' at line " << nextToken.line() << std::endl;
                                }
                                else if (nextToken.type() == EOL) {
                                    for (auto it__ = it_; it__ != end; it__++) {
                                        const auto& temp = *it__;
                                        if (is_operation(temp.value())) {
                                            vm.setLabel(current.value(), temp.line());
                                            //std::cout << "Identified label '" << current.value() << "' at line " << current.line() << std::endl;
                                            //std::cout << "Set destination label(alt2) '" << current.value() << " at line " << temp.line() << std::endl;
                                            break;
                                        }
                                    }
                                }

                            }
                        }
                    }
                    labeled = true;
                    continue;
                }
            }
            if (currentToken.value() == "main:" && !parsing_text_section) {
                error.set("Error:" + std::to_string(currentToken.line()) + ": .text section specifier not found; main: occurs unexpectedly");
            }
            else if (currentToken.value() == "main:" && parsing_text_section) {
                ignoreEOL = true;
            }

            if (parsing_data_section) {
                if (currentToken.value() == ".text") {
                    parsing_data_section = false;
                    parsing_text_section = true;
                    continue;
                }
                switch (data_identifier) {
                case UNLABELED:
                    if (currentToken.type() == EOL) {
                        continue;
                    }
                    else if (currentToken.type() == STRING) {
                        data_identifier = data_identify(currentToken.value(), data_identifier);
                        if (data_identifier == UNLABELED) {
                            error.set("Error:" + std::to_string(currentToken.line()) + ": Invalid declaration type");
                            break;
                        }
                        else if (data_identifier == LABEL) {
                            auto nextIt = std::next(it);
                            const auto& nextToken = *nextIt;
                            DeclarationType nextID = UNLABELED;
                            data_identify(nextToken.value(), nextID);
                            if (nextID == LAYOUT) {
                                data_identifier = LAYOUT;
                            }
                        }
                        else if (data_identifier == CONSTANT) {
                            auto nextIt = std::next(it);
                            auto followingIt = std::next(nextIt);
                            const auto& valueToken = *followingIt;
                            constants.push_back(std::make_pair(currentToken.value(), valueToken.value()));
                        }
                    }
                    else {
                        error.set("Error:" + std::to_string(currentToken.line()) + ": Unexpected token type for unlabeled ID");
                    }
                    break;
                case CONSTANT:
                    if (currentToken.type() == EOL) {
                        data_identifier = UNLABELED;
                    }
                    else if (currentToken.type() == EQUAL) {
                        auto nextIt = std::next(it);
                        const auto& nextToken = *nextIt;
                        auto prevIt = std::prev(it);
                        const auto& prevToken = *prevIt;
                        if (prevToken.type() != STRING || nextToken.type() != STRING) {
                            error.set("Error:" + std::to_string(currentToken.line()) + ": Unexpected equal token for constant expansion"); break;
                        }
                    }
                    else if (currentToken.type() == STRING) {
                        auto nextIt = std::next(it);
                        const auto& nextToken = *nextIt;
                        auto prevIt = std::prev(it);
                        const auto& prevToken = *prevIt;
                        if (prevToken.type() != EQUAL) {
                            error.set("Error:" + std::to_string(currentToken.line()) + ": Equal token missing before string in declaration");
                            break;
                        }
                        if (nextToken.type() != EOL) {
                            error.set("Error:" + std::to_string(currentToken.line()) + ": EOL missing after constant expansion");
                            break;
                        }
                        if (!is_integer(currentToken.value())) {
                            error.set("Error:" + std::to_string(currentToken.line()) + ": Invalid value for integer");
                            break;
                        }
                    }
                    else {
                        error.set("Error:" + std::to_string(currentToken.line()) + ": Unexpected token type for constant ID");
                    }
                    break;
                case LABEL:
                    if (currentToken.type() == EOL) {
                        data_identifier = UNLABELED;
                    }
                    else if (is_layout(currentToken.value())) {
                        data_identifier = LAYOUT;
                    }
                    else {
                        error.set("Error:" + std::to_string(currentToken.line()) + ": Unexpected token type after label");
                    }
                    break;
                case LAYOUT:
                    auto prevIt = std::prev(it);
                    const auto& prevToken = *prevIt;
                    auto labelIt = std::prev(prevIt);
                    const auto& labelToken = *labelIt;
                    std::string label = labelToken.value();
                    if (!is_label(label)) {
                        label = "";
                    }
                    std::string layout = prevToken.value();
                    std::string value = "";
                    if (is_int_layout(prevToken.value())) {
                        if (currentToken.type() == EOL) {
                            error.set("Error:" + std::to_string(currentToken.line()) + ": EOL found before layout declaration");
                        }
                        std::string int_layout = prevToken.value();
                        while (data_identifier != UNLABELED) {
                            const auto& currentToken = *it;
                            if (currentToken.type() == STRING) {
                                bool in_memory = search_constants(constants, currentToken.value());
                                if (!is_integer(currentToken.value())) {
                                    if (!in_memory) {
                                        error.set("Error:" + std::to_string(currentToken.line()) + ": Value is not an integer");
                                    }
                                    else {
                                        value = get_constant(constants, currentToken.value());
                                    }
                                }
                                else {
                                    bool sign = sign_check(currentToken.value());
                                    if (sign) {
                                        if (!is_valid_int(currentToken.value(), int_layout)) {
                                            error.set("Error:" + std::to_string(currentToken.line()) + ": Integer values exceed maximal bounds for signed int");
                                        }
                                    }
                                    else if (!sign) {
                                        if (!is_valid_uint(currentToken.value(), int_layout)) {
                                            error.set("Error:" + std::to_string(currentToken.line()) + ": Integer values exceed maximal bounds for unsigned int");
                                        }
                                    }
                                    value = currentToken.value();
                                }

                            }
                            else if (currentToken.type() == SEP) {
                                auto nextIt = std::next(it);
                                const auto& nextToken = *nextIt;
                                if (!(prevToken.type() == STRING) || !(nextToken.type() == STRING)) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": Invalid usage of separators in integer layout");
                                }
                            }
                            else if (currentToken.type() == EOL) {
                                data_identifier = UNLABELED;
                                break;
                            }
                            else {
                                error.set("Error:" + std::to_string(currentToken.line()) + ": Unexpected token type for int_layout");
                            }

                            if (std::distance(it, end) != 1) {
                                it++;
                            }
                        }
                        if (!error) {
                            vm.setDataMemory(label, layout, value);
                        }
                    }
                    else if (is_string_layout(prevToken.value())) {
                        std::string str_layout = prevToken.value();
                        int counter = 1;
                        if (currentToken.type() == EOL) {
                            error.set("Error:" + std::to_string(currentToken.line()) + ": String layout value not declared");
                        }
                        else if (std::distance(it, end) <= 1) {
                            error.set("Error:" + std::to_string(currentToken.line()) + ": EOF occured unexpectedly");
                        }
                        while (data_identifier == LAYOUT) {
                            const auto& currentToken = *it;
                            if (currentToken.type() == STRING_DELIM && (counter == 1 || counter == 3)) {
                                counter++;
                            }
                            else if (currentToken.type() == STRING && counter == 2) {
                                counter++;
                                if (!is_valid_string(currentToken.value(), prevToken.value())) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": Invalid string for str_layout");
                                }
                                else {
                                    value = currentToken.value();
                                }

                            }
                            else if (currentToken.type() == EOL && counter == 4) {
                                data_identifier = UNLABELED;
                                break;
                            }
                            else {
                                error.set("Error:" + std::to_string(currentToken.line()) + ": Unexpected token type for str_layout");
                                data_identifier = UNLABELED;
                            }
                            if (std::distance(it, end) != 1) {
                                it++;
                            }
                        }
                        if (!error) {
                            vm.setDataMemory(label, layout, value);
                        }
                    }
                    else if (!is_layout(prevToken.value())) {
                        error.set("Error:" + std::to_string(currentToken.line()) + ": Invalid layout definition");
                    }
                    else {
                        error.set("Error:" + std::to_string(currentToken.line()) + ": Unexpected token type for layout ID");
                    }
                    break;
                }
            }

            else if (parsing_text_section) {
                if (currentToken.value() == ".data") {
                    parsing_data_section = true;
                    parsing_text_section = false;
                    continue;
                }
                switch (text_identifier) {
                case UNSORTED:
                    if (currentToken.type() == EOL) {
                        continue;
                    }
                    else if (currentToken.type() == STRING) {
                        auto nextIt = std::next(it);
                        const auto& nextToken = *nextIt;
                        text_identifier = text_identify(currentToken.value(), nextToken.value(), text_identifier);
                        if (text_identifier == INVALID) {
                            error.set("Error:" + std::to_string(currentToken.line()) + ": Invalid instruction declaration");
                            break;
                        }
                        else if (text_identifier == OPERATION) {
                            it--;
                            break;
                        }
                        else if (text_identifier == LABEL_OP) {
                            text_identifier = OPERATION;
                            break;
                        }
                        else if (text_identifier == LABEL_) {
                            break;
                        }
                    }
                    else {
                        error.set("Error:" + std::to_string(currentToken.line()) + ": Unexpected token type for .text");
                    }
                    break;
                case LABEL_:
                    if (currentToken.type() == EOL) {
                        text_identifier = UNSORTED;
                    }
                    else if (currentToken.type() == STRING) {
                        break;
                    }
                    else {
                        error.set("Error:" + std::to_string(currentToken.line()) + ": Unexpected token type after label");
                    }
                    break;
                case LABEL_OP:
                    break;
                case INVALID:
                    error.set("Error:" + std::to_string(currentToken.line()) + ": Invalid instruction");
                    break;
                case OPERATION:
                    // When this case is active, it should start on the instruction token each time
                    if (currentToken.type() == EOL) {
                        text_identifier = UNSORTED;
                    }
                    else if (is_operation(currentToken.value())) {
                        int counter = 0;
                        OperationType optype = getOperationType(operationList, currentToken.value());
                        std::vector <std::string> arguments;
                        //Store arguments
                        for (auto it_ = it; it != end; it_++) {
                            const auto& temp = *it_;
                            if (temp.type() == STRING) {
                                if (search_constants(constants, temp.value())) {
                                    arguments.push_back(get_constant(constants, temp.value()));
                                }
                                else {
                                    arguments.push_back(temp.value());
                                }

                            }
                            else if (temp.type() == OPEN_PAREN) {
                                arguments.push_back("(");
                            }
                            else if (temp.type() == CLOSE_PAREN) {
                                arguments.push_back(")");
                            }
                            else if (temp.type() == EOL) {
                                arguments.push_back(std::to_string(temp.line()));
                                break;
                            }
                        }
                        while (currentToken.type() != EOL) {
                            const auto& currentToken = *it;
                            if (currentToken.type() == EOL) {
                                text_identifier = UNSORTED;
                                break;
                            }

                            // Counter keeps track of token # starting with -> counter = 1 means currentToken = instruction
                            counter++;
                            // While condition ensures next token is always viable
                            auto nextIt = std::next(it);
                            const auto& nextToken = *nextIt;
                            switch (optype) {
                            case Type1:
                                //'nop'
                                if (counter == 1 && nextToken.type() != EOL) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": EOL not found after nop");
                                }
                                break;
                            case Type2:
                                //'j'
                                if (counter == 1) {
                                    if (vm.searchLabels(nextToken.value())){
                                        break;
                                    }
                                    else {
                                        error.set("Error:" + std::to_string(currentToken.line()) + ": Invalid label declaration (Type 2)");
                                    }
                                }
                                //<label>
                                else if (counter == 2 && nextToken.type() != EOL) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": EOL not found after jump destination");
                                }
                                break;
                            case Type3:
                                //'inst'
                                if (counter == 1 && !is_valid_reg(nextToken.value())) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": Invalid register value (Type 3)");
                                }
                                //<register>
                                else if (counter == 2 && nextToken.type() != EOL) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": EOL not found after register (Type 3)");
                                }
                                break;
                            case Type4:
                                //std::cout << "Current Token: " << currentToken.value() << "; Type: " << currentToken.type() << std::endl;
                                
                                //'inst'
                                if (counter == 1 && !is_valid_reg(nextToken.value())) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": Invalid register value (Type 4)");
                                }
                                //<register>
                                else if (counter == 2 && nextToken.type() != SEP) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": Separator not found after register (Type 4)");
                                }
                                //SEP
                                else if (counter == 3) {
                                    if (vm.searchLabels(nextToken.value())) {
                                        //std::cout << "This is a label" << std::endl;
                                        break;
                                    }
                                    else if (is_valid_reg(nextToken.value())) {
                                        break;
                                    }
                                    else if (nextToken.type() == OPEN_PAREN) {
                                        auto followingIt = std::next(nextIt);
                                        const auto& followingToken = *followingIt;

                                        if (is_valid_reg(followingToken.value())) {
                                            
                                        }
                                        else if (vm_is_immediate(constants, followingToken.value())) {
                                            
                                        }
                                        else if (vm.searchLabels(followingToken.value())) {
                                            
                                        }
                                        else {
                                            error.set("Error:" + std::to_string(currentToken.line()) + ": Invalid register value for memref (Type 4)");
                                        }

                                        if (error) { break; }
                                        //'('
                                        it++;
                                        auto followingIt2 = std::next(followingIt);
                                        const auto& followingToken2 = *followingIt2;
                                        if (followingToken2.type() != CLOSE_PAREN) {
                                            error.set("Error:" + std::to_string(currentToken.line()) + ": Incomplete memory reference (Type 4)");
                                        }
                                        // If error, it++ not executed
                                        if (error) { break; }
                                        //<register>
                                        it++;
                                    }
                                    else if (nextToken.type() == STRING && is_valid_offset(nextToken.value())) {
                                        auto followingIt = std::next(nextIt);
                                        const auto& followingToken = *followingIt;
                                        if (followingToken.type() == EOL) {
                                            break;
                                        }
                                        if (followingToken.type() != OPEN_PAREN) {
                                            error.set("Error:" + std::to_string(currentToken.line()) + ": Open paren not found after offset (Type 4)");
                                        }
                                        if (error) { break; }
                                        //[offset]
                                        it++;
                                        auto followingIt1 = std::next(followingIt);
                                        const auto& followingToken1 = *followingIt1;
                                        if (is_valid_reg(followingToken1.value())) {
                                            
                                        }
                                        else if (vm_is_immediate(constants, followingToken1.value())) {
                                            
                                        }
                                        else if (vm.searchLabels(followingToken1.value())) {
                                            
                                        }
                                        else {
                                            error.set("Error:" + std::to_string(currentToken.line()) + ": Invalid register value for memref (Type 4)");
                                        }
                                        if (error) { break; }
                                        //'('
                                        it++;
                                        auto followingIt2 = std::next(followingIt1);
                                        const auto& followingToken2 = *followingIt2;
                                        if (followingToken2.type() != CLOSE_PAREN) {
                                            error.set("Error:" + std::to_string(currentToken.line()) + ": Incomplete memory reference (Type 4)");
                                        }
                                        if (error) { break; }
                                        //<register>
                                        it++;
                                    }
                                    else if (vm_is_immediate(constants, nextToken.value())) {
                                        break;
                                    }
                                    else {
                                        error.set("Error:" + std::to_string(currentToken.line()) + ": Invalid instruction declaration (Type 4)");
                                    }
                                    break;
                                }
                                //<memref> (aka last character before EOL)
                                else if (counter == 4 && nextToken.type() != EOL) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": EOL not found after memref (Type 4)");
                                }
                                break;
                            case Type5:
                                //'inst'
                                if (counter == 1 && !is_valid_reg(nextToken.value())) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": Invalid register value (Type 5)");
                                }
                                //<register>
                                else if (counter == 2 && nextToken.type() != SEP) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": Separator not found after register (Type 5)");
                                }
                                //SEP
                                else if (counter == 3 && !vm_is_immediate(constants, nextToken.value())) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": Immediate value is invalid (Type 5)");
                                }
                                //<immediate>
                                else if (counter == 4 && nextToken.type() != EOL) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": EOL not found after immediate (Type 5)");
                                }
                                break;
                            case Type6:
                                //'inst'
                                if (counter == 1 && !is_valid_reg(nextToken.value())) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": Invalid register value (Type 6)");
                                }
                                //<register>
                                else if (counter == 2 && nextToken.type() != SEP) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": Separator not found after register (Type 6)");
                                }
                                //SEP
                                else if (counter == 3 && !is_valid_reg(nextToken.value())) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": Invalid register value (Type 6)");
                                }
                                //<register>
                                else if (counter == 4 && nextToken.type() != EOL) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": EOL not found after register (Type 6)");
                                }
                                break;
                            case Type7:
                                //'inst'
                                if (counter == 1 && !is_valid_reg(nextToken.value())) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": Invalid register value (Type 7)");
                                }
                                //<register>
                                else if (counter == 2 && nextToken.type() != SEP) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": Separator not found after register (Type 7)");
                                }
                                //SEP
                                else if (counter == 3 && !is_valid_reg(nextToken.value())) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": Invalid register value (Type 7)");
                                }
                                //<register>
                                else if (counter == 4 && nextToken.type() != SEP) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": Separator not found after register (Type 7)");
                                }
                                //SEP
                                else if (counter == 5) {
                                    if (is_valid_reg(nextToken.value())) {
                                        break;
                                    }
                                    else if (vm_is_immediate(constants, nextToken.value())) {
                                        break;
                                    }
                                    else {
                                        error.set("Error:" + std::to_string(currentToken.line()) + ": Invalid source declaration (Type 7)");
                                    }
                                }
                                //<source>
                                else if (counter == 6 && nextToken.type() != EOL) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": EOL not found after register (Type 7)");
                                }
                                break;
                            case Type8:
                                //'inst'
                                if (counter == 1 && !is_valid_reg(nextToken.value())) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": Invalid register value (Type 8)");
                                }
                                //<register>
                                else if (counter == 2 && nextToken.type() != SEP) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": Separator not found after register (Type 8)");
                                }
                                //SEP
                                else if (counter == 3) {
                                    if (is_valid_reg(nextToken.value())) {
                                        break;
                                    }
                                    else if (vm_is_immediate(constants, nextToken.value())) {
                                        break;
                                    }
                                    else {
                                        error.set("Error:" + std::to_string(currentToken.line()) + ": Invalid source declaration (Type 8)");
                                    }
                                }
                                //<source>
                                else if (counter == 4 && nextToken.type() != EOL) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": EOL not found after register (Type 8)");
                                }
                                break;
                            case Type9:
                                //'inst'
                                if (counter == 1 && !is_valid_reg(nextToken.value())) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": Invalid register value (Type 9)");
                                }
                                //<register>
                                else if (counter == 2 && nextToken.type() != SEP) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": Separator not found after register (Type 9)");
                                }
                                //SEP
                                else if (counter == 3) {
                                    if (is_valid_reg(nextToken.value())) {
                                        break;
                                    }
                                    else if (vm_is_immediate(constants, nextToken.value())) {
                                        break;
                                    }
                                    else {
                                        error.set("Error:" + std::to_string(currentToken.line()) + ": Invalid source declaration (Type 9)");
                                    }
                                }
                                //<source>
                                else if (counter == 4 && nextToken.type() != SEP) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": EOL not found after register (Type 9)");
                                }
                                //SEP
                                else if (counter == 5) {
                                    if (vm.searchLabels(nextToken.value())) {
                                        break;
                                    }
                                    else {
                                        error.set("Error:" + std::to_string(currentToken.line()) + ": Invalid label declaration (Type 9)");
                                    }
                                }
                                //<label>
                                else if (counter == 6 && nextToken.type() != EOL) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": EOL not found after register (Type 9)");
                                }
                                break;
                            case Type10:
                                //'inst'
                                if (counter == 1 && !is_valid_reg(nextToken.value())) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": Invalid register value (Type 10)");
                                }
                                //<register>
                                else if (counter == 2 && nextToken.type() != SEP) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": Separator not found after register (Type 10)");
                                }
                                //SEP
                                else if (counter == 3 && !is_valid_reg(nextToken.value())) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": Invalid register value (Type 10)");
                                }
                                //<register>
                                else if (counter == 4) {
                                    if (nextToken.type() == SEP) {
                                        break;
                                    }
                                    else if (nextToken.type() == EOL) {
                                        break;
                                    }
                                    else {
                                        error.set("Error:" + std::to_string(currentToken.line()) + ": Invalid instruction declaration (Type 10)");
                                    }
                                }
                                //SEP (only if not EOL)
                                else if (counter == 5) {
                                    if (is_valid_reg(nextToken.value())) {
                                        break;
                                    }
                                    else if (vm_is_immediate(constants, nextToken.value())) {
                                        break;
                                    }
                                    else {
                                        error.set("Error:" + std::to_string(currentToken.line()) + ": Invalid source declaration (Type 10)");
                                    }
                                }
                                //<source>
                                else if (counter == 6 && nextToken.type() != EOL) {
                                    error.set("Error:" + std::to_string(currentToken.line()) + ": EOL not found after register (Type 10)");
                                }
                                break;
                            }
                            if (error) {
                                break;
                            }
                            it++;
                        }
                        if (!error) {
                            vm.addInstruction(arguments); 
                        }
                        // After checkop, current token should be EOL
                    }
                    else {
                        error.set("Error:" + std::to_string(currentToken.line()) + ": Unexpected token type after label for operation");
                    }
                    break;
                }

            }
            if (error) {
                vm.setError();
            }
            if (ignoreEOL) {
                ignoreEOL = false;
            }
        }
        // Post processing
        vm.adjustLabels();
        std::pair<ParseError, VirtualMachine> ParserOutput = std::make_pair(error, vm);
        return ParserOutput;
    }

    ParseError parse(const TokenList& tokens) {
        std::pair<ParseError, VirtualMachine> output = parseVM(tokens);
        return output.first;
    }

}
