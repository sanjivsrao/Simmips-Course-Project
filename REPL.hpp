#ifndef REPL_H
#define REPL_H

#include "VirtualMachine.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

namespace simmips {
    class REPL {
    public:
        REPL(VirtualMachine& vm) : vm_(vm), stop(false){}
        void runREPL();

    private:

        VirtualMachine& vm_;
        std::atomic<bool> stop;

        // Simple function to display memory values in proper hex format
        std::string hex_convert(uint32_t value);

        // Consumer method for handling commands in queue
        void runLoop();
    };
}
#endif
