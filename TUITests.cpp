#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "REPL.hpp"
#include <sstream>

// TA suggested I test each member function individually for code coverage later

TEST_CASE("REPL Run Test") {
    std::stringstream input_stream;
    std::stringstream output_stream;
    simmips::VirtualMachine vm;
    simmips::REPL repl(vm);

    SECTION("Print Command") {
        input_stream.str("print &0x00000000\n"); 
        std::streambuf* old_cin = std::cin.rdbuf(input_stream.rdbuf()); // Redirect cin to input_stream
        std::streambuf* old_cout = std::cout.rdbuf(output_stream.rdbuf()); // Redirect cout to output_stream
        repl.runREPL(); // Start the REPL loop (interactively)
        std::string output = output_stream.str();
        REQUIRE(output == "0x00\n"); // Assuming expected output for the provided input
        std::cin.rdbuf(old_cin); // Reset cin redirection
        std::cout.rdbuf(old_cout); // Reset cout redirection
    }
}

