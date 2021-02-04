#include <cstdint>
#include <stdexcept>
#include <vector>

#pragma once

namespace Simulator {

class MOS6502;

struct MemoryReadError : std::runtime_error
{
	explicit MemoryReadError(uint16_t address);

	uint16_t address;
};

struct MemoryWriteError : std::runtime_error
{
	explicit MemoryWriteError(uint16_t address);

	uint16_t address;
};

struct InvalidOpcodeError : std::runtime_error
{
	explicit InvalidOpcodeError(uint8_t opcode);

	uint8_t opcode;
};

struct Memory
{
	virtual uint8_t Read(uint16_t offset) = 0;
	virtual void Write(uint16_t offset, uint8_t value) = 0;
};

std::vector<uint8_t> Load(const std::string& path);

std::string ShowRegisters(const MOS6502& cpu);
std::string Disassemble(Memory& memory, uint16_t addr);

void TestCpu(bool trace);

} // namespace Simulator
