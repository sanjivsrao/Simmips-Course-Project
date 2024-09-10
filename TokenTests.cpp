#include "catch.hpp"

#include "Token.hpp"
#include <sstream>


TEST_CASE("Test initializing token and token list", "[Token]") {
  simmips::Token t1(simmips::TokenType::EOL, 1);
  simmips::TokenList lst;
  REQUIRE(lst.empty());
  REQUIRE(lst.size() == 0);
  lst.push_back(t1);
  REQUIRE_FALSE(lst.empty());
  lst.emplace_back(simmips::TokenType::SEP, 1);
  REQUIRE(lst.size() == 2);
  // note this should not compile
  // simmips::Token t2(simmips::EOL, 1);
}

TEST_CASE("Test Token constructors and rendering", "[Token]") {
  using namespace simmips;
 
  {
    Token t(TokenType::EOL, 100);

    REQUIRE(t.type() == TokenType::EOL);
    REQUIRE(t.line() == 100);
    REQUIRE(t.value().empty());
 
    std::ostringstream oss;
    oss << t;

    REQUIRE(oss.str() == "Token: type(EOL) value () source line (100)");
  }

  {
    Token t(TokenType::SEP, 1);

    REQUIRE(t.type() == TokenType::SEP);
    REQUIRE(t.line() == 1);
    REQUIRE(t.value().empty());

    std::ostringstream oss;
    oss << t;

    REQUIRE(oss.str() == "Token: type(SEP) value () source line (1)");
  }

  {
    Token t(TokenType::EQUAL, 2);

    REQUIRE(t.type() == TokenType::EQUAL);
    REQUIRE(t.line() == 2);
    REQUIRE(t.value().empty());

    std::ostringstream oss;
    oss << t;

    REQUIRE(oss.str() == "Token: type(EQUAL) value () source line (2)");
  }

  {
    Token t(TokenType::OPEN_PAREN, 18);

    REQUIRE(t.type() == TokenType::OPEN_PAREN);
    REQUIRE(t.line() == 18);
    REQUIRE(t.value().empty());

    std::ostringstream oss;
    oss << t;

    REQUIRE(oss.str() == "Token: type(OPEN_PAREN) value () source line (18)");
  }

  {
    Token t(TokenType::CLOSE_PAREN, 18);

    REQUIRE(t.type() == TokenType::CLOSE_PAREN);
    REQUIRE(t.line() == 18);
    REQUIRE(t.value().empty());

    std::ostringstream oss;
    oss << t;

    REQUIRE(oss.str() == "Token: type(CLOSE_PAREN) value () source line (18)");
  }

  {
    Token t(TokenType::STRING_DELIM, 42);

    REQUIRE(t.type() == TokenType::STRING_DELIM);
    REQUIRE(t.line() == 42);
    REQUIRE(t.value().empty());

    std::ostringstream oss;
    oss << t;

    REQUIRE(oss.str() == "Token: type(STRING_DELIM) value () source line (42)");
  }

  {
    Token t(TokenType::STRING, 34, "thestring");

    REQUIRE(t.type() == TokenType::STRING);
    REQUIRE(t.line() == 34);
    REQUIRE(t.value() == "thestring");

    std::ostringstream oss;
    oss << t;

    REQUIRE(oss.str() ==
            "Token: type(STRING) value (thestring) source line (34)");
  }

  {
    Token t(TokenType::ERROR, 1, "The Error Message");

    REQUIRE(t.type() == TokenType::ERROR);
    REQUIRE(t.line() == 1);
    REQUIRE(t.value() == "The Error Message");

    std::ostringstream oss;
    oss << t;

    REQUIRE(oss.str() ==
            "Token: type(ERROR) value (The Error Message) source line (1)");
  }

}

TEST_CASE("Test Token const correctness", "[Token]") {
  using namespace simmips;
 
  const Token t(TokenType::EOL, 1);
  
  REQUIRE(t.type() == TokenType::EOL);
  REQUIRE(t.line() == 1);
  REQUIRE(t.value().empty());
  
  std::ostringstream oss;
  oss << t;
  
  REQUIRE(oss.str() == "Token: type(EOL) value () source line (1)");

}

TEST_CASE("Test Token comparison", "[Token]") {
  using namespace simmips;
 
  Token t1(TokenType::OPEN_PAREN, 18);
  Token t2(TokenType::OPEN_PAREN, 17);
  Token t3(TokenType::STRING, 1, "thevalue");
  Token t4(TokenType::STRING, 2, "thevalue");
  Token t5(TokenType::STRING, 1, "avalue");

  REQUIRE(t1 == t1);
  REQUIRE(t2 == t2);
  REQUIRE(t3 == t3);
  REQUIRE(t4 == t4);
  REQUIRE(t5 == t5);

  REQUIRE(t1 != t2);
  REQUIRE(t3 != t4);
  REQUIRE(t3 != t5);
}

TEST_CASE("Test output for unknown type") {

}

