#include "VirtualMachine.hpp"
//#include "VirtualMachineInterface.hpp"
#include <algorithm>
#include <iostream>


namespace simmips {
    std::unordered_map<std::string, RegisterKind> register_list = {
        {"$zero", Register00},
        {"$at", Register01},
        {"$v0", Register02},
        {"$v1", Register03},
        {"$a0", Register04},
        {"$a1", Register05},
        {"$a2", Register06},
        {"$a3", Register07},
        {"$t0", Register08},
        {"$t1", Register09},
        {"$t2", Register10},
        {"$t3", Register11},
        {"$t4", Register12},
        {"$t5", Register13},
        {"$t6", Register14},
        {"$t7", Register15},
        {"$s0", Register16},
        {"$s1", Register17},
        {"$s2", Register18},
        {"$s3", Register19},
        {"$s4", Register20},
        {"$s5", Register21},
        {"$s6", Register22},
        {"$s7", Register23},
        {"$t8", Register24},
        {"$t9", Register25},
        {"$k0", Register26},
        {"$k1", Register27},
        {"$gp", Register28},
        {"$sp", Register29},
        {"$fp", Register30},
        {"$ra", Register31},
        {"$pc", RegisterPC},
        {"$hi", RegisterHi},
        {"$lo", RegisterLo}
     };


    VirtualMachine::VirtualMachine() :
        memory_(1024), // Initialize memory with 1024 bytes, all set to 0
        currentSourceLine_(0),
        error_(false),
        errorMessage_(""),
        top_of_mem(0),
        start_(true),
        branch_(0){}

    std::size_t VirtualMachine::getMemorySize() const noexcept {
        return memory_.memory.size();
    }

    std::size_t VirtualMachine::getInstructionSize() const noexcept {
        return instructions_.size();
    }

    uint8_t VirtualMachine::getMemoryByte(uint32_t address) const{
        if (address >= getMemorySize()) {
            throw std::range_error("Memory address out of range");
        }
        return memory_.memory[address];
    }
    
    int8_t VirtualMachine::getMemoryByte_s(uint32_t address) const {
        return static_cast<int8_t>(memory_.memory[address]);
    }


    uint16_t VirtualMachine::getMemoryHalf(uint32_t address) const {
        if (address + 1 >= memory_.memory.size()) {
            throw std::range_error("Memory address out of range");
        }
        uint16_t value = 0;
        value |= memory_.memory[address];
        value |= (memory_.memory[address + 1]) << 8;
        return value;
    }
    
    int16_t VirtualMachine::getMemoryHalf_s(uint32_t address) const
    {
        uint16_t value = 0;
        value |= static_cast<int16_t>((memory_.memory[address]));
        value |= static_cast<int16_t>((memory_.memory[address + 1])) << 8;
        return value;
    }

    uint32_t VirtualMachine::getMemoryWord(uint32_t address) const {
        if (address + 3 >= memory_.memory.size()) {
            throw std::range_error("Memory address out of range");
        }
        uint32_t value = 0;
        value |= memory_.memory[address];
        value |= (memory_.memory[address + 1]) << 8;
        value |= (memory_.memory[address + 2]) << 16;
        value |= (memory_.memory[address + 3]) << 24;
        return value;
    }

    int32_t VirtualMachine::getMemoryWord_s(uint32_t address) const {
        int32_t value = 0;
        value |= static_cast<int32_t>(memory_.memory[address]);
        value |= static_cast<int32_t>(memory_.memory[address + 1]) << 8;
        value |= static_cast<int32_t>(memory_.memory[address + 2]) << 16;
        value |= static_cast<int32_t>(memory_.memory[address + 3]) << 24;
        return value;
    }


    uint32_t VirtualMachine::getRegister(RegisterKind reg) const noexcept{
        return registers_.registers[std::underlying_type<RegisterKind>::type(reg)];
    }

    int32_t VirtualMachine::getRegister_s(RegisterKind reg) const
    {
        return static_cast<int32_t>(registers_.registers[std::underlying_type<RegisterKind>::type(reg)]);
    }

    std::string VirtualMachine::getRegisterAlias(RegisterKind reg) {
        switch (reg) {
        case Register00: return "$zero";
        case Register01: return "$at";
        case Register02: return "$v0";
        case Register03: return "$v1";
        case Register04: return "$a0";
        case Register05: return "$a1";
        case Register06: return "$a2";
        case Register07: return "$a3";
        case Register08: return "$t0";
        case Register09: return "$t1";
        case Register10: return "$t2";
        case Register11: return "$t3";
        case Register12: return "$t4";
        case Register13: return "$t5";
        case Register14: return "$t6";
        case Register15: return "$t7";
        case Register16: return "$s0";
        case Register17: return "$s1";
        case Register18: return "$s2";
        case Register19: return "$s3";
        case Register20: return "$s4";
        case Register21: return "$s5";
        case Register22: return "$s6";
        case Register23: return "$s7";
        case Register24: return "$t8";
        case Register25: return "$t9";
        case Register26: return "$k0";
        case Register27: return "$k1";
        case Register28: return "$gp";
        case Register29: return "$sp";
        case Register30: return "$fp";
        case Register31: return "$ra";
        case RegisterPC: return "$pc";
        case RegisterHi: return "$hi";
        case RegisterLo: return "$lo";
        default: return "";
        }
    }

    uint32_t VirtualMachine::getCurrentSourceLine() const noexcept {
        return currentSourceLine_;
    }

    void VirtualMachine::setRegister(RegisterKind reg, uint32_t value) {
        registers_.registers[std::underlying_type<RegisterKind>::type(reg)] = value;
    }

    void VirtualMachine::setMemoryByte(uint32_t address, uint8_t value) {
        memory_.memory[address] = value;
        top_of_mem++;
    }

