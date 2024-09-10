#include "catch.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include "Lexer.hpp"
#include "Token.hpp"
#include "test_config.hpp"

using namespace simmips;

TEST_CASE("Tokenize function base student tests") {
    SECTION("Testing empty input stream") {
        std::istringstream input(" \"\" ");
        TokenList tokens = simmips::tokenize(input);
        REQUIRE(tokens.size() == 4);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Testing parenthesis inputs") {
        std::istringstream input1("yo(test 123 )(hello)world");
        std::istringstream input2("yo(test(");
        std::istringstream input3("yo((test)");
        std::istringstream input4("yo(test))");
        std::istringstream input5("yo \"(hello))\" ");
        std::istringstream input6(R"(Hello"
        Xyz))");
        std::istringstream input7("Xyz)");
        std::istringstream input8("xyz(");
        TokenList tokens1 = simmips::tokenize(input1);
        std::cout << std::endl;
        TokenList tokens2 = simmips::tokenize(input2);
        std::cout << std::endl;
        TokenList tokens3 = simmips::tokenize(input3);
        std::cout << std::endl;
        TokenList tokens4 = simmips::tokenize(input4);
        std::cout << std::endl;
        TokenList tokens5 = simmips::tokenize(input5);
        std::cout << std::endl;
        TokenList tokens6 = simmips::tokenize(input6);
        std::cout << std::endl;
        TokenList tokens7 = simmips::tokenize(input7);
        std::cout << std::endl;
        TokenList tokens8 = simmips::tokenize(input8);
        std::cout << std::endl;
        REQUIRE(tokens1.size() == 10);
        REQUIRE(tokens2.size() == 5);
        REQUIRE(tokens3.size() == 6);
        REQUIRE(tokens4.size() == 5);
        REQUIRE(tokens5.size() == 5);
        REQUIRE(tokens6.size() == 3);
        REQUIRE(tokens7.size() == 2);
        REQUIRE(tokens8.size() == 3);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }

    SECTION("Testing semi-empty string") {
        std::istringstream input(" a\"\" ");
        TokenList tokens = simmips::tokenize(input);
        REQUIRE(tokens.size() == 5);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }

    SECTION("Tokenize input stream with single string") {
        std::istringstream input("Apple");
        TokenList tokens = simmips::tokenize(input);
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens.front().type() == TokenType::STRING);
        REQUIRE(tokens.front().value() == "Apple");
        REQUIRE(tokens.front().line() == 1);
        REQUIRE(tokens.back().type() == TokenType::EOL);
        REQUIRE(tokens.back().value() == "");
        REQUIRE(tokens.back().line() == 1);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }

    SECTION("Tokenize input stream with project description 1") {
        std::istringstream input(R"(
        .data
        LENGTH = 10
        array:  .space LENGTH #this is a comment, code should be unaffected
        str:    .asciiz "the (end) "
        .text
        main : lw $t0, array
        lw $t1, ($t0)
        )");
        TokenList tokens = tokenize(input);
        REQUIRE(tokens.size() == 32);
        REQUIRE(tokens.front().type() == STRING);
        REQUIRE(tokens.front().value() == ".data");
        REQUIRE(tokens.back().type() == EOL);
        REQUIRE(tokens.back().value() == "");
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }

    SECTION("Tokenize input stream with project description 2") {
        std::istringstream input(R"(
        .text
        lw $t0, ($t1
        )
        )");
        TokenList tokens = tokenize(input);
        REQUIRE(tokens.size() == 7);
        REQUIRE(tokens.front().type() == STRING);
        REQUIRE(tokens.front().value() == ".text");
        REQUIRE(tokens.back().type() == ERROR);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }

    SECTION("Testing equal token after string") {
        std::istringstream input("Hello=world");
        TokenList tokens = simmips::tokenize(input);
        REQUIRE(tokens.size() == 4);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }

    SECTION("Testing comment token directly after string") {
        std::istringstream input("Hello_World# Hey world, I'm dad");
        TokenList tokens = simmips::tokenize(input);
        REQUIRE(tokens.size() == 2);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Testing empty string") {
        std::istringstream input(R"(
        x: .ascii "" apple
        )");
        TokenList tokens = simmips::tokenize(input);
        REQUIRE(tokens.size() == 7);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
}

