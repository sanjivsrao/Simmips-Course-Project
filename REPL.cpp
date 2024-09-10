#include "REPL.hpp"
#include <iomanip>
#include <sstream>

using namespace simmips;

void REPL::runREPL() {
    std::string command;
    std::thread runThread;
    bool threadRunning = false;
    while (true) {
        std::cout << "simmips> ";
        std::getline(std::cin, command);
        if (threadRunning && command != "break") {
            std::cout << "Error: simulation running.Type break to halt" << std::endl;
            continue;
        }
        if (command.substr(0, 5) == "print") {
            std::string arg;
            arg = command.substr(6);
            if (arg[0] == '$') {
                RegisterKind reg = vm_.identifyRegister(arg);
                if (reg == RegisterUnknown) {
                    std::cerr << "Error:0: Register is unknown" << std::endl;
                }
                else {
                    uint32_t regValue = vm_.getRegister(reg);
                    std::cout << hex_convert(regValue) << std::endl;
                }
            }
            else if (arg[0] == '&') {
                uint32_t address = std::stoi(arg.substr(1), nullptr, 16);
                uint8_t byteValue = vm_.getMemoryByte(address);
                std::cout << "0x" << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(byteValue) << std::endl;
            }
        }
        else if (command == "step") {
            vm_.step();
            std::cout << hex_convert(vm_.getRegister(simmips::RegisterPC)) << std::endl;
        }
        else if (command == "run") {
            if (!runThread.joinable()) {
                threadRunning = true;
                stop = false;
                runThread = std::thread(&REPL::runLoop, this);
            }
        }
        else if (command == "status") {
            if (vm_.isError()) {
                std::cout << vm_.getError() << std::endl;
            }
        }
        else if (command == "break") {
            if (runThread.joinable()) {
                threadRunning = false;
                stop = true;
                runThread.join();
            }
        }
        else if (command == "quit") {
            break;
        }
        else {
            std::cout << "Error: unknown command." << std::endl;
        }
    }
}

std::string REPL::hex_convert(uint32_t value) {
    std::stringstream ss;
    ss << "0x" << std::setfill('0') << std::setw(8) << std::hex << value;
    return ss.str();
}

void simmips::REPL::runLoop()
{
    while (!stop) {
        if (vm_.getCurrentSourceLine() < vm_.getInstructionSize()) {
            vm_.step();
        }
    }
}

