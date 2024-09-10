#include "catch.hpp"
#include <iostream>
#include <sstream>
#include <fstream>

#include "Parser.hpp"
#include "Lexer.hpp"
#include "Token.hpp"

#include "test_config.hpp"

using namespace simmips;

TEST_CASE("Parser base data section student tests") {
    
    SECTION("Testing constant") {
        std::istringstream input(R"(
        .data
        a5424acbmdkrigjsl = 10
        abcdefghijkLMNOPQRUST = -10
        a1234567890 = +10
        bcd = -0
        a1Pc42 = +0
        apple = 0
        )");
        TokenList tokens = tokenize(input);
        ParseError error = parse(tokens);
        REQUIRE(bool(error) == false);
        std::cout << error.message() << std::endl << std::endl;

        std::istringstream input1(R"(
        .data
        a5424acbmdkrigjsl = 10
        abcdefghijkLMNOPQRUST = -10
        a1234567890 = +10
        bcd = -0
        a1Pc42 = +0
        apple = 0+0
        )");
        TokenList tokens1 = tokenize(input1);
        ParseError error1 = parse(tokens1);
        REQUIRE(bool(error1) == true);
        std::cout << error1.message() << std::endl << std::endl;

        std::istringstream input2(R"(
        .data
        = a5424acbmdkrigjsl = 10
        abcdefghijkLMNOPQRUST = -10
        a1234567890 = +10
        bcd = -0
        a1Pc42 = +0
        apple = 49573
        )");
        TokenList tokens2 = tokenize(input2);
        ParseError error2 = parse(tokens2);
        REQUIRE(bool(error2) == true);
        std::cout << error2.message() << std::endl << std::endl;

        std::istringstream input3(R"(
        .data
        a5424acbmdkrigjsl = 10
        abcdefghijkLMNOPQRUST = -10
        a1234567890 = +10
        bcd = -0
        1Pc42 = +0
        apple = 0
        )");
        TokenList tokens3 = tokenize(input3);
        ParseError error3 = parse(tokens3);
        REQUIRE(bool(error3) == true);
        std::cout << error3.message() << std::endl << std::endl;

        std::istringstream input4(R"(
        .data
        a5424acbmdkrigjsl = 10
        abcdefghijkLMNOPQRUST = -10
        a1234567890 = = 10
        bcd = -0
        aPc42 = +0
        apple = 0
        )");
        TokenList tokens4 = tokenize(input4);
        ParseError error4 = parse(tokens4);
        REQUIRE(bool(error4) == true);
        std::cout << error4.message() << std::endl << std::endl;

        std::istringstream input5(R"(
        .data
        a5424acbmdkrigjsl = 10
        abcdefghijkLMNOPQRUST = -10
        a1234567890 = 10
        bcd = --0 
        aPc42 = +0
        apple = 0+0
        )");
        TokenList tokens5 = tokenize(input5);
        ParseError error5 = parse(tokens5);
        REQUIRE(bool(error5) == true);
        std::cout << error5.message() << std::endl << std::endl;


        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Testing label") {
        std::cout << "Running label student tests" << std::endl;
        std::istringstream input(R"(
        .data
        a123:
        )");
        TokenList tokens = tokenize(input);
        ParseError error = parse(tokens);
        REQUIRE(bool(error) == false);
        std::cout << error.message() << std::endl << std::endl;

        std::istringstream input1(R"(
        .data
        Z5424acbmdkrigjsl:
        )");
        TokenList tokens1 = tokenize(input1);
        ParseError error1 = parse(tokens1);
        REQUIRE(bool(error1) == false);
        std::cout << error1.message() << std::endl << std::endl;

        std::istringstream input2(R"(
        .data
        :a5424acbmdkrigjsl:
        )");
        TokenList tokens2 = tokenize(input2);
        ParseError error2 = parse(tokens2);
        REQUIRE(bool(error2) == true);
        std::cout << error2.message() << std::endl << std::endl;

        std::istringstream input3(R"(
        .data
        a5424acbmdkrigjsl:
        1112abc:
        )");
        TokenList tokens3 = tokenize(input3);
        ParseError error3 = parse(tokens3);
        REQUIRE(bool(error3) == true);
        std::cout << error3.message() << std::endl << std::endl;

        std::istringstream input4(R"(
        .data
        ABCD1234:
        )");
        TokenList tokens4 = tokenize(input4);
        ParseError error4 = parse(tokens4);
        REQUIRE(bool(error4) == false);
        std::cout << error4.message() << std::endl << std::endl;

        std::istringstream input5(R"(
        .data
        a5424a abc123:
        )");
        TokenList tokens5 = tokenize(input5);
        ParseError error5 = parse(tokens5);
        REQUIRE(bool(error5) == true);
        std::cout << error5.message() << std::endl << std::endl;

        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Testing 32-bit integer layout values") {
        std::istringstream input1(R"(
        .data
        .word +2147483647, -2147483648, 0, 10, -10
        )");
        TokenList tokens1 = tokenize(input1);
        ParseError error1 = parse(tokens1);
        std::cout << error1.message() << std::endl << std::endl;
        REQUIRE(bool(error1) == false);


        std::istringstream input1A(R"(
        .text
        main:
            li $t1,42
            li $t2,+42
            li $t3,-42
        end:
            j end
        )");
        TokenList tokens1A = tokenize(input1A);
        std::pair<ParseError, VirtualMachine> output1 = simmips::parseVM(tokens1A);
        REQUIRE(!output1.first);

        std::istringstream input1B(R"(
        .data
        .text
        .data
        .text
        )");
        TokenList tokens1B = tokenize(input1B);
        std::pair<ParseError, VirtualMachine> output2 = simmips::parseVM(tokens1B);
        REQUIRE(!output2.first);


        std::istringstream input2(R"(
        .data
        .word +2147483648, -2147483649, 0, 10, -10
        )");
        TokenList tokens2 = tokenize(input2);
        ParseError error2 = parse(tokens2);
        REQUIRE(bool(error2) == true);
        std::cout << error2.message() << std::endl << std::endl;

        std::istringstream input3(R"(
        .data
        .word 4294967295,0, 10
        )");
        TokenList tokens3 = tokenize(input3);
        ParseError error3 = parse(tokens3);
        std::cout << error3.message() << std::endl << std::endl;
        REQUIRE(bool(error3) == false);


        std::istringstream input4(R"(
        .data
        .word 4294967296,0 ,10, -1
        )");
        TokenList tokens4 = tokenize(input4);
        ParseError error4 = parse(tokens4);
        REQUIRE(bool(error4) == true);
        std::cout << error4.message() << std::endl << std::endl;
    }
    SECTION("Testing 16-bit integer layout values") {

        std::istringstream input2(R"(
        .data
        ABC: .half +32767, -32768, 0, 10, -10
        
        )");
        TokenList tokens2 = tokenize(input2);
        ParseError error2 = parse(tokens2);
        REQUIRE(bool(error2) == false);
        std::cout << error2.message() << std::endl << std::endl;

        std::istringstream input2a(R"(
        .data
        ABC: .half +32767, -32769, 0, 10, -10
        
        )");
        TokenList tokens2a = tokenize(input2a);
        ParseError error2a = parse(tokens2a);
        REQUIRE(bool(error2a) == true);
        std::cout << error2a.message() << std::endl << std::endl;

        std::istringstream input3(R"(
        .data
        abc: .half 15
        1abc: .half +150, -150
        )");
        TokenList tokens3 = tokenize(input3);
        ParseError error3 = parse(tokens3);
        REQUIRE(bool(error3) == true);
        std::cout << error3.message() << std::endl << std::endl;

        std::istringstream input4(R"(
        .data
        ABCD1234: .byte +127, -128, 0, 10, -10
        )");
        TokenList tokens4 = tokenize(input4);
        ParseError error4 = parse(tokens4);
        REQUIRE(bool(error4) == false);
        std::cout << error4.message() << std::endl << std::endl;

        std::istringstream input5(R"(
        .data
        a5424: .byte +128, 0, -128
        )");
        TokenList tokens5 = tokenize(input5);
        ParseError error5 = parse(tokens5);
        REQUIRE(bool(error5) == true);
        std::cout << error5.message() << std::endl << std::endl;

        std::istringstream input6(R"(
        .data
        a123: .space +2147483647, +500, 0
        a450: .space +2147483648
        )");
        TokenList tokens6 = tokenize(input6);
        ParseError error6 = parse(tokens6);
        REQUIRE(bool(error6) == true);
        std::cout << error6.message() << std::endl << std::endl;

        std::istringstream input7(R"(
        .data
        Z5424acbmdkrigjsl: .space 10, 200, -1
        )");
        TokenList tokens7 = tokenize(input7);
        ParseError error7 = parse(tokens7);
        REQUIRE(bool(error7) == true);
        std::cout << error7.message() << std::endl << std::endl;

        std::istringstream input8(R"(
        .data
        a123: .spaace 4290, +100, 50
        )");
        TokenList tokens8 = tokenize(input8);
        ParseError error8 = parse(tokens8);
        REQUIRE(bool(error8) == true);
        std::cout << error8.message() << std::endl << std::endl;

        std::istringstream input9(".data\n abc: .ascii \"hello\"");
        TokenList tokens9 = tokenize(input9);
        ParseError error9 = parse(tokens9);
        REQUIRE(bool(error9) == false);
        std::cout << error9.message() << std::endl << std::endl;

        std::istringstream input10(".data\n abc: .asciiz \"hello\"\0");
        TokenList tokens10 = tokenize(input10);
        ParseError error10 = parse(tokens10);
        REQUIRE(bool(error10) == false);
        std::cout << error10.message() << std::endl << std::endl;

        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
}


TEST_CASE("Parser passing text file tests") {
    
    SECTION("Tokenize input stream with passing test file 0") {
        std::ifstream input(TEST_FILE_DIR + "/pass/win/test00.asm");
        if (input.is_open()) {
            std::cout << "File 0 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        ParseError error = parse(tokens);
        std::cout << error.message() << std::endl;
        REQUIRE(tokens.size() == 50);
        REQUIRE(tokens.front().line() == 2);
        REQUIRE(!error);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with passing test file 1") {
        std::ifstream input(TEST_FILE_DIR + "/pass/win/test01.asm");
        if (input.is_open()) {
            std::cout << "File 1 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        ParseError error = parse(tokens);
        std::cout << error.message() << std::endl;
        REQUIRE(tokens.size() == 12);
        REQUIRE(!error);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with passing test file 2") {
        std::ifstream input(TEST_FILE_DIR + "/pass/win/test02.asm");
        if (input.is_open()) {
            std::cout << "File 2 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        ParseError error = parse(tokens);
        std::cout << error.message() << std::endl;
        REQUIRE(tokens.size() == 58);
        REQUIRE(!error);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with passing test file 3") {
        std::ifstream input(TEST_FILE_DIR + "/pass/win/test03.asm");
        if (input.is_open()) {
            std::cout << "File 3 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        ParseError error = parse(tokens);
        std::cout << error.message() << std::endl;
        REQUIRE(tokens.size() == 217);
        REQUIRE(!error);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with passing test file 4") {
        std::ifstream input(TEST_FILE_DIR + "/pass/win/test04.asm");
        if (input.is_open()) {
            std::cout << "File 4 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        ParseError error = parse(tokens);
        std::cout << error.message() << std::endl;
        REQUIRE(tokens.size() == 22 + 20 + 56);
        REQUIRE(!error);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with passing test file 5") {
        std::ifstream input(TEST_FILE_DIR + "/pass/win/test05.asm");
        if (input.is_open()) {
            std::cout << "File 5 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        ParseError error = parse(tokens);
        std::cout << error.message() << std::endl;
        REQUIRE(tokens.size() == 83);
        REQUIRE(!error);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with passing test file 6") {
        std::ifstream input(TEST_FILE_DIR + "/pass/win/test06.asm");
        if (input.is_open()) {
            std::cout << "File 6 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        ParseError error = parse(tokens);
        std::cout << error.message() << std::endl;
        REQUIRE(tokens.size() == 69);
        REQUIRE(!error);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with passing test file 7") {
        std::ifstream input(TEST_FILE_DIR + "/pass/win/test07.asm");
        if (input.is_open()) {
            std::cout << "File 7 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        ParseError error = parse(tokens);
        std::cout << error.message() << std::endl;
        REQUIRE(tokens.size() == 321);
        REQUIRE(!error);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    
    SECTION("Tokenize input stream with passing test file 8") {
        std::ifstream input(TEST_FILE_DIR + "/pass/win/test08.asm");
        if (input.is_open()) {
            std::cout << "File 8 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        ParseError error = parse(tokens);
        std::cout << error.message() << std::endl;
        REQUIRE(tokens.size() == 18 + 10 + 7 + 32);
        REQUIRE(!error);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
}

TEST_CASE("Parser failing text file tests"){
    SECTION("Tokenize input stream with failing test file 0") {
        std::ifstream input(TEST_FILE_DIR + "/fail/win/test00.asm");
        if (input.is_open()) {
            std::cout << "File 0 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        ParseError error = parse(tokens);
        std::cout << error.message() << std::endl;
        std::string message = error.message();
        std::string line = message.substr(6, 1);
        REQUIRE(error);
        REQUIRE(line == "9");
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with failing test file 1") {
        std::ifstream input(TEST_FILE_DIR + "/fail/win/test01.asm");
        if (input.is_open()) {
            std::cout << "File 1 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        ParseError error = parse(tokens);
        std::cout << error.message() << std::endl;
        std::string message = error.message();
        std::string line = message.substr(6, 1);
        REQUIRE(error);
        REQUIRE(line == "3");
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with failing test file 2") {
        std::ifstream input(TEST_FILE_DIR + "/fail/win/test02.asm");
        if (input.is_open()) {
            std::cout << "File 2 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        ParseError error = parse(tokens);
        std::cout << error.message() << std::endl;
        std::string message = error.message();
        std::string line = message.substr(6, 2);
        REQUIRE(error);
        REQUIRE(line == "10");
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with failing test file 3") {
        std::ifstream input(TEST_FILE_DIR + "/fail/win/test03.asm");
        if (input.is_open()) {
            std::cout << "File 3 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        ParseError error = parse(tokens);
        std::cout << error.message() << std::endl;
        std::string message = error.message();
        std::string line = message.substr(6, 1);
        REQUIRE(error);
        REQUIRE(line == "9");
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with failing test file 4") {
        std::ifstream input(TEST_FILE_DIR + "/fail/win/test04.asm");
        if (input.is_open()) {
            std::cout << "File 4 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        ParseError error = parse(tokens);
        std::cout << error.message() << std::endl;
        std::string message = error.message();
        std::string line = message.substr(6, 2);
        REQUIRE(error);
        REQUIRE(line == "34");
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with failing test file 5") {
        std::ifstream input(TEST_FILE_DIR + "/fail/win/test05.asm");
        if (input.is_open()) {
            std::cout << "File 5 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        ParseError error = parse(tokens);
        std::cout << error.message() << std::endl;
        std::string message = error.message();
        std::string line = message.substr(6, 2);
        REQUIRE(error);
        REQUIRE(line == "17");
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with failing test file 6") {
        std::ifstream input(TEST_FILE_DIR + "/fail/win/test06.asm");
        if (input.is_open()) {
            std::cout << "File 6 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        ParseError error = parse(tokens);
        std::cout << error.message() << std::endl;
        std::string message = error.message();
        std::string line = message.substr(6, 2);
        REQUIRE(error);
        REQUIRE(line == "23");
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with failing test file 7") {
        std::ifstream input(TEST_FILE_DIR + "/fail/win/test07.asm");
        if (input.is_open()) {
            std::cout << "File 7 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        ParseError error = parse(tokens);
        std::cout << error.message() << std::endl;
        std::string message = error.message();
        std::string line = message.substr(6, 2);
        REQUIRE(error);
        REQUIRE(line == "20");
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with failing test file 8") {
        std::ifstream input(TEST_FILE_DIR + "/fail/win/test08.asm");
        if (input.is_open()) {
            std::cout << "File 8 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        ParseError error = parse(tokens);
        std::cout << error.message() << std::endl;
        std::string message = error.message();
        std::string line = message.substr(6, 2);
        REQUIRE(error);
        REQUIRE(line == "12");
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with failing test file 9") {
        std::ifstream input(TEST_FILE_DIR + "/fail/win/test09.asm");
        if (input.is_open()) {
            std::cout << "File 9 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        ParseError error = parse(tokens);
        std::cout << error.message() << std::endl;
        std::string message = error.message();
        std::string line = message.substr(6, 1);
        REQUIRE(error);
        REQUIRE(line == "3");
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
}

TEST_CASE("Extra parsing tests for special cases"){
    
    SECTION("Parsing an empty file") {
        std::istringstream input(R"(

        )");
        TokenList tokens = simmips::tokenize(input);
        ParseError error = parse(tokens);
        std::cout << error.message() << std::endl;
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Parsing a basic .data section (no errors)") {
        std::istringstream input(R"(
        .data
        ALPHA123 = 1
        var1:
        var2: .word 2
        var3: .ascii "Hello"
        .word 5
        )");
        TokenList tokens = simmips::tokenize(input);
        ParseError error = parse(tokens);
        std::cout << error.message() << std::endl;
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Testing failing cases") {
    
        std::istringstream input(R"(
        .dataa
        a5424acbmdkrigjsl = 10
        )");
        TokenList tokens = tokenize(input);
        std::cout << std::endl;
        ParseError error = parse(tokens);
        REQUIRE(bool(error) == true);
        std::cout << error.message() << std::endl;


        std::istringstream input1(R"(
        .data
        a5424acbmdkrigjsl = 1$0
        abcdefghijkLMNOPQRUST = -10
        a1234567890 = +10
        bcd = -0
        a1Pc42 = +0
        apple = 0+0
        )");
        TokenList tokens1 = tokenize(input1);
        std::cout << std::endl;
        ParseError error1 = parse(tokens1);
        REQUIRE(bool(error1) == true);
        std::cout << error1.message() << std::endl;


        std::istringstream input2(R"(
        .data
        var3: .woord 3
        )");
        TokenList tokens2 = tokenize(input2);
        std::cout << std::endl;
        ParseError error2 = parse(tokens2);
        REQUIRE(bool(error2) == true);
        std::cout << error2.message() << std::endl;


        std::istringstream input3(R"(
        .data:
        a5424acbmdkrigjsl = 10
        abcdefghijkLMNOPQRUST = -10
        a1234567890 = +10
        bcd = -0
        apple = 0
        )");
        TokenList tokens3 = tokenize(input3);
        std::cout << std::endl;
        ParseError error3 = parse(tokens3);
        REQUIRE(bool(error3) == true);
        std::cout << error3.message() << std::endl;

        std::istringstream input4(R"(
        .data
        A = 10
        var3: .word B
        )");
        TokenList tokens4 = tokenize(input4);
        std::cout << std::endl;
        ParseError error4 = parse(tokens4);
        REQUIRE(bool(error4) == true);
        std::cout << error4.message() << std::endl;

        std::istringstream input4a(R"(
        .data
        A = 10
        var3: .word A
        )");
        TokenList tokens4a = tokenize(input4a);
        std::cout << std::endl;
        ParseError error4a = parse(tokens4a);
        REQUIRE(bool(error4a) == false);
        std::cout << error4a.message() << std::endl;

        std::istringstream input5(R"(
        .data
        var3: ..word 10
        )");
        TokenList tokens5 = tokenize(input5);
        std::cout << std::endl;
        ParseError error5 = parse(tokens5);
        REQUIRE(bool(error5) == true);
        std::cout << error5.message() << std::endl;

        std::istringstream input6(R"(
        .data
        var3: .ascii "hello"
        .asciiz hello
        )");
        TokenList tokens6 = tokenize(input6);
        std::cout << std::endl;
        ParseError error6 = parse(tokens6);
        REQUIRE(bool(error6) == true);
        std::cout << error6.message() << std::endl;


        std::istringstream input7(R"(
        ".data"
        var3: .ascii "hello"
        .asciiz hello
        )");
        TokenList tokens7 = tokenize(input7);
        std::cout << std::endl;
        ParseError error7 = parse(tokens7);
        REQUIRE(bool(error7) == true);
        std::cout << error7.message() << std::endl;

        std::istringstream input8(R"(
        .data var1:
        var3: .ascii "hello"
        .asciiz hello
        )");
        TokenList tokens8 = tokenize(input8);
        std::cout << std::endl;
        ParseError error8 = parse(tokens8);
        REQUIRE(bool(error8) == true);
        std::cout << error8.message() << std::endl;
        
        std::istringstream input9(R"(
        .data
        var3: .ascii
        
        )");
        TokenList tokens9 = tokenize(input9);
        std::cout << std::endl;
        ParseError error9 = parse(tokens9);
        REQUIRE(bool(error9) == true);
        std::cout << error9.message() << std::endl;
        

        std::istringstream input10(R"(
        .data
        .asciiz
        
        )");
        TokenList tokens10 = tokenize(input10);
        std::cout << std::endl;
        ParseError error10 = parse(tokens10);
        REQUIRE(bool(error10) == true);
        std::cout << error10.message() << std::endl;


        std::istringstream input11(R"(
        .data
        var1: .ascii
        
        )");
        TokenList tokens11 = tokenize(input11);
        std::cout << std::endl;
        ParseError error11 = parse(tokens11);
        REQUIRE(bool(error11) == true);
        std::cout << error11.message() << std::endl;

        std::istringstream input12(R"(
        .data
        A = = 10
        
        )");
        TokenList tokens12 = tokenize(input12);
        std::cout << std::endl;
        ParseError error12 = parse(tokens12);
        REQUIRE(bool(error12) == true);
        std::cout << error12.message() << std::endl;
        
        std::istringstream input13(R"(
        .data
        .ascii "hello
        .ascii hello"
        .ascii hello
        
        )");
        TokenList tokens13 = tokenize(input13);
        std::cout << std::endl;
        ParseError error13 = parse(tokens13);
        REQUIRE(bool(error13) == true);
        std::cout << error13.message() << std::endl;
        
    }
}

TEST_CASE("Parsing tests for special cases") {
    SECTION("test no data but empty text") {
        std::istringstream input(R"(
        .text
        )");
        TokenList tokens = tokenize(input);
        std::cout << std::endl;
        ParseError error = parse(tokens);
        std::cout << error.message() << std::endl;
        REQUIRE(!error);

    }

    SECTION("Test M4 example") {
        std::istringstream input(R"(
             .data
x:   .word 100
arr: .byte 10,11,12

     .text
main:
     # load word from location x into temporary register 0
     lw $t0, x

     # load address of arr into $t1
     la $t1, arr
     # and get first value (10) into $t2
     lb $t2, ($t1)
     # and get second value (11) into $t3
     lb $t3, 1($t1)
     # and get third value (12)into $t4
     lb $t4, 2($t1)
        )");
        TokenList tokens = tokenize(input);
        std::cout << std::endl;
        ParseError error = parse(tokens);
        std::cout << error.message() << std::endl;
        REQUIRE(!error);

    }

    SECTION("testing passing not") {
        std::istringstream input(R"(
        .data
        A = 10
        .text
        not $t1, $0
        not $5, 12
        not $zero, A
        )");
        TokenList tokens = tokenize(input);
        std::cout << std::endl;
        ParseError error = parse(tokens);
        std::cout << error.message() << std::endl;
        REQUIRE(!error);

    }

    SECTION("testing failing not") {
        std::istringstream input(R"(
        .data
        A = 10
        .text
        not $t1
        )");
        std::istringstream input1(R"(
        .data
        A = 10
        .text
        not $t1, B
        )");
        std::istringstream input2(R"(
        .data
        A = 10
        .text
        not 5, A
        )");
        std::istringstream input3(R"(
        .data
        A = 10
        .text
        not $32, A
        )");
        std::istringstream input4(R"(
        .data
        A = 10
        .text
        not $s1,
        )");
        std::istringstream input5(R"(
        .data
        A = 10
        .text
        not $t1 $t0
        )");
        std::istringstream input6(R"(
        .data
        A = 10
        .text
        not 12, A
        )");
        std::istringstream input7(R"(
        .data
        A = 10
        .text
        no $t0, $t1
        )");
        std::istringstream input8(R"(
        .data
        A = 10
        .text
        not $t0, 65536
        )");
        std::istringstream input9(R"(
        .data
        A = 10
        .text
        not $t0, +32768
        )");
        std::istringstream input10(R"(
        .data
        A = 10
        .text
        not $t0, -32769
        )");
        TokenList tokens = tokenize(input);
        TokenList tokens1 = tokenize(input1);
        TokenList tokens2 = tokenize(input2);
        TokenList tokens3 = tokenize(input3);
        TokenList tokens4 = tokenize(input4);
        TokenList tokens5 = tokenize(input5);
        TokenList tokens6 = tokenize(input6);
        TokenList tokens7 = tokenize(input7);
        TokenList tokens8 = tokenize(input8);
        TokenList tokens9 = tokenize(input9);
        TokenList tokens10 = tokenize(input10);
        std::cout << std::endl;
        ParseError error = parse(tokens);
        ParseError error1 = parse(tokens1);
        ParseError error2 = parse(tokens2);
        ParseError error3 = parse(tokens3);
        ParseError error4 = parse(tokens4);
        ParseError error5 = parse(tokens5);
        ParseError error6 = parse(tokens6);
        ParseError error7 = parse(tokens7);
        ParseError error8 = parse(tokens8);
        ParseError error9 = parse(tokens9);
        ParseError error10 = parse(tokens10);
        std::cout << error.message() << std::endl;
        std::cout << error1.message() << std::endl;
        std::cout << error2.message() << std::endl;
        std::cout << error3.message() << std::endl;
        std::cout << error4.message() << std::endl;
        std::cout << error5.message() << std::endl;
        std::cout << error6.message() << std::endl;
        std::cout << error7.message() << std::endl;
        std::cout << error8.message() << std::endl;
        std::cout << error9.message() << std::endl;
        std::cout << error10.message() << std::endl;
        REQUIRE(error);
        REQUIRE(error1);
        REQUIRE(error2);
        REQUIRE(error3);
        REQUIRE(error4);
        REQUIRE(error5);
        REQUIRE(error6);
        REQUIRE(error7);
        REQUIRE(error8);
        REQUIRE(error9);
        REQUIRE(error10);
    }

    SECTION("failing coverage tests") {
        std::istringstream input(R"(
        .data
        A = 10
        .text
        mfhi $1
        mflo $2
        mthi $3
        mtlo $4
        mfhi 12
        )");
        std::istringstream input1(R"(
        .data
        A = 10
        .text
        mfhi $t1 12
        )");
        std::istringstream input2(R"(
        .data
        A = 10
        .text
        nop 5
        )");
        std::istringstream input3(R"(
        .data
        A = 10
        .text
        main:
        j main
        j mainn
        )");
        std::istringstream input4(R"(
        .data
        A = 10
        .text
        lw $1, $32
        )");
        std::istringstream input5(R"(
        .data
        A = 10
        .text
        la $1 $3
        )");
        std::istringstream input6(R"(
        .data
        A = 10
        .text
        lw $1, $3, $2
        )");
        std::istringstream input7(R"(
        .data
        A = 10
        .text
        no $t0, $t1
        )");
        std::istringstream input8(R"(
        .data
        A = 10
        .text
        not $t0, 65536
        )");
        std::istringstream input9(R"(
        .data
        A = 10
        .text
        not $t0, +32768
        )");
        std::istringstream input10(R"(
        .data
        A = 10
        .text
        not $t0, -32769
        )");


        std::istringstream input11(R"(
        .data
        A = 10
        .text
        nooo $t1
        )");
        std::istringstream input12(R"(
        .data
        A = 10
        .text
        main:
        lw $100, $0
        )");
        std::istringstream input13(R"(
        .data
        A = 10
        .text
        li $100, 5
        )");
        std::istringstream input13a(R"(
        .data
        A = 10
        .text
        li $100, 5
        )");
        std::istringstream input13b(R"(
        .data
        A = 10
        .text
        li $t0 5
        )");
        std::istringstream input13c(R"(
        .data
        A = 10
        .text
        li $t0, 5,
        )");
        std::istringstream input14(R"(
        .data
        A = 10
        .text
        main:
        j main a
        )");
        std::istringstream input15(R"(
        .data
        A = 10
        .text
        move $100, $0
        )");
        std::istringstream input15a(R"(
        .data
        A = 10
        .text
        move $t0 $0
        )");
        std::istringstream input15b(R"(
        .data
        A = 10
        .text
        move $t0, $100
        )");
        std::istringstream input15c(R"(
        .data
        A = 10
        .text
        move $t0, $0,,
        )");
        std::istringstream input17(R"(
        .data
        A = 10
        .text
        add $100, $0, $0
        )");
        std::istringstream input17a(R"(
        .data
        A = 10
        .text
        add $t0 $0, $0
        )");
        std::istringstream input17c(R"(
        .data
        A = 10
        .text
        add $t0, $100, $0
        )");
        std::istringstream input17d(R"(
        .data
        A = 10
        .text
        add $t0, $0, $100
        )");
        std::istringstream input18(R"(
        .data
        A = 10
        .text
        main:
        beq $100, $0, main
        )");
        std::istringstream input18a(R"(
        .data
        A = 10
        .text
        main:
        beq $0 $0, main
        )");
        std::istringstream input18b(R"(
        .data
        A = 10
        .text
        main:
        beq $t0, $0 main
        )");
        std::istringstream input18c(R"(
        .data
        A = 10
        .text
        main:
        beq $0, $0, mainnn
        )");
        std::istringstream input18d(R"(
        .data
        A = 10
        .text
        main:
        beq $0, $0, main,
        )");
        std::istringstream input19(R"(
        .data
        A = 10
        .text
        div $100, $t0, $t0
        )");
        std::istringstream input19a(R"(
        .data
        A = 10
        .text
        div $t0 $t0, $t0
        )");
        std::istringstream input19b(R"(
        .data
        A = 10
        .text
        div $t0, $100, $t0
        )");
        std::istringstream input19c(R"(
        .data
        A = 10
        .text
        div $t0, $t0 $t0
        )");
        std::istringstream input19d(R"(
        .data
        A = 10
        .text
        div $t0, $t0, $100
        )");
        std::istringstream input19e(R"(
        .data
        A = 10
        .text
        div $t0, $t0, $t0,
        )");
        TokenList tokens = tokenize(input);
        TokenList tokens1 = tokenize(input1);
        TokenList tokens2 = tokenize(input2);
        TokenList tokens3 = tokenize(input3);
        TokenList tokens4 = tokenize(input4);
        TokenList tokens5 = tokenize(input5);
        TokenList tokens6 = tokenize(input6);
        TokenList tokens7 = tokenize(input7);
        TokenList tokens8 = tokenize(input8);
        TokenList tokens9 = tokenize(input9);
        TokenList tokens10 = tokenize(input10);
        TokenList tokens11 = tokenize(input11);
        TokenList tokens12 = tokenize(input12);
        TokenList tokens13 = tokenize(input13);
        TokenList tokens13a = tokenize(input13a);
        TokenList tokens13b = tokenize(input13b);
        TokenList tokens13c = tokenize(input13c);
        TokenList tokens14 = tokenize(input14);
        TokenList tokens15 = tokenize(input15);
        TokenList tokens15a = tokenize(input15a);
        TokenList tokens15b = tokenize(input15b);
        TokenList tokens15c = tokenize(input15c);
        TokenList tokens17 = tokenize(input17);
        TokenList tokens17a = tokenize(input17a);
        TokenList tokens17c = tokenize(input17c);
        TokenList tokens17d = tokenize(input17d);
        TokenList tokens18 = tokenize(input18);
        TokenList tokens18a = tokenize(input18a);
        TokenList tokens18b = tokenize(input18b);
        TokenList tokens18c = tokenize(input18c);
        TokenList tokens18d = tokenize(input18d);
        TokenList tokens19 = tokenize(input19);
        TokenList tokens19a = tokenize(input19a);
        TokenList tokens19b = tokenize(input19b);
        TokenList tokens19c = tokenize(input19c);
        TokenList tokens19d = tokenize(input19d);
        TokenList tokens19e = tokenize(input19e);
        std::cout << std::endl;
        ParseError error = parse(tokens);
        ParseError error1 = parse(tokens1);
        ParseError error2 = parse(tokens2);
        ParseError error3 = parse(tokens3);
        ParseError error4 = parse(tokens4);
        ParseError error5 = parse(tokens5);
        ParseError error6 = parse(tokens6);
        ParseError error7 = parse(tokens7);
        ParseError error8 = parse(tokens8);
        ParseError error9 = parse(tokens9);
        ParseError error10 = parse(tokens10);
        ParseError error11 = parse(tokens11);
        ParseError error12 = parse(tokens12);
        ParseError error13 = parse(tokens13);
        ParseError error13a = parse(tokens13a);
        ParseError error13b = parse(tokens13b);
        ParseError error13c = parse(tokens13c);
        ParseError error14 = parse(tokens14);
        ParseError error15 = parse(tokens15);
        ParseError error15a = parse(tokens15a);
        ParseError error15b = parse(tokens15b);
        ParseError error15c = parse(tokens15c);
        ParseError error17 = parse(tokens17);
        ParseError error17a = parse(tokens17a);
        ParseError error17c = parse(tokens17c);
        ParseError error17d = parse(tokens17d);
        ParseError error18 = parse(tokens18);
        ParseError error18a = parse(tokens18a);
        ParseError error18b = parse(tokens18b);
        ParseError error18c = parse(tokens18c);
        ParseError error18d = parse(tokens18d);
        ParseError error19 = parse(tokens19);
        ParseError error19a = parse(tokens19a);
        ParseError error19b = parse(tokens19b);
        ParseError error19c = parse(tokens19c);
        ParseError error19d = parse(tokens19d);
        ParseError error19e = parse(tokens19e);
        REQUIRE(error);
        REQUIRE(error1);
        REQUIRE(error2);
        REQUIRE(error3);
        REQUIRE(error4);
        REQUIRE(error5);
        REQUIRE(error6);
        REQUIRE(error7);
        REQUIRE(error8);
        REQUIRE(error9);
        REQUIRE(error10);
        REQUIRE(error11 );
        REQUIRE(error12 );
        REQUIRE(error13 );
        REQUIRE(error13a );
        REQUIRE(error13b );
        REQUIRE(error13c );
        REQUIRE(error14 );
        REQUIRE(error15 );
        REQUIRE(error15a );
        REQUIRE(error15b );
        REQUIRE(error15c );
        REQUIRE(error17 );
        REQUIRE(error17a );
        REQUIRE(error17c );
        REQUIRE(error17d );
        REQUIRE(error18 );
        REQUIRE(error18a );
        REQUIRE(error18b );
        REQUIRE(error18c );
        REQUIRE(error18d );
        REQUIRE(error19 );
        REQUIRE(error19a );
        REQUIRE(error19b );
        REQUIRE(error19c );
        REQUIRE(error19d );
        REQUIRE(error19e );
    }
}

TEST_CASE("Parser specific function tests") {
    SECTION("is_valid_reg") {
        std::istringstream input(R"(
        .data
        ABC: .half 12
        main:
        mfhi $$1
        )");
        TokenList tokens = tokenize(input);
        ParseError error = parse(tokens);
        REQUIRE(error);

        std::istringstream input1(R"(
        .data
        ABC: .half 12
        main:
        mfhi 
        )");
        TokenList tokens1 = tokenize(input1);
        ParseError error1 = parse(tokens1);
        REQUIRE(error1);
    }
}