TEST_CASE("Tokenize function special case tests") {
    SECTION("Testing failing case") {
        std::istringstream input(R"(
        A, ($t1), ail # This is a comment
        Parenthesis (Hello #)
        )");
        TokenList tokens = simmips::tokenize(input);
        REQUIRE(tokens.size() == 12);
        REQUIRE(tokens.front().line() == 2);
        REQUIRE(tokens.back().type() == ERROR);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize special edge cases") {
        std::istringstream input(R"(
        .text
        abcd"
        )");
        TokenList tokens = tokenize(input);
        REQUIRE(tokens.size() == 5);
        REQUIRE(tokens.front().type() == STRING);
        REQUIRE(tokens.front().value() == ".text");
        REQUIRE(tokens.back().type() == ERROR);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize unmatched close paren") {
        std::istringstream input(R"(
        .text
        abcd )34)
        )");
        TokenList tokens = tokenize(input);
        REQUIRE(tokens.size() == 4);
        REQUIRE(tokens.front().type() == STRING);
        REQUIRE(tokens.front().value() == ".text");
        REQUIRE(tokens.back().type() == ERROR);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize multiple parenthesis") {
        std::istringstream input(R"(
        .text
        abcd (4(5))
        )");
        TokenList tokens = tokenize(input);
        REQUIRE(tokens.size() == 10);
        REQUIRE(tokens.front().type() == STRING);
        REQUIRE(tokens.front().value() == ".text");
        REQUIRE(tokens.back().type() == EOL);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize parenthesis with errored comment") {
        std::istringstream input(R"(
        abcd (4(#
        )");
        TokenList tokens = tokenize(input);
        REQUIRE(tokens.size() == 5);
        REQUIRE(tokens.front().type() == STRING);
        REQUIRE(tokens.front().value() == "abcd");
        REQUIRE(tokens.back().type() == ERROR);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize parenthesis spaces") {
        std::istringstream input(R"(
        abcd (4 5 6)
        )");
        TokenList tokens = tokenize(input);
        REQUIRE(tokens.size() == 7);
        REQUIRE(tokens.front().type() == STRING);
        REQUIRE(tokens.front().value() == "abcd");
        REQUIRE(tokens.back().type() == EOL);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }

    SECTION("Tokenize equals sign and comment") {
        std::istringstream input(R"(
        a=5#this is my variable
        )");
        TokenList tokens = tokenize(input);
        REQUIRE(tokens.size() == 4);
        REQUIRE(tokens.front().type() == STRING);
        REQUIRE(tokens.front().value() == "a");
        REQUIRE(tokens.back().type() == EOL);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }

}

TEST_CASE("Tokenize function passing instructor tests") {
    SECTION("Tokenize input stream with test file 0") {
        std::ifstream input(TEST_FILE_DIR + "/pass/win/test00.asm");
        if (input.is_open()) {
            std::cout << "File 0 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        REQUIRE(tokens.size() == 50);
        REQUIRE(tokens.front().line() == 2);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }

    SECTION("Tokenize input stream with test file 1") {
        std::ifstream input(TEST_FILE_DIR + "/pass/win/test01.asm");
        if (input.is_open()) {
            std::cout << "File 1 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        REQUIRE(tokens.size() == 12);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }

    SECTION("Tokenize input stream with test file 2") {
        std::ifstream input(TEST_FILE_DIR + "/pass/win/test02.asm");
        if (input.is_open()) {
            std::cout << "File 2 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        REQUIRE(tokens.size() == 58);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }

    SECTION("Tokenize input stream with passing test file 3") {
        std::ifstream input(TEST_FILE_DIR + "/pass/win/test03.asm");
        if (input.is_open()) {
            std::cout << "File 3 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        REQUIRE(tokens.size() == 217);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with passing test file 4") {
        std::ifstream input(TEST_FILE_DIR + "/pass/win/test04.asm");
        if (input.is_open()) {
            std::cout << "File 4 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        REQUIRE(tokens.size() == 22+20+56);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with passing test file 5") {
        std::ifstream input(TEST_FILE_DIR + "/pass/win/test05.asm");
        if (input.is_open()) {
            std::cout << "File 5 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        REQUIRE(tokens.size() == 83);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with passing test file 6") {
        std::ifstream input(TEST_FILE_DIR + "/pass/win/test06.asm");
        if (input.is_open()) {
            std::cout << "File 6 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        REQUIRE(tokens.size() == 69);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with passing test file 7") {
        std::ifstream input(TEST_FILE_DIR + "/pass/win/test07.asm");
        if (input.is_open()) {
            std::cout << "File 7 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        REQUIRE(tokens.size() == 321);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
    SECTION("Tokenize input stream with passing test file 8") {
        std::ifstream input(TEST_FILE_DIR + "/pass/win/test08.asm");
        if (input.is_open()) {
            std::cout << "File 8 accessible" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        REQUIRE(tokens.size() == 18+10+7+32);
        std::cout << std::endl << "Test Complete!\n" << std::endl;
    }
}

TEST_CASE("Tokensize function failing instructor tests") {
    SECTION("Tokenize input stream with failing test file 1") {
        std::ifstream input(TEST_FILE_DIR + "/fail/win/test01.asm");
        if (input.is_open()) {
            std::cout << "File accessible" << std::endl;
        }
        else {
            std::cout << "File was not able to be accessed" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        REQUIRE(tokens.size() == 5);
        REQUIRE(tokens.back().type() == TokenType::EOL);
        std::cout << std::endl;
    }

    SECTION("Tokenize input stream with failing test file 2") {
        std::ifstream input(TEST_FILE_DIR + "/fail/win/test02.asm");
        if (input.is_open()) {
            std::cout << "File accessible" << std::endl;
        }
        else {
            std::cout << "File was not able to be accessed" << std::endl;
        }
        TokenList tokens = simmips::tokenize(input);
        REQUIRE(tokens.size() == 25);
        std::cout << std::endl;
    }
}



