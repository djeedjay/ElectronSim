// (C) Copyright Gert-Jan de Vos 2021.

#include <array>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <conio.h>
#include "DjeeDjay/MOS6502.h"

namespace DjeeDjay {

class CpuTester : public Memory
{
public:
	explicit CpuTester(const std::vector<uint8_t>& code) :
		m_cpu(*this)
	{
		if (code.size() > 0x10000 - 10)
			throw std::invalid_argument("Image file too large");
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
			return static_cast<uint8_t>(ch);
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

std::vector<uint8_t> Load(const std::string& path)
{
	std::vector<uint8_t> data;
	std::ifstream fs(path, std::ios::binary);
	std::copy(std::istreambuf_iterator<char>(fs), std::istreambuf_iterator<char>(), std::back_inserter(data));
	return data;
}

void TestCpu(const char* filename, bool trace)
{
	CpuTester tester(Load(filename));
	tester.Run(trace);
}

void Syntax()
{
	std::cout << "Syntax: CpuTest [--trace] <ImageFile>\n";
}

} // namespace DjeeDjay

int main(int /*argc*/, char* argv[])
try
{
	using namespace DjeeDjay;

	bool trace = false;
	const char* filename = nullptr;

	while (*++argv)
	{
		if (argv[0] == std::string("--trace"))
			trace = true;
		else
			filename = argv[0];
	}

	if (filename)
		TestCpu(filename, trace);
	else
		Syntax();
}
catch (DjeeDjay::MemoryReadError& ex)
{
	std::cerr << ex.what() << " at " << std::hex << ex.address << "\n";
	return EXIT_FAILURE;
}
catch (DjeeDjay::MemoryWriteError& ex)
{
	std::cerr << ex.what() << " at " << std::hex << ex.address << "\n";
	return EXIT_FAILURE;
}
catch (DjeeDjay::InvalidOpcodeError& ex)
{
	std::cerr << ex.what() << ": " << std::hex << +ex.opcode << "\n";
	return EXIT_FAILURE;
}
catch (std::exception& ex)
{
	std::cerr << ex.what() << "\n";
	return EXIT_FAILURE;
}
