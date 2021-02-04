#include <array>
#include <vector>
#include <algorithm>
#include <iostream>
#include <conio.h>
#include <windows.h>
#include "MOS6502.h"

namespace Simulator {

class CpuTester : public Memory
{
public:
	explicit CpuTester(const std::vector<uint8_t>& code) :
		m_cpu(*this)
	{
		std::copy(code.begin(), code.end(), m_memory.begin() + 10);
		m_memory[0xfffc] = 0x00;
		m_memory[0xfffd] = 0x04;
	}

	uint8_t Read(uint16_t address) override
	{
		if (address == 0xf004)
		{
			auto ch = _getch();
			if (ch == 3 || ch == 27 || ch == 'q' || ch == 'Q')
				m_stop = true;
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

	void Run(bool trace)
	{
		m_cpu.Reset(true);
		m_cpu.Step();
		m_cpu.Reset(false);

		for (m_stop = false; !m_stop; )
		{
			if (trace)
			//	OutputDebugStringA((ShowRegisters(cpu) + " " + Disassemble(memory, cpu.PC()) + "\n").c_str());
				std::cout << ShowRegisters(m_cpu) << " " << Disassemble(*this, m_cpu.PC()) << "\n";
			m_cpu.Step();
		}

		std::cout << m_cpu.Cycles() << " CPU cycles, " << m_cpu.Cycles() / 2e6 << " s\n";
	}

private:
	std::array<uint8_t, 64 * 1024> m_memory;
	MOS6502 m_cpu;
	bool m_stop;
};

void TestCpu(bool trace)
{
	CpuTester tester(Load("6502_functional_test.bin"));
	tester.Run(trace);
}

} // namespace Simulator
