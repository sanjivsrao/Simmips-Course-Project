#include "catch.hpp"
#include <iostream>
#include <sstream>
#include <fstream>

#include "Parser.hpp"
#include "Lexer.hpp"
#include "Token.hpp"
#include "VirtualMachine.hpp"


#include "test_config.hpp"

using namespace simmips;

TEST_CASE("Parser VM special cases") {
    SECTION("Testing ascii") {
        std::istringstream input(R"(
        .data
        .ascii "abcd"
        .text
        main:
        lb $t0, 0;
        lb $t1, 0;
        lb $t2, 0;
        lb $t3, 0;
        )");
        TokenList tokens = tokenize(input);
        std::pair<ParseError, VirtualMachine> output = simmips::parseVM(tokens);
        for (std::size_t i = 0; i < 10; i++) {
            std::cout << "Mem Addr " << i << ": " << output.second.getMemoryByte(i) << std::endl;
        }
        REQUIRE(!output.first);
        REQUIRE(!output.second.isError());
        REQUIRE(output.second.getMemoryByte(0) == 'a');
        REQUIRE(output.second.getMemoryByte(1) == 'b');
        REQUIRE(output.second.getMemoryByte(2) == 'c');
        REQUIRE(output.second.getMemoryByte(3) == 'd');
    }
    SECTION("Testing asciiz") {
        std::istringstream input(R"(
        .data
        .asciiz "abcd"
        .text
        main:
        lb $t0, 0;
        lb $t1, 0;
        lb $t2, 0;
        lb $t3, 0;
        lb $t4, 0;
        )");
        TokenList tokens = tokenize(input);
        std::pair<ParseError, VirtualMachine> output = simmips::parseVM(tokens);
        REQUIRE(!output.first);
        REQUIRE(!output.second.isError());
        REQUIRE(output.second.getMemoryByte(0) == 'a');
        REQUIRE(output.second.getMemoryByte(1) == 'b');
        REQUIRE(output.second.getMemoryByte(2) == 'c');
        REQUIRE(output.second.getMemoryByte(3) == 'd');
        REQUIRE(output.second.getMemoryByte(4) == 0);
    }

}

