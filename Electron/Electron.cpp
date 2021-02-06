#include <array>
#include <vector>
#include <string>
#include <stdexcept>
#include <iterator>
#include <algorithm>
#include <fstream>
#include <iostream>
#include "DjeeDjay/MOS6502.h"

namespace DjeeDjay {

std::vector<uint8_t> Load(const std::string& path)
{
	std::vector<uint8_t> data;
	std::ifstream fs(path, std::ios::binary);
	std::copy(std::istreambuf_iterator<char>(fs), std::istreambuf_iterator<char>(), std::back_inserter(data));
	return data;
}

class ElectronMemory : public Memory
{
public:
	explicit ElectronMemory(const std::vector<uint8_t>& os) :
		m_irqStatus(UnusedMask | PowerOnReset),
		m_irqEnable(0),
		m_romBankIndex(0)
	{
		if (os.size() != 0x4000)
			throw std::runtime_error("Bad ROM size");
		std::copy(os.begin(), os.end(), m_os.begin());
	}

	void InstallRom(const std::vector<uint8_t>& rom)
	{
		if (rom.size() != 0x4000)
			throw std::runtime_error("Bad ROM size");
		std::copy(rom.begin(), rom.end(), m_romBank.begin());
	}

	uint8_t Read(uint16_t address) override
	{
		if (address < 0x8000)
			return m_ram[address];
		else if (address < 0xc000)
			return m_romBank[address - 0x8000];
		else if (address >= 0xfe00 && address < 0xff00)
			return IORead(address);
		else
			return m_os[address - 0xc000];
	}

	void Write(uint16_t address, uint8_t value) override
	{
		if (address < 0x8000)
			m_ram[address] = value;
		else if (address >= 0xfe00 && address < 0xff00)
			return IOWrite(address, value);
		else
			std::cout << "Invalid write " << std::hex << address << ", " << +value << "\n";
	}

	uint8_t IORead(uint16_t address)
	{
		switch (address & 0xff0f)
		{
		case 0xfe00: return InterruptStatus();
		case 0xfe04: return CassetteDataShift();
		case 0xfe07: return MiscellaneousControl();
		case 0xfe08: return Palette(0);
		case 0xfe09: return Palette(1);
		case 0xfe0a: return Palette(2);
		case 0xfe0b: return Palette(3);
		case 0xfe0c: return Palette(4);
		case 0xfe0d: return Palette(5);
		case 0xfe0e: return Palette(6);
		case 0xfe0f: return Palette(7);
		}

		std::cout << "IO Read " << std::hex << address << "\n";
		std::exit(0);
		return 0;
	}

	void IOWrite(uint16_t address, uint8_t value)
	{
		switch (address & 0xff0f)
		{
		case 0xfe00: return InterruptStatus(value);
		case 0xfe02: return ScreenLow(value);
		case 0xfe03: return ScreenHigh(value);
		case 0xfe04: return CassetteDataShift(value);
		case 0xfe05: return InterruptClearAndPaging(value);
		case 0xfe06: return Counter(value);
		case 0xfe07: return MiscellaneousControl(value);
		case 0xfe08: return Palette(0, value);
		case 0xfe09: return Palette(1, value);
		case 0xfe0a: return Palette(2, value);
		case 0xfe0b: return Palette(3, value);
		case 0xfe0c: return Palette(4, value);
		case 0xfe0d: return Palette(5, value);
		case 0xfe0e: return Palette(6, value);
		case 0xfe0f: return Palette(7, value);
		}

		std::cout << "IO Write " << std::hex << address << ", " << +value << "\n";
		std::exit(0);
	}

	uint8_t InterruptStatus()
	{
		auto irqStatus = m_irqStatus;
		m_irqStatus &= ~PowerOnReset;
		return irqStatus;
	}

	void InterruptStatus(uint8_t value)
	{
		m_irqEnable = value;
	}

	void ScreenLow(uint8_t value)
	{
		m_screenLow = value;
	}

	void ScreenHigh(uint8_t value)
	{
		m_screenHigh = value;
	}

	uint8_t CassetteDataShift()
	{
		return 0;
	}

	void CassetteDataShift(uint8_t)
	{
	}

	void InterruptClearAndPaging(uint8_t value)
	{
		m_romBankIndex = value & 0x0f;
		if (value & 0x80)
			m_nmi = false;
		if (value & 0x40)
			m_irqEnable = m_irqEnable & ~HighToneDetect;
		if (value & 0x20)
			m_irqEnable = m_irqEnable & ~RealTimeClock;
		if (value & 0x10)
			m_irqEnable = m_irqEnable & ~DisplayEnd;
	}

	void Counter(uint8_t)
	{
	}

	uint8_t MiscellaneousControl()
	{
		return m_miscControl;
	}

	void MiscellaneousControl(uint8_t value)
	{
		m_miscControl = value;
	}

	uint8_t Palette(int index)
	{
		return m_palette[index];
	}

	void Palette(int index, uint8_t value)
	{
		m_palette[index] = value;
	}

private:
	static constexpr uint8_t UnusedMask = 0x80;
	static constexpr uint8_t HighToneDetect = 0x40;
	static constexpr uint8_t ReceiveDataFull = 0x20;
	static constexpr uint8_t TransmitDataEmpty = 0x10;
	static constexpr uint8_t RealTimeClock = 0x08;
	static constexpr uint8_t DisplayEnd = 0x04;
	static constexpr uint8_t PowerOnReset = 0x02;
	static constexpr uint8_t MasterIrq = 0x01;

