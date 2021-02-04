#include <array>
#include <vector>
#include <algorithm>
#include <iostream>
#include <conio.h>
#include <windows.h>
#include "MOS6502.h"

namespace Simulator {

class TestMemory : public Memory
{
public:
	explicit TestMemory(const std::vector<uint8_t>& code)
	{
		std::copy(code.begin(), code.end(), m_memory.begin() + 10);
	}

	uint8_t Read(uint16_t address) override
	{
		if (address == 0xf004)
		{
			auto ch = _getch();
			if (ch == 3 || ch == 27 || ch == 'q' || ch == 'Q')
				std::exit(0);
			return ch;
		}
		return m_memory[address];
	}

	void Write(uint16_t address, uint8_t value) override
	{
		if (address == 0xf001)
		{
			putchar(value);
		}
		m_memory[address] = value;
	}

private:
	std::array<uint8_t, 64 * 1024> m_memory;
};

void TestCpu(bool trace)
{
	TestMemory memory(Load("6502_functional_test.bin"));
	memory.Write(0xfffc, 0x00);
	memory.Write(0xfffd, 0x04);

	MOS6502 cpu(memory);
	cpu.Reset(true);
	cpu.Step();
	cpu.Reset(false);

	for (;;)
	{
		if (trace)
		//	OutputDebugStringA((ShowRegisters(cpu) + " " + Disassemble(memory, cpu.PC()) + "\n").c_str());
			std::cout << ShowRegisters(cpu) << " " << Disassemble(memory, cpu.PC()) << "\n";
		cpu.Step();
	}
}

} // namespace Simulator
