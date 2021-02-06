// (C) Copyright Gert-Jan de Vos 2021.

#include <iostream>
#include "DjeeDjay/ElectronMemory.h"

namespace DjeeDjay {

ElectronMemory::ElectronMemory(const std::vector<uint8_t>& rom) :
	m_irqStatus(UnusedMask | PowerOnReset),
	m_irqEnable(0),
	m_romBankIndex(0)
{
	if (rom.size() != 0x4000)
		throw std::runtime_error("Bad ROM size");
	std::copy(rom.begin(), rom.end(), m_os.begin());
}

void ElectronMemory::InstallRom(const std::vector<uint8_t>& rom)
{
	if (rom.size() != 0x4000)
		throw std::runtime_error("Bad ROM size");
	std::copy(rom.begin(), rom.end(), m_romBank.begin());
}

uint8_t ElectronMemory::Read(uint16_t address)
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

void ElectronMemory::Write(uint16_t address, uint8_t value)
{
	if (address < 0x8000)
		m_ram[address] = value;
	else if (address >= 0xfe00 && address < 0xff00)
		return IOWrite(address, value);
	else
		std::cout << "Invalid write " << std::hex << address << ", " << +value << "\n";
}

uint8_t ElectronMemory::IORead(uint16_t address)
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
	return 0;
}

void ElectronMemory::IOWrite(uint16_t address, uint8_t value)
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

uint8_t ElectronMemory::InterruptStatus()
{
	auto irqStatus = m_irqStatus;
	m_irqStatus &= ~PowerOnReset;
	return irqStatus;
}

void ElectronMemory::InterruptStatus(uint8_t value)
{
	m_irqEnable = value;
}

void ElectronMemory::ScreenLow(uint8_t value)
{
	m_screenLow = value;
}

void ElectronMemory::ScreenHigh(uint8_t value)
{
	m_screenHigh = value;
}

uint8_t ElectronMemory::CassetteDataShift()
{
	return 0;
}

void ElectronMemory::CassetteDataShift(uint8_t)
{
}

void ElectronMemory::InterruptClearAndPaging(uint8_t value)
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

void ElectronMemory::Counter(uint8_t)
{
}

uint8_t ElectronMemory::MiscellaneousControl()
{
	return m_miscControl;
}

void ElectronMemory::MiscellaneousControl(uint8_t value)
{
	m_miscControl = value;
}

uint8_t ElectronMemory::Palette(int index)
{
	return m_palette[index];
}

void ElectronMemory::Palette(int index, uint8_t value)
{
	m_palette[index] = value;
}

} // namespace DjeeDjay
