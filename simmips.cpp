#include "VirtualMachine.hpp"
#include "Parser.hpp"
#include "REPL.hpp"
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>


bool parseInputFile(const std::string& filename, std::pair<simmips::ParseError, simmips::VirtualMachine>& output, std::string& error_msg) {
    
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        error_msg = "Error:0: Unable to open file: " + filename;
        return false;
    }
    simmips::TokenList tokens = simmips::tokenize(file);
    output = simmips::parseVM(tokens);
    if (output.first) {
        error_msg = output.first.message();
        return false;
    }

    file.close();
    return true;
}

int main(int argc, char*argv[])
{
    if (argc != 2) {
        std::cerr << "Error:0: Incorrect number of arguments." << std::endl;
        return EXIT_FAILURE;
    }
    std::vector<std::string>  arguments;
    for (int i = 0; i < argc; ++i) arguments.emplace_back(argv[i]);

    std::pair<simmips::ParseError, simmips::VirtualMachine> output;
    std::string error_msg; 

    if (!parseInputFile(argv[1], output, error_msg)) {
        std::cerr << error_msg << std::endl;
        return EXIT_FAILURE;
    }

    simmips::REPL repl(output.second);
    repl.runREPL();

    return EXIT_SUCCESS;
}