TEST_CASE("Parser VM test files 0-10") {
    SECTION("Tokenize input stream with vm file 00") {
        std::ifstream input(TEST_FILE_DIR + "/vm/test00.asm");
        if (input.is_open()) {
            std::cout << "File 0 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        std::pair<ParseError, VirtualMachine> output = simmips::parseVM(tokens);
        REQUIRE(!(output.first));

        REQUIRE(output.second.getInstructionSize() == 1);
        for (std::size_t i = 0; i < output.second.getInstructionSize(); i++) {
            output.second.step();
        }
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }

    SECTION("Tokenize input stream with vm file 01") {
        std::ifstream input(TEST_FILE_DIR + "/vm/test01.asm");
        if (input.is_open()) {
            std::cout << "File 1 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        std::pair<ParseError, VirtualMachine> output = simmips::parseVM(tokens);
        REQUIRE(!(output.first));

        REQUIRE(output.second.getInstructionSize() == 6);
        REQUIRE(output.second.getMemoryWord(0) == 0);
        REQUIRE(output.second.getMemoryWord(8) == 1);
        REQUIRE(output.second.getMemoryWord(12) == static_cast<uint32_t>(-2));
        for (std::size_t i = 0; i < output.second.getInstructionSize(); i++) {
            output.second.step();
            std::cout << output.second.getError() << std::endl;
        }
        std::cout << output.second.getError() << std::endl;
        REQUIRE(!output.second.isError());
        output.second.step();
        std::cout << output.second.getError() << std::endl;
        REQUIRE(output.second.isError());

        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t0")) == 8);
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t1")) == 0);
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t2")) == 1);
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t3")) == static_cast<uint32_t>(-2));
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t4")) == static_cast<uint32_t>(-2));
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t5")) == static_cast<uint32_t>(-2));
        output.second.printInstructions();
        output.second.printLabels();
        output.second.printRegisters();
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with vm file 02") {
        std::ifstream input(TEST_FILE_DIR + "/vm/test02.asm");
        if (input.is_open()) {
            std::cout << "File 2 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        std::pair<ParseError, VirtualMachine> output = simmips::parseVM(tokens);
        std::cout << output.first.message() << std::endl;
        REQUIRE(!(output.first));
        REQUIRE(output.second.getInstructionSize() == 7);
        REQUIRE(output.second.getMemoryWord(20) == 7);
        for (std::size_t i = 0; i < output.second.getInstructionSize(); i++) {
            output.second.step();
        }
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t0")) == 4);
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t1")) == 7);
        REQUIRE(output.second.getMemoryWord(0) == 7);
        REQUIRE(output.second.getMemoryWord(4) == 7);
        REQUIRE(output.second.getMemoryWord(8) == 7);
        REQUIRE(output.second.getMemoryWord(16) == 7);
        REQUIRE(output.second.getMemoryWord(20) == 7);
        REQUIRE(!output.second.isError());
        output.second.step();
        REQUIRE(output.second.isError());
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with vm file 03") {
        std::ifstream input(TEST_FILE_DIR + "/vm/test03.asm");
        if (input.is_open()) {
            std::cout << "File 3 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        std::pair<ParseError, VirtualMachine> output = simmips::parseVM(tokens);
        std::cout << output.first.message() << std::endl;
        REQUIRE(!(output.first));
        REQUIRE(output.second.getInstructionSize() == 5);
        for (std::size_t i = 0; i < output.second.getInstructionSize(); i++) {
            output.second.step();
        }
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t0")) == 100);
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t1")) == 0);
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t2")) == 1);
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t3")) == 2);
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t4")) == 4);
        REQUIRE(!output.second.isError());
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with vm file 04") {
        std::ifstream input(TEST_FILE_DIR + "/vm/test04.asm");
        if (input.is_open()) {
            std::cout << "File 4 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        std::pair<ParseError, VirtualMachine> output = simmips::parseVM(tokens);
        std::cout << output.first.message() << std::endl;
        REQUIRE(!(output.first));
        REQUIRE(output.second.getInstructionSize() == 8);
        for (std::size_t i = 0; i < output.second.getInstructionSize(); i++) {
            output.second.step();
        }
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t0")) == static_cast<uint32_t>(-1234));
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t1")) == static_cast<uint32_t>(-1234));
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t2")) == static_cast<uint32_t>(-1234));
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t3")) == static_cast<uint32_t>(-1234));
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t4")) == static_cast<uint32_t>(-1234));
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t5")) == static_cast<uint32_t>(-1234));
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t6")) == static_cast<uint32_t>(-1234));
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t7")) == static_cast<uint32_t>(-1234));
        REQUIRE(!output.second.isError());
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with vm file 05") {
        std::ifstream input(TEST_FILE_DIR + "/vm/test05.asm");
        if (input.is_open()) {
            std::cout << "File 5 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        std::pair<ParseError, VirtualMachine> output = simmips::parseVM(tokens);
        std::cout << output.first.message() << std::endl;
        REQUIRE(!(output.first));
        REQUIRE(output.second.getMemoryWord(0) == 1);
        REQUIRE(output.second.getInstructionSize() == 3);
        for (std::size_t i = 0; i < output.second.getInstructionSize(); i++) {
            output.second.step();
        }
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t0")) == 1);
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t1")) == 0);
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t2")) == 1);
        REQUIRE(!output.second.isError());
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }

    SECTION("Tokenize input stream with vm file 06") {
        std::ifstream input(TEST_FILE_DIR + "/vm/test06.asm");
        if (input.is_open()) {
            std::cout << "File 6 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        std::pair<ParseError, VirtualMachine> output = simmips::parseVM(tokens);
        std::cout << output.first.message() << std::endl;
        REQUIRE(!(output.first));
        REQUIRE(output.second.getMemoryWord(0) == 31);
        REQUIRE(output.second.getInstructionSize() == 3);
        for (std::size_t i = 0; i < output.second.getInstructionSize(); i++) {
            output.second.step();
        }
        REQUIRE(!output.second.isError());
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }

    SECTION("Tokenize input stream with vm file 07") {
        std::ifstream input(TEST_FILE_DIR + "/vm/test07.asm");
        if (input.is_open()) {
            std::cout << "File 7 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        std::pair<ParseError, VirtualMachine> output = simmips::parseVM(tokens);
        std::cout << output.first.message() << std::endl;
        REQUIRE(!(output.first));
        REQUIRE(output.second.getMemoryWord(0) == 1);
        REQUIRE(output.second.getMemoryWord(4) == 12);
        REQUIRE(output.second.getMemoryWord(8) == static_cast<uint32_t>(-1));
        REQUIRE(output.second.getInstructionSize() == 6);
        for (std::size_t i = 0; i < output.second.getInstructionSize(); i++) {
            output.second.step();
        }
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t0")) == 1);
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t1")) == 12);
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t2")) == static_cast<uint32_t>(-1));
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t3")) == static_cast<uint32_t>(-1));
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t4")) == 11);
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t5")) == static_cast<uint32_t>(-3));
        REQUIRE(!output.second.isError());
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with vm file 08") {
        std::ifstream input(TEST_FILE_DIR + "/vm/test08.asm");
        if (input.is_open()) {
            std::cout << "File 8 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        std::pair<ParseError, VirtualMachine> output = simmips::parseVM(tokens);
        std::cout << output.first.message() << std::endl;
        REQUIRE(!(output.first));
        REQUIRE(output.second.getMemoryWord(0) == 1);
        REQUIRE(output.second.getMemoryWord(4) == 12);
        REQUIRE(output.second.getMemoryWord(8) == static_cast<uint32_t>(-1));
        REQUIRE(output.second.getInstructionSize() == 6);
        for (std::size_t i = 0; i < output.second.getInstructionSize(); i++) {
            output.second.step();
        }
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t0")) == 1);
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t1")) == 12);
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t2")) == static_cast<uint32_t>(-1));
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t3")) == 4294967295);
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t4")) == 11);
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t5")) == 4294967293);
        REQUIRE(!output.second.isError());
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with vm file 09") {
        std::ifstream input(TEST_FILE_DIR + "/vm/test09.asm");
        if (input.is_open()) {
            std::cout << "File 9 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        std::pair<ParseError, VirtualMachine> output = simmips::parseVM(tokens);
        std::cout << output.first.message() << std::endl;
        REQUIRE(!(output.first));
        REQUIRE(output.second.getMemoryWord(0) == 2);
        REQUIRE(output.second.getMemoryWord(4) == static_cast<uint32_t>(-2));
        REQUIRE(output.second.getMemoryWord(8) == 1073741824);
        REQUIRE(output.second.getInstructionSize() == 20);


        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t0")) == 2);
        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t1")) == 4);
        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t0")) == 2);
        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t8")) == 8);
        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t9")) == 0);

        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t0")) == static_cast<uint32_t>(-2));
        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t1")) == 2);
        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t0")) == static_cast<uint32_t>(-2));
        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t8")) == static_cast<uint32_t>(-4));
        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t9")) == static_cast<uint32_t>(-1));

        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t0")) == 1073741824);
        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t1")) == 4);
        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t0")) == 1073741824);
        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t8")) == 0);
        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t9")) == 1);

        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t0")) == 1073741824);
        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t1")) == static_cast<uint32_t>(-4));
        output.second.step();
        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t8")) == 0);
        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t9")) == static_cast<uint32_t>(-1));



        REQUIRE(!output.second.isError());
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with vm file 10") {
        std::ifstream input(TEST_FILE_DIR + "/vm/test10.asm");
        if (input.is_open()) {
            std::cout << "File 10 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        std::pair<ParseError, VirtualMachine> output = simmips::parseVM(tokens);
        std::cout << output.first.message() << std::endl;
        REQUIRE(!(output.first));
        REQUIRE(output.second.getMemoryWord(0) == 2);
        REQUIRE(output.second.getMemoryWord(4) == 1073741824);
        REQUIRE(output.second.getInstructionSize() == 10);
        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t0")) == 2);
        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t1")) == 4);
        output.second.step();
        REQUIRE(output.second.getRegister(RegisterLo) == 8);
        REQUIRE(output.second.getRegister(RegisterHi) == 0);
        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t8")) == 8);
        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t9")) == 0);

        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t0")) == 1073741824);
        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t1")) == 4);
        output.second.step();
        REQUIRE(output.second.getRegister(RegisterLo) == 0);
        REQUIRE(output.second.getRegister(RegisterHi) == 1);
        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t8")) == 0);
        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t9")) == 1);
        REQUIRE(!output.second.isError());
        std::cout << output.second.getError() << std::endl;
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
}

TEST_CASE("Parser VM test files 11-20") {
    SECTION("Tokenize input stream with vm file 11") {
        std::ifstream input(TEST_FILE_DIR + "/vm/test11.asm");
        if (input.is_open()) {
            std::cout << "File 11 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        std::pair<ParseError, VirtualMachine> output = simmips::parseVM(tokens);
        std::cout << output.first.message() << std::endl;
        REQUIRE(!(output.first));
        REQUIRE(output.second.getMemoryWord(0) == 2);
        REQUIRE(output.second.getMemoryWord(4) == static_cast<uint32_t>(- 2));
        REQUIRE(output.second.getMemoryWord(8) == 1073741824);
        REQUIRE(output.second.getInstructionSize() == 21);
        for (std::size_t i = 0; i < output.second.getInstructionSize(); i++) {
            output.second.step();
        }
        std::cout << output.second.getError() << std::endl;
        REQUIRE(!output.second.isError());
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with vm file 12") {
        std::ifstream input(TEST_FILE_DIR + "/vm/test12.asm");
        if (input.is_open()) {
            std::cout << "File 12 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        std::pair<ParseError, VirtualMachine> output = simmips::parseVM(tokens);
        std::cout << output.first.message() << std::endl;
        REQUIRE(!(output.first));
        REQUIRE(output.second.getMemoryWord(0) == 2);
        REQUIRE(output.second.getMemoryWord(4) == 1073741825);
        REQUIRE(output.second.getInstructionSize() == 11);
        for (std::size_t i = 0; i < output.second.getInstructionSize(); i++) {
            output.second.step();
        }
        std::cout << output.second.getError() << std::endl;
        REQUIRE(!output.second.isError());
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with vm file 13") {
        std::ifstream input(TEST_FILE_DIR + "/vm/test13.asm");
        if (input.is_open()) {
            std::cout << "File 13 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        std::pair<ParseError, VirtualMachine> output = simmips::parseVM(tokens);
        std::cout << output.first.message() << std::endl;
        REQUIRE(!(output.first));
        REQUIRE(output.second.getMemoryWord(0) == 12);
        REQUIRE(output.second.getMemoryWord(4) == 10);
        REQUIRE(output.second.getInstructionSize() == 4);
        for (std::size_t i = 0; i < output.second.getInstructionSize(); i++) {
            output.second.step();
        }
        REQUIRE(!output.second.isError());
        std::cout << output.second.getError() << std::endl;
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with vm file 14") {
        std::ifstream input(TEST_FILE_DIR + "/vm/test14.asm");
        if (input.is_open()) {
            std::cout << "File 14 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        std::pair<ParseError, VirtualMachine> output = simmips::parseVM(tokens);
        std::cout << output.first.message() << std::endl;
        REQUIRE(!(output.first));
        REQUIRE(output.second.getMemoryWord(0) == 12);
        REQUIRE(output.second.getMemoryWord(4) == 10);
        REQUIRE(output.second.getInstructionSize() == 4);
        for (std::size_t i = 0; i < output.second.getInstructionSize(); i++) {
            output.second.step();
        }
        REQUIRE(!output.second.isError());
        std::cout << output.second.getError() << std::endl;
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with vm file 15") {
        std::ifstream input(TEST_FILE_DIR + "/vm/test15.asm");
        if (input.is_open()) {
            std::cout << "File 15 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        std::pair<ParseError, VirtualMachine> output = simmips::parseVM(tokens);
        std::cout << output.first.message() << std::endl;
        REQUIRE(!(output.first));
        REQUIRE(output.second.getMemoryWord(0) == 12);
        REQUIRE(output.second.getMemoryWord(4) == 10);
        REQUIRE(output.second.getInstructionSize() == 4);
        for (std::size_t i = 0; i < output.second.getInstructionSize(); i++) {
            output.second.step();
        }
        REQUIRE(!output.second.isError());
        std::cout << output.second.getError() << std::endl;
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with vm file 16") {
        std::ifstream input(TEST_FILE_DIR + "/vm/test16.asm");
        if (input.is_open()) {
            std::cout << "File 16 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        std::pair<ParseError, VirtualMachine> output = simmips::parseVM(tokens);
        std::cout << output.first.message() << std::endl;
        REQUIRE(!(output.first));
        REQUIRE(output.second.getMemoryWord(0) == 12);
        REQUIRE(output.second.getMemoryWord(4) == 10);
        REQUIRE(output.second.getInstructionSize() == 4);
        for (std::size_t i = 0; i < output.second.getInstructionSize(); i++) {
            output.second.step();
        }
        REQUIRE(!output.second.isError());
        std::cout << output.second.getError() << std::endl;
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with vm file 17") {
        std::ifstream input(TEST_FILE_DIR + "/vm/test17.asm");
        if (input.is_open()) {
            std::cout << "File 17 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        std::pair<ParseError, VirtualMachine> output = simmips::parseVM(tokens);
        std::cout << output.first.message() << std::endl;
        REQUIRE(!(output.first));
        REQUIRE(output.second.getMemoryWord(0) == 12);
        REQUIRE(output.second.getMemoryWord(4) == 10);
        REQUIRE(output.second.getInstructionSize() == 5);

        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t0")) == 12);
        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t1")) == 10);
        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t2")) == 4294967283);
        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t2")) == 4294967285);
        output.second.step();
        REQUIRE(output.second.getRegister(output.second.identifyRegister("$t2")) == 4294967292);

        REQUIRE(!output.second.isError());
        std::cout << output.second.getError() << std::endl;
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with vm file 18") {
        std::ifstream input(TEST_FILE_DIR + "/vm/test18.asm");
        if (input.is_open()) {
            std::cout << "File 18 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        std::pair<ParseError, VirtualMachine> output = simmips::parseVM(tokens);
        std::cout << output.first.message() << std::endl;
        REQUIRE(!(output.first));
        REQUIRE(output.second.getInstructionSize() == 5);
        for (std::size_t i = 0; i < output.second.getInstructionSize(); i++) {
            output.second.step();
        }
        REQUIRE(!output.second.isError());
        std::cout << output.second.getError() << std::endl;
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with vm file 19") {
        std::ifstream input(TEST_FILE_DIR + "/vm/test19.asm");
        if (input.is_open()) {
            std::cout << "File 19 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        std::pair<ParseError, VirtualMachine> output = simmips::parseVM(tokens);
        std::cout << output.first.message() << std::endl;
        REQUIRE(!(output.first));
        REQUIRE(output.second.getMemoryWord(0) == 0);
        REQUIRE(output.second.getMemoryWord(4) == 1);
        REQUIRE(output.second.getMemoryWord(8) == 2);
        REQUIRE(output.second.getMemoryWord(12) == 3);
        REQUIRE(output.second.getInstructionSize() == 24);
        for (std::size_t i = 0; i < output.second.getInstructionSize(); i++) {
            output.second.step();
        }
        REQUIRE(!output.second.isError());
        std::cout << output.second.getError() << std::endl;
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with vm file 20") {
        std::ifstream input(TEST_FILE_DIR + "/vm/test20.asm");
        if (input.is_open()) {
            std::cout << "File 20 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        std::pair<ParseError, VirtualMachine> output = simmips::parseVM(tokens);
        std::cout << output.first.message() << std::endl;
        REQUIRE(!(output.first));
        REQUIRE(output.second.getMemoryWord(0) == 10);
        REQUIRE(output.second.getMemoryWord(4) == 0);
        REQUIRE(output.second.getInstructionSize() == 10);
        for (std::size_t i = 0; i < output.second.getInstructionSize(); i++) {
            output.second.step();
        }

        REQUIRE(!output.second.isError());
        std::cout << output.second.getError() << std::endl;
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
}

TEST_CASE("Testing VM register alias function") {
    VirtualMachine vm;

    SECTION("General Purpose Registers") {
        REQUIRE(vm.getRegisterAlias(Register00) == "$zero");
        REQUIRE(vm.getRegisterAlias(Register01) == "$at");
        REQUIRE(vm.getRegisterAlias(Register02) == "$v0");
        REQUIRE(vm.getRegisterAlias(Register03) == "$v1");
        REQUIRE(vm.getRegisterAlias(Register04) == "$a0");
        REQUIRE(vm.getRegisterAlias(Register05) == "$a1");
        REQUIRE(vm.getRegisterAlias(Register06) == "$a2");
        REQUIRE(vm.getRegisterAlias(Register07) == "$a3");
        REQUIRE(vm.getRegisterAlias(Register08) == "$t0");
        REQUIRE(vm.getRegisterAlias(Register09) == "$t1");
        REQUIRE(vm.getRegisterAlias(Register10) == "$t2");
        REQUIRE(vm.getRegisterAlias(Register11) == "$t3");
        REQUIRE(vm.getRegisterAlias(Register12) == "$t4");
        REQUIRE(vm.getRegisterAlias(Register13) == "$t5");
        REQUIRE(vm.getRegisterAlias(Register14) == "$t6");
        REQUIRE(vm.getRegisterAlias(Register15) == "$t7");
        REQUIRE(vm.getRegisterAlias(Register16) == "$s0");
        REQUIRE(vm.getRegisterAlias(Register17) == "$s1");
        REQUIRE(vm.getRegisterAlias(Register18) == "$s2");
        REQUIRE(vm.getRegisterAlias(Register19) == "$s3");
        REQUIRE(vm.getRegisterAlias(Register20) == "$s4");
        REQUIRE(vm.getRegisterAlias(Register21) == "$s5");
        REQUIRE(vm.getRegisterAlias(Register22) == "$s6");
        REQUIRE(vm.getRegisterAlias(Register23) == "$s7");
        REQUIRE(vm.getRegisterAlias(Register24) == "$t8");
        REQUIRE(vm.getRegisterAlias(Register25) == "$t9");
    }

    SECTION("Special Registers") {
        REQUIRE(vm.getRegisterAlias(Register26) == "$k0");
        REQUIRE(vm.getRegisterAlias(Register27) == "$k1");
        REQUIRE(vm.getRegisterAlias(Register28) == "$gp");
        REQUIRE(vm.getRegisterAlias(Register29) == "$sp");
        REQUIRE(vm.getRegisterAlias(Register30) == "$fp");
        REQUIRE(vm.getRegisterAlias(Register31) == "$ra");
        REQUIRE(vm.getRegisterAlias(RegisterPC) == "$pc");
        REQUIRE(vm.getRegisterAlias(RegisterHi) == "$hi");
        REQUIRE(vm.getRegisterAlias(RegisterLo) == "$lo");
    }

    SECTION("Default Case") {
        // Test the default case where no matching alias is found
        REQUIRE(vm.getRegisterAlias(RegisterUnknown) == "");
    }
}

TEST_CASE("Testing other functions") {
    VirtualMachine vm;

    SECTION("getCurrentSourceLine") {
        REQUIRE(vm.getCurrentSourceLine() == 0);
    }

    SECTION("setMemoryWord") {
        REQUIRE_THROWS(vm.setMemoryWord(1025,2));
    }
    SECTION("setMemoryHalf") {
        REQUIRE_THROWS(vm.setMemoryHalf(1025, 2));
    }
    SECTION("identifyRegister") {
        REQUIRE(vm.identifyRegister("$100") == RegisterUnknown);
    }
    SECTION("getMemoryByte_s") {
        REQUIRE(vm.getMemoryByte_s(0) == 0);
    }
    SECTION("getMemoryHalf_s") {
        REQUIRE(vm.getMemoryHalf_s(0) == 0);
    }
    SECTION("getMemoryWord_s") {
        REQUIRE(vm.getMemoryWord_s(0) == 0);
    }
    SECTION("getMemoryByte") {
        REQUIRE(vm.getMemoryWord(0) == 0);
    }
    SECTION("getMemoryHalf") {
        REQUIRE(vm.getMemoryHalf(0) == 0);
    }
    SECTION("getMemoryWord") {
        REQUIRE(vm.getMemoryHalf(0) == 0);
    }
    SECTION("getBranch") {
        REQUIRE(vm.getBranch() == false);
    }
    SECTION("Testing untested functions") {
        std::istringstream input(R"(
        .data
                .space 8
        var1:   .word 1
        var2:   .word -2

        .text
        main:
     	    la $t0, var1
            mthi $t0
            mtlo $t1
	        lw $t1, 0
	        lw $t2, $t0
	        lw $t3, 4($t0)
	        lw $t4, 4(var1)
	        lw $t5, var2
            lw $t1, (0)
	        lw $t2, ($t0)
	        lw $t3, (var1)
	        lw $t4, 4(0)
            lh $t1, 0
	        lh $t2, $t0
	        lh $t3, 4($t0)
	        lh $t4, 4(var1)
	        lh $t5, var2
            lh $t1, (0)
	        lh $t2, ($t0)
	        lh $t3, (var1)
	        lh $t4, 4(0)
            lb $t1, 0
	        lb $t2, $t0
	        lb $t3, 4($t0)
	        lb $t4, 4(var1)
	        lb $t5, var2
            lb $t1, (0)
	        lb $t2, ($t0)
	        lb $t3, (var1)
	        lb $t4, 4(0)
            sw $t0, (0)
            sw $t1,($t2)
            sw $t2, (var1)
            sw $t3, 4(0)
            sh $t1, 0
	        sh $t2, $t0
	        sh $t3, 4($t0)
	        sh $t4, 4(var1)
	        sh $t5, var2
            sh $t1, (0)
	        sh $t2, ($t0)
	        sh $t3, (var1)
	        sh $t4, 4(0)
            sb $t1, 0
	        sb $t2, $t0
	        sb $t3, 4($t0)
	        sb $t4, 4(var1)
	        sb $t5, var2
            sb $t1, (0)
	        sb $t2, ($t0)
	        sb $t3, (var1)
	        sb $t4, 4(0)
            la $s0, $t0
            la $s1, 0
        )");
        TokenList tokens = tokenize(input);
        std::pair<ParseError, VirtualMachine> output = simmips::parseVM(tokens);
        REQUIRE(!output.first);
        REQUIRE(!output.second.isError());
        for (std::size_t i = 0; i < output.second.getInstructionSize(); i++) {
            output.second.step();
        }

    }
    SECTION("Testing more untested functions") {
        std::istringstream input(R"(
        .data
        .asciiz "abcd"
        .text
        main:
        li $t1, 1
        abs $t0, $0
        neg $t1, $t1
        negu $t1, $t1
        multu $t1, $t1
        mul $t1, $t1, $t1
        mulo $t1, $t1, $t1
        mulou $t1, $t1, $t1
        rem $t1, $t1, $t1
        remu $t1, $t1, $t1
        div $t1, $t1, $t1
        divu $t1, $t1, $t1
        div $t1, $t1
        divu $t1, $t1
        div $t1, $t1, $0
        divu $t1, $t1, $0
        )");
        TokenList tokens = tokenize(input);
        std::pair<ParseError, VirtualMachine> output = simmips::parseVM(tokens);
        REQUIRE(!output.first);
        REQUIRE(!output.second.isError());
        for (std::size_t i = 0; i < output.second.getInstructionSize(); i++) {
            output.second.step();
        }
    }
}