    void VirtualMachine::setMemoryHalf(uint32_t address, uint16_t value) {
        if (address + 1 < memory_.memory.size()) {
            memory_.memory[address] = static_cast<uint8_t>(value & 0xFF);
            memory_.memory[address + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
        }
        else {
            throw std::range_error("Memory address out of bounds");
        }
        top_of_mem += 2;
    }

    void VirtualMachine::setMemoryWord(uint32_t address, uint32_t value) {
        if (address + 3 < memory_.memory.size()) {
            // Store the bytes in little endian format
            memory_.memory[address] = static_cast<uint8_t>(value & 0xFF);
            memory_.memory[address + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
            memory_.memory[address + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
            memory_.memory[address + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
        }
        else {
            throw std::range_error("Memory address out of bounds");
        }
        top_of_mem += 4;
    }

    void VirtualMachine::setDataMemory(std::string label, std::string layout, std::string value) {
        if (layout == ".word") {
            uint32_t intValue = static_cast<uint32_t>(std::stoul(value));
            setLabel(label, top_of_mem);
            setMemoryWord(top_of_mem, intValue);
        }
        else if (layout == ".half") {
            uint16_t intValue = static_cast<uint16_t>(std::stoul(value));
            setLabel(label, top_of_mem);
            setMemoryHalf(top_of_mem, intValue);
        }
        else if (layout == ".byte") {
            uint8_t byteValue = static_cast<uint8_t>(std::stoi(value));
            setLabel(label, top_of_mem);
            setMemoryByte(top_of_mem, byteValue);
        }
        else if (layout == ".space") {
            if (label != "") {
                setLabel(label, top_of_mem);
            }
            size_t spaceSize = std::stoul(value);
            top_of_mem += spaceSize;
        }
        else if (layout == ".ascii" || layout == ".asciiz") {
            setLabel(label, top_of_mem);
            for (char c : value) {
                setMemoryByte(top_of_mem, static_cast<uint8_t>(c));
            }
            if (layout == ".asciiz") {
                setMemoryByte(top_of_mem, 0);
            }
        }
        return;
    }

    void VirtualMachine::setLabel(const std::string& label, uint32_t address) {
        labels_[label] = VariantType(address);
    }

    void VirtualMachine::setLabel(const std::string& label, std::size_t instructionIndex) {
        labels_[label] = VariantType(instructionIndex);
    }

    RegisterKind VirtualMachine::identifyRegister(const std::string& reg)
    {
        std::string substr = reg.substr(1);
        auto it = register_list.find(reg);
        if (it != register_list.end()) {
            return it->second;
        }
        else {
            if (reg[0] == '$') {
                int regNum = std::stoi(substr);
                if (regNum >= 0 && regNum <= static_cast<int>(RegisterKind::Register31)) {
                    return static_cast<RegisterKind>(regNum);
                }
            }
        }
        return RegisterKind(RegisterUnknown);
    }

    bool VirtualMachine::searchLabels(const std::string& label)
    {
        std::string key = label;
        key += ":";
        auto it = labels_.find(key);
        if (it != labels_.end()) {
            return true;
        }
        else {
            return false;
        }
    }

    VariantType VirtualMachine::getLabel(const std::string& label) const {
        std::string key = label;
        key += ":";
        auto it = labels_.find(key);
        if (it != labels_.end()) {
            return it->second;
        }
        else {
            return VariantType();
        }
    }


    void VirtualMachine::addInstruction(std::vector<std::string> ins) {
        instructions_.push_back(ins);
    }

    std::vector<std::string> VirtualMachine::getInstruction(std::size_t instructionIndex) {
        std::vector<std::string> instruction;
        instruction = instructions_[instructionIndex];
        return instruction;
    }

    void VirtualMachine::setError() {
        error_ = true;
    }

    bool VirtualMachine::isError() const noexcept {
        return error_;
    }

    std::string VirtualMachine::getError() const noexcept {
        if (!isError()) {
            return "";
        }
        return errorMessage_;
    }

    // tester function
    void VirtualMachine::printLabels() {
        std::cout << "Memory content:" << std::endl;
        for (const auto& pair : labels_) {
            const std::string& label = pair.first;
            const VariantType& variant = pair.second;
            std::cout << "Label: " << label << ", Type: ";
            switch (variant.type) {
            case DataType::Null:
                std::cout << "Null";
                break;
            case DataType::ByteAddress:
                std::cout << "ByteAddress, Value: " << variant.data.byteAddress;
                break;
            case DataType::InstructionIndex:
                std::cout << "InstructionIndex, Value: " << variant.data.instructionIndex;
                break;
            }
            std::cout << std::endl;
        }
    }

    // tester function
    void VirtualMachine::printInstructions() {
        for (const auto& vec : instructions_) {
            for (const auto& str : vec) {
                std::cout << str << " ";
            }
            std::cout << std::endl;
        }
    }

    void VirtualMachine::printRegisters(){
        for (size_t i = 0; i < registers_.registers.size(); ++i) {
            std::string index_key = "$" + std::to_string(i);
            std::cout << "Register '$'" << identifyRegister(index_key);
            if (i < 10) {
                std::cout << " "; // Adjust spacing for alignment
            }
            std::cout << ": " << registers_.registers[i] << std::endl;
        }
    }

    void VirtualMachine::adjustLabels() {
        for (const auto& pair : labels_) {
            if (pair.second.type == DataType::InstructionIndex) {
                std::string source_line = std::to_string(pair.second.data.instructionIndex);
                std::size_t i = 0;
                for (const auto& vec : instructions_) {
                    if (vec.back() == source_line) {
                        setLabel(pair.first, i);
                    }
                    i++;
                }
            }
        }
        return;
    }

    bool VirtualMachine::executeInstruction(std::vector<std::string> instruction) {
        std::size_t numArgs = instruction.size()-1;
        std::string opcode = instruction[0];
        if (opcode == "j") {
            VariantType label = getLabel(instruction[1]);
            std::size_t instructionIndex = label.data.instructionIndex;
            setRegister(RegisterPC, static_cast<uint32_t>(instructionIndex));
            currentSourceLine_ = instructionIndex;
            return true;
        }

        else if (opcode == "nop") {
            return false;
        }

        else if (opcode == "mfhi") {
            // Execute move_from_hi operation
            std::string rd = instruction[1];
            uint32_t regValue = getRegister(RegisterHi);
            RegisterKind reg = identifyRegister(rd);
            setRegister(reg, regValue);
        }
        else if (opcode == "mflo") {
            // Execute move_from_lo operation
            std::string rd = instruction[1];
            uint32_t regValue = getRegister(RegisterLo);
            RegisterKind reg = identifyRegister(rd);
            setRegister(reg, regValue);
        }
        else if (opcode == "mthi") {
            // Execute move_to_hi operation
            std::string rs = instruction[1];
            RegisterKind reg = identifyRegister(rs);
            uint32_t regValue = getRegister(reg);
            setRegister(RegisterHi, regValue);
        }
        else if (opcode == "mtlo") {
            // Execute move_to_lo operation
            std::string rs = instruction[1];
            RegisterKind reg = identifyRegister(rs);
            uint32_t regValue = getRegister(reg);
            setRegister(RegisterLo, regValue);
        }

        else if (opcode == "lw") {
            // Execute load_word operation
            std::string rt = instruction[1];
            RegisterKind reg = identifyRegister(rt);
            // Direct addressing
            if (numArgs == 3) {
                // Label
                if (searchLabels(instruction[2])) {
                    VariantType label = getLabel(instruction[2]);
                    uint32_t memAddr = label.data.byteAddress;
                    int32_t regValue = getMemoryWord(memAddr);
                    setRegister(reg, regValue);
                }
                // Register
                else if (instruction[2].at(0) == '$') {
                    RegisterKind rs = identifyRegister(instruction[2]);
                    uint32_t regAddr = getRegister(rs);
                    int32_t regValue = getMemoryWord(regAddr);
                    setRegister(reg, regValue);
                }
                // Immediate address
                else {
                    uint32_t absAddr = static_cast<uint32_t>(stoul(instruction[2]));
                    int32_t regValue = getMemoryWord(absAddr);
                    setRegister(reg, regValue);
                }
            }
            // Indirect addressing (w/o imm)
            else if (numArgs == 5) {
                // Label
                if (searchLabels(instruction[3])) {
                    VariantType label = getLabel(instruction[3]);
                    uint32_t memAddr = label.data.byteAddress;
                    int32_t regValue = getMemoryWord(memAddr);
                    setRegister(reg, regValue);
                }
                // Register
                else if (instruction[3].at(0) == '$') {
                    RegisterKind rs = identifyRegister(instruction[1]);
                    uint32_t regAddr = getRegister(rs);
                    int32_t regValue = getMemoryWord(regAddr);
                    setRegister(reg, regValue);
                }
                // Immediate
                else {
                    uint32_t memAddr = static_cast<uint32_t>(stoul(instruction[3]));
                    int32_t regValue = getMemoryWord(memAddr);
                    setRegister(reg, regValue);
                }
            }
            // Indirect addressing (w/ imm)
            else if (numArgs == 6) {
                // Label
                uint32_t offset = static_cast<uint32_t>(stoul(instruction[2]));
                if (searchLabels(instruction[4])) {
                    VariantType label = getLabel(instruction[4]);
                    uint32_t memAddr = label.data.byteAddress;
                    int32_t regValue = getMemoryWord(memAddr+offset);
                    setRegister(reg, regValue);
                }
                // Register
                else if (instruction[4].at(0) == '$') {
                    RegisterKind rs = identifyRegister(instruction[4]);
                    uint32_t regAddr = getRegister(rs);
                    int32_t regValue = getMemoryWord(regAddr+offset);
                    setRegister(reg, regValue);
                }
                // Immediate
                else {
                    uint32_t memAddr = static_cast<uint32_t>(stoul(instruction[4]));
                    int32_t regValue = getMemoryWord(memAddr+offset);
                    setRegister(reg, regValue);
                }

            }
        }
        else if (opcode == "lh") {
            // Execute load_half operation
            std::string rt = instruction[1];
            RegisterKind reg = identifyRegister(rt);
            // Direct addressing
            if (numArgs == 3) {
                // Label
                if (searchLabels(instruction[2])) {
                    VariantType label = getLabel(instruction[2]);
                    uint32_t memAddr = label.data.byteAddress;
                    int32_t regValue = getMemoryHalf(memAddr);
                    setRegister(reg, regValue);
                }
                // Register
                else if (instruction[2].at(0) == '$') {
                    RegisterKind rs = identifyRegister(instruction[2]);
                    uint32_t regAddr = getRegister(rs);
                    int32_t regValue = getMemoryHalf(regAddr);
                    setRegister(reg, regValue);
                }
                // Immediate address
                else {
                    uint32_t absAddr = static_cast<uint32_t>(stoul(instruction[2]));
                    int32_t regValue = getMemoryHalf(absAddr);
                    setRegister(reg, regValue);
                }
            }
            // Indirect addressing (w/o imm)
            else if (numArgs == 5) {
                // Label
                if (searchLabels(instruction[3])) {
                    VariantType label = getLabel(instruction[3]);
                    uint32_t memAddr = label.data.byteAddress;
                    int32_t regValue = getMemoryHalf(memAddr);
                    setRegister(reg, regValue);
                }
                // Register
                else if (instruction[3].at(0) == '$') {
                    RegisterKind rs = identifyRegister(instruction[1]);
                    uint32_t regAddr = getRegister(rs);
                    int32_t regValue = getMemoryHalf(regAddr);
                    setRegister(reg, regValue);
                }
                // Immediate
                else {
                    uint32_t memAddr = static_cast<uint32_t>(stoul(instruction[3]));
                    int32_t regValue = getMemoryHalf(memAddr);
                    setRegister(reg, regValue);
                }
            }
            // Indirect addressing (w/ imm)
            else if (numArgs == 6) {
                // Label
                uint32_t offset = static_cast<uint32_t>(stoul(instruction[2]));
                if (searchLabels(instruction[4])) {
                    VariantType label = getLabel(instruction[4]);
                    uint32_t memAddr = label.data.byteAddress;
                    int32_t regValue = getMemoryHalf(memAddr + offset);
                    setRegister(reg, regValue);
                }
                // Register
                else if (instruction[4].at(0) == '$') {
                    RegisterKind rs = identifyRegister(instruction[4]);
                    uint32_t regAddr = getRegister(rs);
                    int32_t regValue = getMemoryHalf(regAddr + offset);
                    setRegister(reg, regValue);
                }
                // Immediate
                else {
                    uint32_t memAddr = stoul(instruction[4]);
                    int32_t regValue = getMemoryHalf(memAddr + offset);
                    setRegister(reg, regValue);
                }

            }
        }
        else if (opcode == "lb") {
            // Execute load_byte operation
            std::string rt = instruction[1];
            RegisterKind reg = identifyRegister(rt);
            // Direct addressing
            if (numArgs == 3) {
                // Label
                if (searchLabels(instruction[2])) {
                    VariantType label = getLabel(instruction[2]);
                    uint32_t memAddr = label.data.byteAddress;
                    int32_t regValue = getMemoryByte_s(memAddr);
                    setRegister(reg, regValue);
                }
                // Register
                else if (instruction[2].at(0) == '$') {
                    RegisterKind rs = identifyRegister(instruction[2]);
                    uint32_t regAddr = getRegister(rs);
                    int32_t regValue = getMemoryByte_s(regAddr);
                    setRegister(reg, regValue);
                }
                // Immediate address
                else {
                    uint32_t absAddr = static_cast<uint32_t>(stoul(instruction[2]));
                    int32_t regValue = getMemoryByte_s(absAddr);
                    setRegister(reg, regValue);
                }
            }
            // Indirect addressing (w/o imm)
            else if (numArgs == 5) {
                // Label
                if (searchLabels(instruction[3])) {
                    VariantType label = getLabel(instruction[3]);
                    uint32_t memAddr = label.data.byteAddress;
                    int32_t regValue = getMemoryByte_s(memAddr);
                    setRegister(reg, regValue);
                }
                // Register
                else if (instruction[3].at(0) == '$') {
                    RegisterKind rs = identifyRegister(instruction[1]);
                    uint32_t regAddr = getRegister(rs);
                    int32_t regValue = getMemoryByte_s(regAddr);
                    setRegister(reg, regValue);
                }
                // Immediate
                else {
                    uint32_t memAddr = static_cast<uint32_t>(stoul(instruction[3]));
                    int32_t regValue = getMemoryByte_s(memAddr);
                    setRegister(reg, regValue);
                }
            }
            // Indirect addressing (w/ imm)
            else if (numArgs == 6) {
                // Label
                uint32_t offset = static_cast<uint32_t>(stoul(instruction[2]));
                if (searchLabels(instruction[4])) {
                    VariantType label = getLabel(instruction[4]);
                    uint32_t memAddr = label.data.byteAddress;
                    int32_t regValue = getMemoryByte_s(memAddr + offset);
                    setRegister(reg, regValue);
                }
                // Register
                else if (instruction[4].at(0) == '$') {
                    RegisterKind rs = identifyRegister(instruction[4]);
                    uint32_t regAddr = getRegister(rs);
                    int32_t regValue = getMemoryByte_s(regAddr + offset);
                    setRegister(reg, regValue);
                }
                // Immediate
                else {
                    uint32_t memAddr = stoul(instruction[4]);
                    int32_t regValue = getMemoryByte_s(memAddr + offset);
                    setRegister(reg, regValue);
                }

            }
        }
        else if (opcode == "sw") {
            // Execute store_word operation
            std::string rs = instruction[1];
            RegisterKind reg = identifyRegister(rs);
            uint32_t regValue = getRegister(reg);
            // Direct addressing
            if (numArgs == 3) {
                // Label
                if (searchLabels(instruction[2])) {
                    VariantType label = getLabel(instruction[2]);
                    uint32_t memAddr = label.data.byteAddress;
                    setMemoryWord(memAddr, regValue);
                }
                // Register
                else if (instruction[2].at(0) == '$') {
                    RegisterKind rs = identifyRegister(instruction[2]);
                    uint32_t regAddr = getRegister(rs);
                    setMemoryWord(regAddr, regValue);
                }
                // Immediate address
                else {
                    uint32_t absAddr = static_cast<uint32_t>(stoul(instruction[2]));
                    setMemoryWord(absAddr, regValue);
                }
            }
            // Indirect addressing (w/o imm)
            else if (numArgs == 5) {
                // Label
                if (searchLabels(instruction[3])) {
                    VariantType label = getLabel(instruction[3]);
                    uint32_t memAddr = label.data.byteAddress;
                    setMemoryWord(memAddr, regValue);
                }
                // Register
                else if (instruction[3].at(0) == '$') {
                    RegisterKind rs = identifyRegister(instruction[3]);
                    uint32_t regAddr = getRegister(rs);
                    setMemoryWord(regAddr, regValue);
                }
                // Immediate address
                else {
                    uint32_t absAddr = static_cast<uint32_t>(stoul(instruction[3]));
                    setMemoryWord(absAddr, regValue);
                }
            }
            // Indirect addressing (w/ imm)
            else if (numArgs == 6) {
                // Label
                uint32_t offset = static_cast<uint32_t>(stoul(instruction[2]));
                if (searchLabels(instruction[4])) {
                    VariantType label = getLabel(instruction[4]);
                    uint32_t memAddr = label.data.byteAddress;
                    setMemoryWord(memAddr+offset, regValue);
                }
                // Register
                else if (instruction[4].at(0) == '$') {
                    RegisterKind rs = identifyRegister(instruction[4]);
                    uint32_t regAddr = getRegister(rs);
                    setMemoryWord(regAddr+offset, regValue);
                }
                // Immediate address
                else {
                    uint32_t absAddr = static_cast<uint32_t>(stoul(instruction[4]));
                    setMemoryWord(absAddr+offset, regValue);
                }
            }
        }
        else if (opcode == "sh") {
            // Execute store_half operation
            std::string rs = instruction[1];
            RegisterKind reg = identifyRegister(rs);
            uint32_t regValue = getRegister(reg);
            // Direct addressing
            if (numArgs == 3) {
                // Label
                if (searchLabels(instruction[2])) {
                    VariantType label = getLabel(instruction[2]);
                    uint32_t memAddr = label.data.byteAddress;
                    setMemoryHalf(memAddr, regValue);
                }
                // Register
                else if (instruction[2].at(0) == '$') {
                    RegisterKind rs = identifyRegister(instruction[2]);
                    uint32_t regAddr = getRegister(rs);
                    setMemoryHalf(regAddr, regValue);
                }
                // Immediate address
                else {
                    uint32_t absAddr = static_cast<uint32_t>(stoul(instruction[2]));
                    setMemoryHalf(absAddr, regValue);
                }
            }
            // Indirect addressing (w/o imm)
            else if (numArgs == 5) {
                // Label
                if (searchLabels(instruction[3])) {
                    VariantType label = getLabel(instruction[3]);
                    uint32_t memAddr = label.data.byteAddress;
                    setMemoryHalf(memAddr, regValue);
                }
                // Register
                else if (instruction[3].at(0) == '$') {
                    RegisterKind rs = identifyRegister(instruction[3]);
                    uint32_t regAddr = getRegister(rs);
                    setMemoryHalf(regAddr, regValue);
                }
                // Immediate address
                else {
                    uint32_t absAddr = static_cast<uint32_t>(stoul(instruction[3]));
                    setMemoryHalf(absAddr, regValue);
                }
            }
            // Indirect addressing (w/ imm)
            else if (numArgs == 6) {
                // Label
                uint32_t offset = static_cast<uint32_t>(stoul(instruction[2]));
                if (searchLabels(instruction[4])) {
                    VariantType label = getLabel(instruction[4]);
                    uint32_t memAddr = label.data.byteAddress;
                    setMemoryHalf(memAddr + offset, regValue);
                }
                // Register
                else if (instruction[4].at(0) == '$') {
                    RegisterKind rs = identifyRegister(instruction[4]);
                    uint32_t regAddr = getRegister(rs);
                    setMemoryHalf(regAddr + offset, regValue);
                }
                // Immediate address
                else {
                    uint32_t absAddr = static_cast<uint32_t>(stoul(instruction[4]));
                    setMemoryHalf(absAddr + offset, regValue);
                }
            }
        }
        else if (opcode == "sb") {
            // Execute store_byte operation
            std::string rs = instruction[1];
            RegisterKind reg = identifyRegister(rs);
            int32_t regValue = getRegister_s(reg);
            // Direct addressing
            if (numArgs == 3) {
                // Label
                if (searchLabels(instruction[2])) {
                    VariantType label = getLabel(instruction[2]);
                    uint32_t memAddr = label.data.byteAddress;
                    setMemoryByte(memAddr, regValue);
                }
                // Register
                else if (instruction[2].at(0) == '$') {
                    RegisterKind rs = identifyRegister(instruction[2]);
                    uint32_t regAddr = getRegister(rs);
                    setMemoryByte(regAddr, regValue);
                }
                // Immediate address
                else {
                    uint32_t absAddr = static_cast<uint32_t>(stoul(instruction[2]));
                    setMemoryByte(absAddr, regValue);
                }
            }
            // Indirect addressing (w/o imm)
            else if (numArgs == 5) {
                // Label
                if (searchLabels(instruction[3])) {
                    VariantType label = getLabel(instruction[3]);
                    uint32_t memAddr = label.data.byteAddress;
                    setMemoryByte(memAddr, regValue);
                }
                // Register
                else if (instruction[3].at(0) == '$') {
                    RegisterKind rs = identifyRegister(instruction[3]);
                    uint32_t regAddr = getRegister(rs);
                    setMemoryByte(regAddr, regValue);
                }
                // Immediate address
                else {
                    uint32_t absAddr = static_cast<uint32_t>(stoul(instruction[3]));
                    setMemoryByte(absAddr, regValue);
                }
            }
            // Indirect addressing (w/ imm)
            else if (numArgs == 6) {
                // Label
                uint32_t offset = static_cast<uint32_t>(stoul(instruction[2]));
                if (searchLabels(instruction[4])) {
                    VariantType label = getLabel(instruction[4]);
                    uint32_t memAddr = label.data.byteAddress;
                    setMemoryByte(memAddr + offset, regValue);
                }
                // Register
                else if (instruction[4].at(0) == '$') {
                    RegisterKind rs = identifyRegister(instruction[4]);
                    uint32_t regAddr = getRegister(rs);
                    setMemoryByte(regAddr + offset, regValue);
                }
                // Immediate address
                else {
                    uint32_t absAddr = static_cast<uint32_t>(stoul(instruction[4]));
                    setMemoryByte(absAddr + offset, regValue);
                }
            }
        }

        else if (opcode == "la") {
            // Execute load_add operation
            std::string rt = instruction[1];
            RegisterKind reg = identifyRegister(rt);
            // Label
            if (searchLabels(instruction[2])) {
                VariantType label = getLabel(instruction[2]);
                uint32_t memAddr = label.data.byteAddress;
                setRegister(reg, memAddr);
            }
            // Register
            else if (instruction[2].at(0) == '$') {
                RegisterKind rs = identifyRegister(instruction[2]);
                uint32_t regAddr = getRegister(rs);
                setRegister(reg, regAddr);
            }
            // Immediate address
            else {
                if (stoi(instruction[2]) < 0) {
                    setError();
                    return false;
                }
                uint32_t absAddr = static_cast<uint32_t>(stoul(instruction[2]));
                setRegister(reg, absAddr);
            }
        }

        else if (opcode == "li") {
            // Execute load_imm operation
            std::string rt = instruction[1];
            RegisterKind reg = identifyRegister(rt);
            uint32_t imm = static_cast<uint32_t>(stoi(instruction[2]));

            setRegister(reg, imm);
        }

        else if (opcode == "move") {
            // Execute move operation
            std::string rt = instruction[1];
            std::string rs = instruction[2];
            RegisterKind regDest = identifyRegister(rt);
            RegisterKind regSource = identifyRegister(rs);
            uint32_t regValue = getRegister(regSource);
            setRegister(regDest, regValue);
        }
        else if (opcode == "abs") {
            // Execute abs operation
            std::string rt = instruction[1];
            std::string rs = instruction[2];
            RegisterKind regDest = identifyRegister(rt);
            RegisterKind regSource = identifyRegister(rs);
            uint32_t regValue = getRegister(regSource);
            regValue = std::abs(static_cast<int32_t>(regValue));
            setRegister(regDest, regValue);
        }
        else if (opcode == "neg") {
            // Execute neg_s operation
            std::string rt = instruction[1];
            std::string rs = instruction[2];
            RegisterKind regDest = identifyRegister(rt);
            RegisterKind regSource = identifyRegister(rs);
            uint32_t regValue = getRegister(regSource);
            regValue = static_cast<int32_t>(regValue)*-1;
            setRegister(regDest, regValue);
        }
        else if (opcode == "negu") {
            // Execute neg_u operation
            std::string rt = instruction[1];
            std::string rs = instruction[2];
            RegisterKind regDest = identifyRegister(rt);
            RegisterKind regSource = identifyRegister(rs);
            uint32_t regValue = getRegister(regSource);
            regValue *= -1;
            setRegister(regDest, regValue);
        }

        else if (opcode == "mult") {
            // Execute mult_s operation
            std::string rt = instruction[1];
            std::string rs = instruction[2];
            RegisterKind regSource1 = identifyRegister(rt);
            RegisterKind regSource2 = identifyRegister(rs);
            int32_t regSource1Value = static_cast<int32_t>(getRegister(regSource1));
            int32_t regSource2Value = static_cast<int32_t>(getRegister(regSource2));

            long long int regValue = static_cast<int64_t>(regSource1Value) * static_cast<int64_t>(regSource2Value);
            int64_t highValue = regValue >> 32;
            int32_t lowValue = regValue & 0xFFFFFFFF;
           
            std::memcpy(&registers_.registers[RegisterHi], &highValue, 4);
            setRegister(RegisterLo, static_cast<uint32_t>(lowValue));
        }
        else if (opcode == "multu") {
            // Execute mult_u operation
            std::string rt = instruction[1];
            std::string rs = instruction[2];
            RegisterKind regSource1 = identifyRegister(rt);
            RegisterKind regSource2 = identifyRegister(rs);
            uint64_t regSource1Value = getRegister(regSource1);
            uint64_t regSource2Value = getRegister(regSource2);
            uint64_t regValue = regSource1Value * regSource2Value;
            uint32_t highValue = (regValue >> 32) & 0xFFFFFFFF;
            uint32_t lowValue = regValue & 0xFFFFFFFF;
            setRegister(RegisterHi, highValue);
            setRegister(RegisterLo, lowValue);
        }

        else if (opcode == "add") {
            // Execute add_s operation
            std::string rd = instruction[1];
            RegisterKind regDest = identifyRegister(rd);
            std::string rs = instruction[2];
            RegisterKind regSource = identifyRegister(rs);
            int32_t regSourceVal = static_cast<int32_t>(getRegister(regSource));

            std::string rt = instruction[3];
            int32_t regSourceVal2;
            if (rt.at(0) == '$') {
                RegisterKind regSource2 = identifyRegister(rt);
                regSourceVal2 = static_cast<int32_t>(getRegister(regSource2));
            }
            else {
                regSourceVal2 = static_cast<int32_t>(stoi(instruction[3]));
            }
            
            int32_t regValue = regSourceVal + regSourceVal2;
            if ((regValue - regSourceVal) != regSourceVal2) {
                setError();
                errorMessage_ = "add; overflow detected";
                return false;
            }
            else {
                setRegister(regDest, static_cast<uint32_t>(regValue));
            }

        }
        else if (opcode == "addu") {
            // Execute add_u operation
            std::string rd = instruction[1];
            RegisterKind regDest = identifyRegister(rd);
            std::string rs = instruction[2];
            RegisterKind regSource = identifyRegister(rs);
            uint32_t regSourceVal = getRegister(regSource);

            std::string rt = instruction[3];
            uint32_t regSourceVal2;
            if (rt.at(0) == '$') {
                RegisterKind regSource2 = identifyRegister(rt);
                regSourceVal2 = getRegister(regSource2);
            }
            else {
                regSourceVal2 = static_cast<uint32_t>(stoul(instruction[3]));
            }
            uint32_t regValue = regSourceVal + regSourceVal2;
            setRegister(regDest, regValue);
        }
        else if (opcode == "sub") {
            // Execute sub_s operation
            std::string rd = instruction[1];
            RegisterKind regDest = identifyRegister(rd);
            std::string rs = instruction[2];
            RegisterKind regSource = identifyRegister(rs);
            int32_t regSourceVal = static_cast<int32_t>(getRegister(regSource));

            std::string rt = instruction[3];
            int32_t regSourceVal2;
            if (rt.at(0) == '$') {
                RegisterKind regSource2 = identifyRegister(rt);
                regSourceVal2 = static_cast<int32_t>(getRegister(regSource2));
            }
            else {
                regSourceVal2 = static_cast<int32_t>(stoi(instruction[3]));
            }

            int32_t regValue = regSourceVal - regSourceVal2;
            if ((regValue + regSourceVal2) != regSourceVal) {
                setError();
                errorMessage_ = "sub; overflow detected";
                return false;
            }
            else {
                setRegister(regDest, static_cast<int32_t>(regValue));
            }
        }
        else if (opcode == "subu") {
            // Execute sub_u operation
            std::string rd = instruction[1];
            RegisterKind regDest = identifyRegister(rd);
            std::string rs = instruction[2];
            RegisterKind regSource = identifyRegister(rs);
            uint32_t regSourceVal = getRegister(regSource);

            std::string rt = instruction[3];
            uint32_t regSourceVal2;
            if (rt.at(0) == '$') {
                RegisterKind regSource2 = identifyRegister(rt);
                regSourceVal2 = getRegister(regSource2);
            }
            else {
                regSourceVal2 = static_cast<uint32_t>(stoul(instruction[3]));
            }
            uint32_t regValue = regSourceVal - regSourceVal2;
            setRegister(regDest, regValue);
        }
        else if (opcode == "mul") {
            // Execute mul operation
            std::string rd = instruction[1];
            RegisterKind regDest = identifyRegister(rd);
            std::string rs = instruction[2];
            RegisterKind regSource = identifyRegister(rs);
            int32_t regSourceVal = getRegister_s(regSource);

            std::string rt = instruction[3];
            int32_t regSourceVal2;
            if (rt.at(0) == '$') {
                RegisterKind regSource2 = identifyRegister(rt);
                regSourceVal2 = getRegister_s(regSource2);
            }
            else {
                regSourceVal2 = static_cast<int32_t>(stoi(instruction[3]));
            }
            int32_t regValue = regSourceVal * regSourceVal2;
            setRegister(regDest, static_cast<int32_t>(regValue));
        }
        else if (opcode == "mulo") {
            // Execute mul_lo operation
            std::string rd = instruction[1];
            RegisterKind regDest = identifyRegister(rd);
            std::string rs = instruction[2];
            RegisterKind regSource = identifyRegister(rs);
            int32_t regSourceVal = getRegister_s(regSource);

            std::string rt = instruction[3];
            int32_t regSourceVal2;
            if (rt.at(0) == '$') {
                RegisterKind regSource2 = identifyRegister(rt);
                regSourceVal2 = getRegister_s(regSource2);
            }
            else {
                regSourceVal2 = static_cast<int32_t>(stoi(instruction[3]));
            }
            int32_t regValue = regSourceVal * regSourceVal2;
            if (regSourceVal2 != 0) {
                if ((regValue / regSourceVal2) != regSourceVal) {
                    setError();
                    errorMessage_ = "mulo; overflow detected";
                    return false;
                }
                else {
                    setRegister(regDest, static_cast<int32_t>(regValue));
                }
            }
            else {
                setRegister(regDest, static_cast<int32_t>(regValue));
            }

        }
        else if (opcode == "mulou") {
            // Execute mul_lou operation
            std::string rd = instruction[1];
            RegisterKind regDest = identifyRegister(rd);
            std::string rs = instruction[2];
            RegisterKind regSource = identifyRegister(rs);
            uint32_t regSourceVal = getRegister(regSource);

            std::string rt = instruction[3];
            uint32_t regSourceVal2;
            if (rt.at(0) == '$') {
                RegisterKind regSource2 = identifyRegister(rt);
                regSourceVal2 = getRegister(regSource2);
            }
            else {
                regSourceVal2 = static_cast<uint32_t>(stoi(instruction[3]));
            }
            uint32_t regValue = regSourceVal * regSourceVal2;
            if (regSourceVal2 != 0) {
                if ((regValue / regSourceVal2) != regSourceVal) {
                    setError();
                    errorMessage_ = "mulo; overflow detected";
                    return false;
                }
                else {
                    setRegister(regDest, regValue);
                }
            }
            else {
                setRegister(regDest, regValue);
            }
        }
        else if (opcode == "rem") {
            // Execute rem_s operation
            std::string rd = instruction[1];
            RegisterKind regDest = identifyRegister(rd);
            std::string rs = instruction[2];
            RegisterKind regSource = identifyRegister(rs);
            int32_t regSourceVal = getRegister_s(regSource);

            std::string rt = instruction[3];
            int32_t regSourceVal2;
            if (rt.at(0) == '$') {
                RegisterKind regSource2 = identifyRegister(rt);
                regSourceVal2 = getRegister_s(regSource2);
            }
            else {
                regSourceVal2 = static_cast<int32_t>(stoi(instruction[3]));
            }
            int32_t regValue = regSourceVal % regSourceVal2;
            setRegister(regDest, static_cast<int32_t>(regValue));
        }
        else if (opcode == "remu") {
            // Execute rem_u operation
            std::string rd = instruction[1];
            RegisterKind regDest = identifyRegister(rd);
            std::string rs = instruction[2];
            RegisterKind regSource = identifyRegister(rs);
            uint32_t regSourceVal = getRegister(regSource);

            std::string rt = instruction[3];
            uint32_t regSourceVal2;
            if (rt.at(0) == '$') {
                RegisterKind regSource2 = identifyRegister(rt);
                regSourceVal2 = getRegister(regSource2);
            }
            else {
                regSourceVal2 = static_cast<uint32_t>(stoi(instruction[3]));
            }
            if (regSourceVal2 == 0) {
                setError();
                errorMessage_ = "div; divison by zero is undefined";
                return false;
            }
            uint32_t regValue = regSourceVal % regSourceVal2;
            setRegister(regDest, regValue);
        }
        else if (opcode == "and") {
            // Execute and operation
            std::string rd = instruction[1];
            RegisterKind regDest = identifyRegister(rd);
            std::string rs = instruction[2];
            RegisterKind regSource = identifyRegister(rs);
            uint32_t regSourceVal = getRegister(regSource);

            std::string rt = instruction[3];
            uint32_t regSourceVal2;
            if (rt.at(0) == '$') {
                RegisterKind regSource2 = identifyRegister(rt);
                regSourceVal2 = getRegister(regSource2);
            }
            else {
                regSourceVal2 = static_cast<uint32_t>(stoi(instruction[3]));
            }

            uint32_t regValue = regSourceVal & regSourceVal2;
            setRegister(regDest, regValue);

        }
        else if (opcode == "nor") {
            // Execute nor operation
            std::string rd = instruction[1];
            RegisterKind regDest = identifyRegister(rd);
            std::string rs = instruction[2];
            RegisterKind regSource = identifyRegister(rs);
            uint32_t regSourceVal = getRegister(regSource);

            std::string rt = instruction[3];
            uint32_t regSourceVal2;
            if (rt.at(0) == '$') {
                RegisterKind regSource2 = identifyRegister(rt);
                regSourceVal2 = getRegister(regSource2);
            }
            else {
                regSourceVal2 = static_cast<uint32_t>(stoi(instruction[3]));
            }

            uint32_t regValue = ~(regSourceVal | regSourceVal2);
            setRegister(regDest, regValue);
        }
        else if (opcode == "or") {
            // Execute or operation
            std::string rd = instruction[1];
            RegisterKind regDest = identifyRegister(rd);
            std::string rs = instruction[2];
            RegisterKind regSource = identifyRegister(rs);
            uint32_t regSourceVal = getRegister(regSource);

            std::string rt = instruction[3];
            uint32_t regSourceVal2;
            if (rt.at(0) == '$') {
                RegisterKind regSource2 = identifyRegister(rt);
                regSourceVal2 = getRegister(regSource2);
            }
            else {
                regSourceVal2 = static_cast<uint32_t>(stoi(instruction[3]));
            }

            uint32_t regValue = regSourceVal | regSourceVal2;
            setRegister(regDest, regValue);
        }
        else if (opcode == "xor") {
            // Execute xor operation
            std::string rd = instruction[1];
            RegisterKind regDest = identifyRegister(rd);
            std::string rs = instruction[2];
            RegisterKind regSource = identifyRegister(rs);
            uint32_t regSourceVal = getRegister(regSource);

            std::string rt = instruction[3];
            uint32_t regSourceVal2;
            if (rt.at(0) == '$') {
                RegisterKind regSource2 = identifyRegister(rt);
                regSourceVal2 = getRegister(regSource2);
            }
            else {
                regSourceVal2 = static_cast<uint32_t>(stoi(instruction[3]));
            }

            uint32_t regValue = regSourceVal ^ regSourceVal2;
            setRegister(regDest, regValue);
        }

        else if (opcode == "not") {
            // Execute not operation
            std::string rd = instruction[1];
            RegisterKind regDest = identifyRegister(rd);

            std::string rs = instruction[2];
            uint32_t regSourceVal;
            if (rs.at(0) == '$') {
                RegisterKind regSource = identifyRegister(rs);
                regSourceVal = getRegister(regSource);
            }
            else {
                regSourceVal = stoul(instruction[2]);
            }
            uint32_t regValue = ~regSourceVal;
            setRegister(regDest, regValue);
        }

        else if (opcode == "beq") {
            // Execute branch_eq operation
            std::string rs = instruction[1];
            RegisterKind regSource = identifyRegister(rs);
            int32_t regSourceVal = getRegister_s(regSource);
            std::string rt = instruction[2];
            RegisterKind source = identifyRegister(rt);
            int32_t sourceVal = getRegister_s(source);
            VariantType label = getLabel(instruction[3]);
            std::size_t instructionIndex = label.data.instructionIndex;

            if (regSourceVal == sourceVal) {
                setRegister(RegisterPC, static_cast<uint32_t>(instructionIndex));
                currentSourceLine_ = instructionIndex;
                return true;
            }
        }
        else if (opcode == "bne") {
            // Execute branch_ne operation
            std::string rs = instruction[1];
            RegisterKind regSource = identifyRegister(rs);
            int32_t regSourceVal = getRegister_s(regSource);
            std::string rt = instruction[2];
            RegisterKind source = identifyRegister(rt);
            int32_t sourceVal = getRegister_s(source);
            VariantType label = getLabel(instruction[3]);
            std::size_t instructionIndex = label.data.instructionIndex;

            if (regSourceVal != sourceVal) {
                setRegister(RegisterPC, static_cast<uint32_t>(instructionIndex));
                currentSourceLine_ = instructionIndex;
                return true;
            }
        }
        else if (opcode == "blt") {
            // Execute branch_lt operation
            std::string rs = instruction[1];
            RegisterKind regSource = identifyRegister(rs);
            int32_t regSourceVal = getRegister_s(regSource);
            std::string rt = instruction[2];
            RegisterKind source = identifyRegister(rt);
            int32_t sourceVal = getRegister_s(source);
            VariantType label = getLabel(instruction[3]);
            std::size_t instructionIndex = label.data.instructionIndex;

            if (regSourceVal < sourceVal) {
                setRegister(RegisterPC, static_cast<uint32_t>(instructionIndex));
                currentSourceLine_ = instructionIndex;
                return true;
            }
        }
        else if (opcode == "ble") {
            // Execute branch_le operation
            std::string rs = instruction[1];
            RegisterKind regSource = identifyRegister(rs);
            int32_t regSourceVal = getRegister_s(regSource);
            std::string rt = instruction[2];
            RegisterKind source = identifyRegister(rt);
            int32_t sourceVal = getRegister_s(source);
            VariantType label = getLabel(instruction[3]);
            std::size_t instructionIndex = label.data.instructionIndex;

            if (regSourceVal <= sourceVal) {
                setRegister(RegisterPC, static_cast<uint32_t>(instructionIndex));
                currentSourceLine_ = instructionIndex;
                return true;
            }
        }
        else if (opcode == "bgt") {
            // Execute branch_gt operation
            std::string rs = instruction[1];
            RegisterKind regSource = identifyRegister(rs);
            int32_t regSourceVal = getRegister_s(regSource);
            std::string rt = instruction[2];
            RegisterKind source = identifyRegister(rt);
            int32_t sourceVal = getRegister_s(source);
            VariantType label = getLabel(instruction[3]);
            std::size_t instructionIndex = label.data.instructionIndex;

            if (regSourceVal > sourceVal) {
                setRegister(RegisterPC, static_cast<uint32_t>(instructionIndex));
                currentSourceLine_ = instructionIndex;
                return true;
            }
        }
        else if (opcode == "bge") {
            // Execute branch_ge operation
            std::string rs = instruction[1];
            RegisterKind regSource = identifyRegister(rs);
            int32_t regSourceVal = getRegister_s(regSource);
            std::string rt = instruction[2];
            RegisterKind source = identifyRegister(rt);
            int32_t sourceVal = getRegister_s(source);
            VariantType label = getLabel(instruction[3]);
            std::size_t instructionIndex = label.data.instructionIndex;

            if (regSourceVal >= sourceVal) {
                setRegister(RegisterPC, static_cast<uint32_t>(instructionIndex));
                currentSourceLine_ = instructionIndex;
                return true;
            }
        }

        else if (opcode == "div") {
            // Execute div3_s or div2_s operation based on arguments
            if (numArgs == 3) {
                std::string rs = instruction[1];
                RegisterKind regSource1 = identifyRegister(rs);
                std::string rt = instruction[2];
                RegisterKind regSource2 = identifyRegister(rt);
                int32_t regSourceVal1 = getRegister_s(regSource1);
                int32_t regSourceVal2 = getRegister_s(regSource2);
                if (regSourceVal2 == 0) {
                    //setError();
                    //errorMessage_ = "div; divison by zero is undefined";
                    return false;
                }
                int32_t lowValue = regSourceVal1 / regSourceVal2;
                int32_t hiValue = regSourceVal1 % regSourceVal2;
                setRegister(RegisterLo, static_cast<uint32_t>(lowValue));
                setRegister(RegisterHi, static_cast<uint32_t>(hiValue));
            }
            else if (numArgs == 4) {
                std::string rd = instruction[1];
                RegisterKind regDest = identifyRegister(rd);
                std::string rs = instruction[2];
                RegisterKind regSource = identifyRegister(rs);
                int32_t regSourceVal = getRegister_s(regSource);

                std::string rt = instruction[3];
                int32_t sourceVal;
                if (rt.at(0) == '$') {
                    RegisterKind regSource2 = identifyRegister(rt);
                    sourceVal = getRegister_s(regSource2);
                }
                else {
                    sourceVal = static_cast<int32_t>(stoi(instruction[3]));
                }
                if (sourceVal == 0) {
                    //setError();
                    //errorMessage_ = "div; divison by zero is undefined";
                    return false;
                }
                int32_t regValue = regSourceVal / sourceVal;
                setRegister(regDest, static_cast<uint32_t>(regValue));
            }
        }
        else if (opcode == "divu") {
            // Execute div3_u or div2_u operation based on arguments
            if (numArgs == 3) {
                std::string rs = instruction[1];
                RegisterKind regSource1 = identifyRegister(rs);
                std::string rt = instruction[2];
                RegisterKind regSource2 = identifyRegister(rt);
                uint32_t regSourceVal1 = getRegister(regSource1);
                uint32_t regSourceVal2 = getRegister(regSource2);
                if (regSourceVal2 == 0) {
                    //setError();
                    //errorMessage_ = "div; divison by zero is undefined";
                    return false;
                }
                uint32_t lowValue = regSourceVal1 / regSourceVal2;
                uint32_t hiValue = regSourceVal1 % regSourceVal2;
                setRegister(RegisterLo, lowValue);
                setRegister(RegisterHi, hiValue);
            }
            else if (numArgs == 4) {
                std::string rd = instruction[1];
                RegisterKind regDest = identifyRegister(rd);
                std::string rs = instruction[2];
                RegisterKind regSource = identifyRegister(rs);
                uint32_t regSourceVal = getRegister(regSource);

                std::string rt = instruction[3];
                uint32_t sourceVal;
                if (rt.at(0) == '$') {
                    RegisterKind regSource2 = identifyRegister(rt);
                    sourceVal = getRegister(regSource2);
                }
                else {
                    sourceVal = static_cast<uint32_t>(stoi(instruction[3]));
                }
                if (sourceVal == 0) {
                    //setError();
                    //errorMessage_ = "div; divison by zero is undefined";
                    return false;
                }
                uint32_t regValue = regSourceVal / sourceVal;
                setRegister(regDest, regValue);
            }
        }
        return false;
    }


    void VirtualMachine::step() {
        int previousSourceLine = currentSourceLine_;
        if (isError()) {
            return;
        }
        if (currentSourceLine_ >= instructions_.size()) {
            setError();
            errorMessage_ = "Error: Program counter out of bounds";
            return;
        }
        std::size_t instructionIndex;
        uint32_t PCval;
        // Find instruction to compute
        if (start_) {
            instructionIndex = currentSourceLine_;
            // Assign PC for first instruction
            setRegister(RegisterPC, static_cast<std::uint32_t>(instructionIndex));
            start_ = false;
        }
        else {
            PCval = getRegister(RegisterPC);
            instructionIndex = static_cast<std::size_t>(PCval);
        }
        
        std::vector < std::string> instruction = getInstruction(instructionIndex);
        bool branch = executeInstruction(instruction);
        if (!branch) {
            currentSourceLine_++;
            setRegister(RegisterPC, currentSourceLine_);
        }
        if (branch) {
            if (static_cast<std::size_t>(previousSourceLine) == currentSourceLine_) {
                branch_ = 2;
            }
            else {
                branch_ = 1;
            }
        }
        else {
            branch_ = 0;
        }
    }

    int VirtualMachine::getBranch()
    {
        return branch_;
    }

} // namespace simmips

