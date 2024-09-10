namespace simmips {
    enum DeclarationType {
        CONSTANT,
        LABEL,
        LAYOUT,
        UNLABELED
    };

    enum InstructionType {
        UNSORTED,
        LABEL_,
        OPERATION,
        LABEL_OP,
        INVALID
    };

    enum Registers {
        $zero, $at, $v0, $v1, $a0, $a1, $a2, $a3, $t0, $t1, $t2, $t3, $t4, $t5, $t6, $t7,
        $s0, $s1, $s2, $s3, $s4, $s5, $s6, $s7, $t8, $t9, $k0, $k1, $gp, $sp, $fp, $ra, $pc, $hi, $lo
    };

    // Done this way so that registers can be mapped to register map
    std::unordered_map<std::string, Registers> registerMap = {
        {"$zero", $zero},
        {"$at", $at},
        {"$v0", $v0},
        {"$v1", $v1},
        {"$a0", $a0},
        {"$a1", $a1},
        {"$a2", $a2},
        {"$a3", $a3},
        {"$t0", $t0},
        {"$t1", $t1},
        {"$t2", $t2},
        {"$t3", $t3},
        {"$t4", $t4},
        {"$t5", $t5},
        {"$t6", $t6},
        {"$t7", $t7},
        {"$s0", $s0},
        {"$s1", $s1},
        {"$s2", $s2},
        {"$s3", $s3},
        {"$s4", $s4},
        {"$s5", $s5},
        {"$s6", $s6},
        {"$s7", $s7},
        {"$t8", $t8},
        {"$t9", $t9},
        {"$k0", $k0},
        {"$k1", $k1},
        {"$gp", $gp},
        {"$sp", $sp},
        {"$fp", $fp},
        {"$ra", $ra},
        {"$pc", $pc},
        {"hi", $hi},
        {"lo", $lo}
    };

    enum OperationType {
        Type1, Type2, Type3, Type4, Type5, Type6, Type7, Type8, Type9, Type10
    };

    std::unordered_map<std::string, OperationType> operationList = {
        {"nop", Type1},
        {"j", Type2},
        {"mfhi", Type3},
        {"mflo", Type3},
        {"mthi", Type3},
        {"mtlo", Type3},
        {"lw", Type4},
        {"lh", Type4},
        {"lb", Type4},
        {"la", Type4},
        {"sw", Type4},
        {"sh", Type4},
        {"sb", Type4},
        {"li", Type5},
        {"move", Type6},
        {"mult", Type6},
        {"multu", Type6},
        {"abs", Type6},
        {"neg", Type6},
        {"negu", Type6},
        {"add", Type7},
        {"addu", Type7},
        {"sub", Type7},
        {"subu", Type7},
        {"mul", Type7},
        {"mulo", Type7},
        {"mulou", Type7},
        {"rem", Type7},
        {"remu", Type7},
        {"and", Type7},
        {"nor", Type7},
        {"or", Type7},
        {"xor", Type7},
        {"not", Type8},
        {"beq", Type9},
        {"bne", Type9},
        {"blt", Type9},
        {"ble", Type9},
        {"bgt", Type9},
        {"bge", Type9},
        {"div", Type10},
        {"divu", Type10},
    };

    // Function to get Registers enum value from string
    bool is_valid_reg(const std::string& regName) {
        try {
            if (regName.empty()) { return false; }
            std::string substr = regName.substr(1);
            auto it = registerMap.find(regName);
            if (it != registerMap.end()) {
                return true;
            }
            else {
                if (regName[0] == '$') {
                    int regNum = std::stoi(substr);
                    if (regNum >= 0 && regNum <= 31) {
                        return true;
                    }
                }
                return false;
            }
        }
        catch (const std::invalid_argument& e) {
            return false;
        }
        catch (const std::out_of_range& e) {
            return false;
        }
    }

    bool is_operation(const std::string& opcode) {
        auto it = operationList.find(opcode);
        if (it != operationList.end()) {
            return true;
        }
        else {
            return false;
        }
    }

    OperationType getOperationType(const std::unordered_map<std::string, OperationType>& operationList, const std::string& opcode) {
        return operationList.at(opcode);
    }

    bool is_valid_offset(const std::string& offset) {
        try {
            if (offset.empty()) { return false; }
            //std::cout << "Argument for stoll:" << offset << std::endl;
            long long int number = std::stoll(offset);
            return number >= std::numeric_limits<int16_t>::min() && number <= std::numeric_limits<int16_t>::max();
        }
        catch (const std::invalid_argument& e) { return false;}
        catch (const std::out_of_range& e) { return false; }
    }

    bool is_valid_int(const std::string& integer, const std::string& int_layout) {
        long long int value = std::stoll(integer);

        if (int_layout == ".word") {
            return value >= std::numeric_limits<int32_t>::min() && value <= std::numeric_limits<int32_t>::max();
        }
        else if (int_layout == ".half") {
            return value >= std::numeric_limits<int16_t>::min() && value <= std::numeric_limits<int16_t>::max();
        }
        else if (int_layout == ".byte") {
            return value >= std::numeric_limits<int8_t>::min() && value <= std::numeric_limits<int8_t>::max();
        }
        else if (int_layout == ".space") {
            return value >= 0 && value <= std::numeric_limits<int32_t>::max();
        }
        else { return false; }// Unknown layout
    }

    bool is_valid_uint(const std::string& integer, const std::string& int_layout) {
        unsigned long long int value = std::stoull(integer);

        if (int_layout == ".word") {
            return value <= std::numeric_limits<uint32_t>::max();
        }
        else if (int_layout == ".half") {
            return value <= std::numeric_limits<uint16_t>::max();
        }
        else if (int_layout == ".byte") {
            return value <= std::numeric_limits<uint8_t>::max();
        }
        else if (int_layout == ".space") {

            return value <= std::numeric_limits<uint32_t>::max();
        }
        else { return false; }// Unknown layout
    }

    bool is_integer(const std::string& str) {
        if (str.empty()) return false;
        //signed case
        if (str[0] == '+' || str[0] == '-') {
            for (size_t i = 1; i < str.size(); i++) {
                if (!std::isdigit(str[i])) return false;
            }
        }
        //unsigned case
        else {
            for (size_t i = 0; i < str.size(); i++) {
                if (!std::isdigit(str[i])) return false;
            }
        }
        return true;
    }

    bool sign_check(const std::string& str) {
        if (str.empty()) return false;

        if (str[0] == '+' || str[0] == '-') {
            return true;
        }
        else {
            return false;
        }
    }

    bool is_constant(const std::string& str) {
        if (str.empty()) return false;
        for (size_t i = 0; i < str.size(); i++) {
            if (i == 0) {
                if (!std::isalpha(str[i])) { return false; }
            }
            else if (i > 0) {
                if (!(std::isalpha(str[i]) || std::isdigit(str[i]))) return false;
            }
        }
        return true;
    }

    bool is_label(const std::string& str) {
        if (str.empty()) return false;
        for (size_t i = 0; i < str.size(); i++) {
            if (i == 0) {
                if (!std::isalpha(str[i])) return false;
            }
            else if (i > 0 && i < str.size() - 1) {
                if (!(std::isalpha(str[i]) || std::isdigit(str[i]))) return false;
            }
            else if (i == str.size() - 1) {
                if (str[i] != ':') return false;
            }
        }
        if (str[str.size() - 1] != ':') return false;
        return true;
    }

    bool is_int_layout(const std::string& str) {
        if (str.empty()) return false;
        if (str == ".word") {
            return true;
        }
        else if (str == ".half") {
            return true;
        }
        else if (str == ".byte") {
            return true;
        }
        else if (str == ".space") {
            return true;
        }
        else {
            return false;
        }
    }

    bool is_string_layout(const std::string& str) {
        if (str.empty()) return false;
        if (str == ".ascii") {
            return true;
        }
        else if (str == ".asciiz") {
            return true;
        }
        else {
            return false;
        }
    }

    bool is_layout(const std::string& str) {
        if (is_int_layout(str) || is_string_layout(str)) {
            return true;
        }
        return false;
    }

    DeclarationType data_identify(const std::string& str, DeclarationType data_identifier) {
        if (is_constant(str)) {
            data_identifier = CONSTANT;
        }
        else if (is_label(str)) {
            data_identifier = LABEL;
        }
        else if (is_layout(str)) {
            data_identifier = LAYOUT;
        }
        return data_identifier;
    }

    InstructionType text_identify(const std::string& current, const std::string& next, InstructionType instr_identifier) {
        if (is_label(current)) {
            if (is_operation(next)) {
                instr_identifier = LABEL_OP;
            }
            else {
                instr_identifier = LABEL_;
            }
        }
        else if (is_operation(current)) {
            instr_identifier = OPERATION;
        }
        else {
            instr_identifier = INVALID;
        }
        return instr_identifier;
    }

    bool is_valid_string(const std::string& str, const std::string& string_layout) {
        bool expect_null_terminator = (string_layout == ".asciiz");
        if (expect_null_terminator) {
            const char* cstr = str.c_str();
            for (size_t i = 0; cstr[i] != '\0'; i++) {
                if (!std::isprint(cstr[i])) return false;
            }
        }
        else {
            for (size_t i = 0; i < str.size(); i++) {
                if (!std::isprint(str[i])) return false;
            }
        }

        return true;
    }

    bool search_constants(std::vector < std::pair < std::string, std::string >>& constants, const std::string& search) {
        bool found = false;
        for (const auto& pair : constants) {
            if (pair.first == search) {
                found = true;
                break;
            }
        }
        return found;
    }

    std::string get_constant(std::vector < std::pair < std::string, std::string >>& constants, const std::string& search) {
        for (const auto& pair : constants) {
            if (pair.first == search) {
                return pair.second;
            }
        }
        return "";
    }


    bool vm_is_immediate(std::vector < std::pair < std::string, std::string >>& constants, const std::string& str) {
        if (is_integer(str)) {
            if (str[0] == '+' || str[0] == '-') {
                return is_valid_int(str, ".half");
            }
            else {
                return is_valid_uint(str, ".half");
            }
        }
        else if (search_constants(constants, str)) {
            return true;
        }
        else {
            return false;
        }
    }
}