	std::array<uint8_t, 0x8000> m_ram;
	std::array<uint8_t, 0x4000> m_romBank;
	std::array<uint8_t, 0x4000> m_os;

	bool m_nmi;
	uint8_t m_irqStatus;
	uint8_t m_irqEnable;
	uint8_t m_screenLow;
	uint8_t m_screenHigh;
	uint8_t m_romBankIndex;
	uint8_t m_miscControl;
	uint8_t m_palette[8];
};

namespace {

void WriteU1(std::ostream& os, uint8_t value)
{
	os.put(value);
}

void WriteU2(std::ostream& os, uint16_t value)
{
	os.put(value & 0xFF);
	os.put(value >> 8);
}

void WriteU4(std::ostream& os, uint32_t value)
{
	os.put(value & 0xFF);
	os.put((value >> 8) & 0xFF);
	os.put((value >> 16) & 0xFF);
	os.put((value >> 24) & 0xFF);
}

const int BI_RGB = 0;

int GetLinePaddingSize(int lineSize)
{
	return (4 - lineSize % 4) % 4;
}

int GetLinePaddingSize(int width, int bits)
{
	return GetLinePaddingSize(width * (bits / 8));
}

unsigned GetBmpLineSize(int width, int bits)
{
	return width * (bits / 8) + GetLinePaddingSize(width, bits);
}

unsigned BmpDataSize(int width, int height, int bits)
{
	return std::abs(height) * GetBmpLineSize(width, bits);
}

void WriteRgbQuad(std::ostream& os, uint8_t r, uint8_t g, uint8_t b)
{
	WriteU1(os, b);
	WriteU1(os, g);
	WriteU1(os, r);
	WriteU1(os, 0);
}

void WriteBmpHeader(std::ostream& os, int width, int height, int bits)
{
	unsigned hdrSize = 14 + 40;
	if (bits == 8)
		hdrSize += 256 * 4;

	// BITMAPFILEHEADER
	WriteU2(os, 0x4D42); // bfType
	WriteU4(os, hdrSize + BmpDataSize(width, height, bits)); // bfSize
	WriteU2(os, 0); // bfReserved1
	WriteU2(os, 0); // bfReserved2
	WriteU4(os, hdrSize); // bfOffBits

	// BITMAPINFOHEADER
	WriteU4(os, 40); // biSize
	WriteU4(os, width); // biWidth
	WriteU4(os, -height); // biHeight
	WriteU2(os, 1); // biPlanes
	WriteU2(os, bits); // biBitCount
	WriteU4(os, BI_RGB); // biCompression
	WriteU4(os, 0); // biSizeImage
	WriteU4(os, 0); // biXPelsPerMeter
	WriteU4(os, 0); // biYPelsPerMeter
	WriteU4(os, 0); // biClrUsed
	WriteU4(os, 0); // biClrImportant

	if (bits == 8)
		for (int i = 0; i < 256; ++i)
			WriteRgbQuad(os, i, i, i);
}

} // namespace

void SaveBmp(const std::string& filename, const uint8_t* data, int width, int height)
{
	std::ofstream fs(filename, std::ios::binary);

	WriteBmpHeader(fs, width, height, 8);
	
	int pad = GetLinePaddingSize(width);
	auto p = data;
	for (int y = height - 1; y >= 0 ; --y)
	{
		for (int x = 0; x < width; ++x)
			WriteU1(fs, *p++);
		for (int i = 0; i < pad; ++i)
			WriteU1(fs, 0);
	}

	fs.close();
	if (fs.fail())
		throw std::runtime_error("BMP Save failed");
}

void Run(bool trace)
{
	ElectronMemory memory(Load("os_300.rom"));
	memory.InstallRom(Load("basic.rom"));

	MOS6502 cpu(memory);
	cpu.Reset(true);
	cpu.Step();
	cpu.Reset(false);

	while (cpu.Cycles() < 10'000'000)
	{
		if (trace)
			std::cout << ShowRegisters(cpu) << " " << Disassemble(memory, cpu.PC()) << "\n";
		cpu.Step();
	}

#if 1
	uint8_t screen[25 * 8 * 40 * 8];

	for (int y = 0; y < 25; ++y)
	{
		for (int x = 0; x < 40; ++x)
		{
			for (int i = 0; i < 8; ++i)
			{
				auto b = memory.Read(0x6000 + y * 40 * 8 + x * 8 + i);
				for (int j = 0; j < 8; ++j)
				{
					screen[(8 * y + i) * 40 * 8 + x * 8 + 7 - j] = (b & (1 << j)) ? 0xFF : 0;
				}
			}
		}
	}

	SaveBmp("ScreenDump.bmp", screen, 40 * 8, 25 * 8);
#endif
}

void TestCpu(bool trace);

} // namespace DjeeDjay

int main(int argc, char* argv[])
try
{
	using namespace DjeeDjay;

	bool trace = false;
	bool test = false;

	while (*++argv)
	{
		if (argv[0] == std::string("--trace"))
			trace = true;
		if (argv[0] == std::string("--test"))
			test = true;
	}

	if (test)
		TestCpu(trace);
	else
		DjeeDjay::Run(trace);
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
