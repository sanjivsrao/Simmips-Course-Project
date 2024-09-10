#ifndef VIRTUALMACHINE_HPP
#define VIRTUALMACHINE_HPP

#include "VirtualMachineInterface.hpp"
#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <unordered_map>
#include <cstring>
#include <mutex>

namespace simmips {

    // Define RegisterType representing registers 0-31, pc, hi, and lo
    struct RegisterType {
        std::vector<uint32_t> registers = std::vector<uint32_t>(35, 0); // Initialize all registers with 0
        //std::mutex mutex;
    };


    // Define MemoryType representing memory as a sequence of bytes
    struct MemoryType {
        std::vector<uint8_t> memory;
        MemoryType(std::size_t size) : memory(size, 0) {} // Initialize memory with given size, all set to 0
        //std::mutex mutex;
    };


    // Enum to indicate the type of data stored in VariantType
    enum class DataType {
        Null,
        ByteAddress,
        InstructionIndex
    };

    // Define LabelType representing associations between labels and memory locations or instructions
    struct VariantType {
        union Data {
            uint32_t byteAddress;
            std::size_t instructionIndex;

            Data() { memset(this, 0, sizeof(Data)); }
            Data(uint32_t value) : byteAddress(value) {}
            Data(std::size_t value) : instructionIndex(value) {}
            ~Data() noexcept {}
        };
        Data data;
        DataType type;

        // Default Constructor
        VariantType() : data(), type(DataType::Null){}

        // Constructor to initialize the union based on type
        VariantType(uint32_t address) : data(address), type(DataType::ByteAddress) {}
        VariantType(std::size_t index) : data(index), type(DataType::InstructionIndex) {}
    };

    // Define LabelType using the struct
    using LabelType = std::unordered_map<std::string, VariantType>;


    // Define ProgramType using the union
    using ProgramType = std::vector<std::vector<std::string>>;


    class VirtualMachine : public VirtualMachineInterface {
    public:
        // Constructor
        VirtualMachine();

        // Destructor
        virtual ~VirtualMachine() = default;

        // Method to retrieve the size of memory
        std::size_t getMemorySize() const noexcept override;

        // Method to retrieve the size of instructions
        std::size_t getInstructionSize() const noexcept override;

        // Method to retrieve a byte from memory (unsigned)
        uint8_t getMemoryByte(uint32_t address) const override;

        // Method to retrieve a byte from memory (signed)
        int8_t getMemoryByte_s(uint32_t address) const;

        // Method to retrieve a half from memory (unsigned)
        uint16_t getMemoryHalf(uint32_t address) const;

        // Method to retrieve a half from memory (signed)
        int16_t getMemoryHalf_s(uint32_t address) const;

        // Method to retrieve a word from memory (unsigned)
        uint32_t getMemoryWord(uint32_t address) const;

        // Method to retrieve a word from memory (signed)
        int32_t getMemoryWord_s(uint32_t address) const;

        // Method to retrieve the value of a register (unsigned)
        uint32_t getRegister(RegisterKind reg) const noexcept override;

        // Method to retrieve the value of a register (signed)
        int32_t getRegister_s(RegisterKind reg) const;

        // Method to get register alias
        std::string getRegisterAlias(RegisterKind reg);

        // Method to retrieve the current source line
        uint32_t getCurrentSourceLine() const noexcept override;

        // Method for writing to a register (stores as uint32_t)
        void setRegister(RegisterKind reg, uint32_t value);

        // Method for writing a byte to memory
        void setMemoryByte(uint32_t address, uint8_t value);

        // Method for writing a half to memory
        void setMemoryHalf(uint32_t address, uint16_t value);

        // Method for writing a word to memory
        void setMemoryWord(uint32_t address, uint32_t value);

        // Method for initializing variables from .data section
        void setDataMemory(std::string label, std::string layout, std::string value);
        // Adds value to memory or allocates space
        // Also modifies labels memory with label   and address

        // Method for adding a label and address to storage
        void setLabel(const std::string& label, uint32_t address);

        // Alternative method for adding a label and instruction index to storage
        void setLabel(const std::string& label, std::size_t instructionIndex);

        // Method to identify register from string name
        RegisterKind identifyRegister(const std::string& reg);

        // Method to search the labels
        bool searchLabels(const std::string& label);

        // Method to get the instruction index or address associated with a given label
        VariantType getLabel(const std::string& label) const;

        // Method to add instruction as vector of strings
        void addInstruction(std::vector < std::string>);

        // Method to get a instruction at index
        std::vector<std::string> getInstruction(std::size_t instructionIndex);

        // Method to set an error once occured
        void setError();

        // Method to check if the machine is in an error state
        bool isError() const noexcept override;

        // Method to retrieve the error message
        std::string getError() const noexcept override;

        // Method to test proper identification of labels
        void printLabels();

        // Method to test proper placement of instructions
        void printInstructions();

        // Method to test proper register instantiation
        void printRegisters();

        // Method to convert label source lines to instruction index
        void adjustLabels();

        // Method to execute one instruction
        bool executeInstruction(std::vector<std::string> instruction);

        // step the virtual machine one instruction, updating
        // the simulated memory, registers, program counter,
        // and error state. If the machine is in an error state
        // this method has no effect.
        void step() override;

        // Method to get value of branch factor for highlighting
        int getBranch();

    private:
        RegisterType registers_;
        MemoryType memory_;
        LabelType labels_;
        ProgramType instructions_;
        std::size_t currentSourceLine_;
        bool error_;
        std::string errorMessage_;
        uint32_t top_of_mem;
        bool start_;
        int branch_;
    };

}

#endif
