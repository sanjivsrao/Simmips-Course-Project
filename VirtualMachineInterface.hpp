#ifndef VIRTUALMACHINEINTERFACE_HPP
#define VIRTUALMACHINEINTERFACE_HPP

#include <cstdlib>
#include <string>
#include <cstdint>

namespace simmips {

enum RegisterKind{
  Register00,
  Register01,
  Register02,
  Register03,
  Register04,
  Register05,
  Register06,
  Register07,
  Register08,
  Register09,
  Register10,
  Register11,
  Register12,
  Register13,
  Register14,
  Register15,
  Register16,
  Register17,
  Register18,
  Register19,
  Register20,
  Register21,
  Register22,
  Register23,
  Register24,
  Register25,
  Register26,
  Register27,
  Register28,
  Register29,
  Register30,
  Register31,
  RegisterPC,
  RegisterHi,
  RegisterLo,
  RegisterUnknown
};

class VirtualMachineInterface{
public:

  // return the memory size of the virtual
  // machine in bytes
  virtual std::size_t getMemorySize() const noexcept = 0;

  // return the number of MIPS assembly instructions
  virtual std::size_t getInstructionSize() const noexcept = 0;

  // return the value at address as unsigned int
  // throws std::range_error if address invalid
  virtual uint8_t getMemoryByte(uint32_t address) const = 0;

  // return the value in register as unsigned int
  virtual uint32_t getRegister(RegisterKind reg) const noexcept = 0;
  
  // return the current source line in the original
  // assembly input
  virtual uint32_t getCurrentSourceLine() const noexcept = 0;

  // return true if the machine is in an error state
  // else false
  virtual bool isError() const noexcept = 0;

  // return the error state of the virtual machine
  // returns empty string if not in an error state
  // (isError() returns false)
  virtual std::string getError() const noexcept = 0;

  // step the virtual machine one instruction, updating
  // the simulated memory, registers, program counter,
  // and error state. If the machine is in an error state
  // this method has no effect.
  virtual void step() = 0;
};

}

#endif
