// (C) Copyright Gert-Jan de Vos 2021.

#include <cassert>
#include "DjeeDjay/Image.h"
#include "DjeeDjay/ToHexString.h"
#include "DjeeDjay/MOS6502.h"
#include "DjeeDjay/Electron/Ula.h"

namespace DjeeDjay {

namespace {

constexpr uint8_t UnusedMask = 0x80;
constexpr uint8_t HighToneDetect = 0x40;
constexpr uint8_t ReceiveDataFull = 0x20;
constexpr uint8_t TransmitDataEmpty = 0x10;
constexpr uint8_t RealTimeClock = 0x08;
constexpr uint8_t DisplayEnd = 0x04;
constexpr uint8_t PowerOnReset = 0x02;
constexpr uint8_t AllInteruptsMask = HighToneDetect | ReceiveDataFull | TransmitDataEmpty | RealTimeClock | DisplayEnd;
constexpr uint8_t MasterIrq = 0x01;

constexpr uint64_t VSyncCycles = 312 * 64 * 2;
constexpr uint64_t VSyncToRtcCycles = 100 * 64 * 2;

int RomBankNr(int index)
{
	switch (index)
	{
	case 1: return 0;
	case 3: return 2;
	case 9: return 8;
	case 11: return 10;
	default: return index;
	}
}

class ScreenBufferIterator
{
public:
	ScreenBufferIterator(const uint8_t* ram, size_t screenMin, size_t screenStart) :
		m_ram(ram),
		m_screenMin(screenMin),
		m_address(screenStart)
	{
	}

	ScreenBufferIterator& operator++()
	{
		if (++m_address == 0x8000)
			m_address = m_screenMin;
		return *this;
	}

	uint8_t operator*() const
	{
		return m_ram[m_address];
	}

private:
	const uint8_t* m_ram;
	size_t m_screenMin;
	size_t m_address;
};

void Write1(Image& image, int x, int y, uint32_t value)
{
	image.Data()[y * image.Width() + x] = value;
}

void Write2(Image& image, int x, int y, uint32_t value, const std::array<uint32_t, 16>& palette)
{
	Write1(image, x + 0, y, palette[((value & 0x80) >> 4) | ((value & 0x20) >> 3) | ((value & 0x08) >> 2) | ((value & 0x02) >> 1)]);
	Write1(image, x + 1, y, palette[((value & 0x40) >> 3) | ((value & 0x10) >> 2) | ((value & 0x04) >> 1) | (value & 0x01)]);
}

void Write4(Image& image, int x, int y, uint32_t value, const std::array<uint32_t, 4>& palette)
{
	Write1(image, x + 0, y, palette[((value & 0x80) >> 6) | ((value & 0x08) >> 3)]);
	Write1(image, x + 1, y, palette[((value & 0x40) >> 5) | ((value & 0x04) >> 2)]);
	Write1(image, x + 2, y, palette[((value & 0x20) >> 4) | ((value & 0x02) >> 1)]);
	Write1(image, x + 3, y, palette[((value & 0x10) >> 3) | (value & 0x01)]);
}

void Write8(Image& image, int x, int y, uint32_t value, const std::array<uint32_t, 2>& palette)
{
	Write1(image, x + 0, y, palette[(value & 0x80) >> 7]);
	Write1(image, x + 1, y, palette[(value & 0x40) >> 6]);
	Write1(image, x + 2, y, palette[(value & 0x20) >> 5]);
	Write1(image, x + 3, y, palette[(value & 0x10) >> 4]);
	Write1(image, x + 4, y, palette[(value & 0x08) >> 3]);
	Write1(image, x + 5, y, palette[(value & 0x04) >> 2]);
	Write1(image, x + 6, y, palette[(value & 0x02) >> 1]);
	Write1(image, x + 7, y, palette[value & 0x01]);
}

void GenerateMode0(const uint8_t* ram, size_t screenStart, const std::array<uint32_t, 2>& palette, Image& image)
{
	image.Resize(640, 256);
	ScreenBufferIterator it(ram, 0x3000, screenStart);

	for (int y = 0; y < 32; ++y)
	{
		for (int x = 0; x < 80; ++x)
		{
			for (int row = 0; row < 8; ++row)
			{
				Write8(image, 8 * x, 8 * y + row, *it, palette);
				++it;
			}
		}
	}
}

void GenerateMode1(const uint8_t* ram, size_t screenStart, const std::array<uint32_t, 4>& palette, Image& image)
{
	image.Resize(320, 256);
	ScreenBufferIterator it(ram, 0x3000, screenStart);

	for (int y = 0; y < 32; ++y)
	{
		for (int x = 0; x < 80; ++x)
		{
			for (int row = 0; row < 8; ++row)
			{
				Write4(image, 4 * x, 8 * y + row, *it, palette);
				++it;
			}
		}
	}
}

void GenerateMode2(const uint8_t* ram, size_t screenStart, const std::array<uint32_t, 16>& palette, Image& image)
{
	image.Resize(160, 256);
	ScreenBufferIterator it(ram, 0x3000, screenStart);

	for (int y = 0; y < 32; ++y)
	{
		for (int x = 0; x < 80; ++x)
		{
			for (int row = 0; row < 8; ++row)
			{
				Write2(image, 2 * x, 8 * y + row, *it, palette);
				++it;
			}
		}
	}
}

void GenerateMode3(const uint8_t* ram, size_t screenStart, const std::array<uint32_t, 2>& palette, Image& image)
{
	image.Resize(640, 256);
	ScreenBufferIterator it(ram, 0x4000, screenStart);

	for (int x = 0; x < image.Width(); ++x)
	{
		Write1(image, x, 0, 0);
		Write1(image, x, 1, 0);
		Write1(image, x, 2, 0);
		Write1(image, x, 3, 0);
		Write1(image, x, 254, 0);
		Write1(image, x, 255, 0);
	}
	for (int y = 0; y < 25; ++y)
	{
		for (int x = 0; x < 80; ++x)
		{
			for (int row = 0; row < 8; ++row)
			{
				Write8(image, 8 * x, 4 + 10 * y + row, *it, palette);
				++it;
			}
		}

		for (int x = 0; x < image.Width(); ++x)
		{
			Write1(image, x, 4 + 10 * y + 8, 0);
			Write1(image, x, 4 + 10 * y + 9, 0);
		}
	}
}

void GenerateMode4(const uint8_t* ram, size_t screenStart, const std::array<uint32_t, 2>& palette, Image& image)
{
	image.Resize(320, 256);
	ScreenBufferIterator it(ram, 0x5800, screenStart);

	for (int y = 0; y < 32; ++y)
	{
		for (int x = 0; x < 40; ++x)
		{
			for (int row = 0; row < 8; ++row)
			{
				Write8(image, 8 * x, 8 * y + row, *it, palette);
				++it;
			}
		}
	}
}

void GenerateMode5(const uint8_t* ram, size_t screenStart, const std::array<uint32_t, 4>& palette, Image& image)
{
	image.Resize(160, 256);
	ScreenBufferIterator it(ram, 0x5800, screenStart);

	for (int y = 0; y < 32; ++y)
	{
		for (int x = 0; x < 40; ++x)
		{
			for (int row = 0; row < 8; ++row)
			{
				Write4(image, 4 * x, 8 * y + row, *it, palette);
				++it;
			}
		}
	}
}

void GenerateMode6(const uint8_t* ram, size_t screenStart, const std::array<uint32_t, 2>& palette, Image& image)
{
	image.Resize(320, 256);
	ScreenBufferIterator it(ram, 0x6000, screenStart);

	for (int x = 0; x < image.Width(); ++x)
	{
		Write1(image, x, 0, 0);
		Write1(image, x, 1, 0);
		Write1(image, x, 2, 0);
		Write1(image, x, 3, 0);
		Write1(image, x, 254, 0);
		Write1(image, x, 255, 0);
	}
	for (int y = 0; y < 25; ++y)
	{
		for (int x = 0; x < 40; ++x)
		{
			for (int row = 0; row < 8; ++row)
			{
				Write8(image, 8 * x, 4 + 10 * y + row, *it, palette);
				++it;
			}
		}

		for (int x = 0; x < image.Width(); ++x)
		{
			Write1(image, x, 4 + 10 * y + 8, 0);
			Write1(image, x, 4 + 10 * y + 9, 0);
		}
	}
}

} // namespace

KeyboardBit::KeyboardBit(int column, int bit) :
	column(column),
	bit(bit)
{
}

Ula::Ula(MOS6502& cpu) :
	m_cpu(cpu)
{
	std::fill(m_keyboard.begin(), m_keyboard.end(), static_cast<uint8_t>(0));
	Restart();
}

void Ula::Trace(TraceEvent slot)
{
	m_trace = slot;
}

void Ula::CapsLock(CapsLockEvent slot)
{
	m_capsLock = slot;
}

void Ula::CassetteMotor(CassetteMotorEvent slot)
{
	m_cassetteMotor = slot;
}

void Ula::Speaker(SpeakerEvent slot)
{
	m_speaker = slot;
}

bool Ula::CapsLock() const
{
	return m_miscControl & 0x80;
}

bool Ula::CassetteMotor() const
{
	return m_miscControl & 0x40;
}

void Ula::InstallRom(int bank, std::vector<uint8_t> rom)
{
	if (bank < 0 || bank >= m_roms.size())
		throw std::runtime_error("Bad ROM bank");
	if (rom.size() > 0x4000)
		throw std::runtime_error("Bad ROM size");
	m_roms[RomBankNr(bank)] = std::move(rom);
}

void Ula::Restart()
{
	Reset();
	m_irqStatus |= PowerOnReset;
}

void Ula::Reset()
{
	m_oneMHzCycles = 0;
	m_videoCycles = 0;
	m_nextFrameCycle = VSyncCycles;
	m_nextRtcCycle = VSyncCycles + VSyncToRtcCycles;
	m_romBankIndex = 0;
	UpdateIrqStatus(0, 0);
}

uint8_t Ula::ReadKeyboard(uint16_t address) const
{
	uint8_t value = 0;
	if ((address & 0xC001) == 0x8000)
		value |= m_keyboard[0];
	if ((address & 0xC002) == 0x8000)
		value |= m_keyboard[1];
	if ((address & 0xC004) == 0x8000)
		value |= m_keyboard[2];
	if ((address & 0xC008) == 0x8000)
		value |= m_keyboard[3];
	if ((address & 0xC010) == 0x8000)
		value |= m_keyboard[4];
	if ((address & 0xC020) == 0x8000)
		value |= m_keyboard[5];
	if ((address & 0xC040) == 0x8000)
		value |= m_keyboard[6];
	if ((address & 0xC080) == 0x8000)
		value |= m_keyboard[7];
	if ((address & 0xC100) == 0x8000)
		value |= m_keyboard[8];
	if ((address & 0xC200) == 0x8000)
		value |= m_keyboard[9];
	if ((address & 0xC400) == 0x8000)
		value |= m_keyboard[10];
	if ((address & 0xC800) == 0x8000)
		value |= m_keyboard[11];
	if ((address & 0xD000) == 0x8000)
		value |= m_keyboard[12];
	if ((address & 0xE000) == 0x8000)
		value |= m_keyboard[13];

	return value;
}

uint8_t Ula::ReadRom(uint16_t address) const
{
	return
		m_romBankIndex == 8 ? ReadKeyboard(address) :
		address < 0x8000 + m_roms[m_romBankIndex].size() ? m_roms[m_romBankIndex][address - 0x8000] : 0xff;
}

void Ula::KeyDown(const KeyboardBit& key)
{
	m_keyboard[key.column] |= (1 << key.bit);
}

void Ula::KeyUp(const KeyboardBit& key)
{
	m_keyboard[key.column] &= ~(1 << key.bit);
}

void Ula::UpdateTimers()
{
	if (m_cpu.Cycles() > m_nextRtcCycle)
		TriggerRtcInterrupt();
}

uint64_t Ula::OneMHzCycles() const
{
	return m_oneMHzCycles;
}

uint64_t Ula::VideoCycles() const
{
	return m_videoCycles;
}

bool Ula::ReadyForNextFrame() const
{
	return m_cpu.Cycles() > m_nextFrameCycle;
}

void Ula::TriggerRtcInterrupt()
{
	UpdateIrqStatus(m_irqEnable, m_irqStatus | RealTimeClock);
	m_nextRtcCycle += VSyncCycles;
}

void Ula::UpdateIrqStatus(uint8_t enable, uint8_t status)
{
	bool irq = m_irqStatus & MasterIrq;
	if (status & enable & AllInteruptsMask)
		status |= MasterIrq;
	else
		status &= ~MasterIrq;

	m_irqEnable = enable;
	m_irqStatus = status;
	if (irq != (status & MasterIrq))
		m_cpu.IRQ(status & MasterIrq);
}

uint8_t Ula::Read(uint16_t address)
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

	m_trace("IO Read " + ToHexString(address) + "\n");
	return 0;
}

void Ula::Write(uint16_t address, uint8_t value)
{
	switch (address & 0xff0f)
	{
	case 0xfe00: return UpdateIrqStatus(value, m_irqStatus);
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

	m_trace("IO Write" + ToHexString(address) + ", " + ToHexString(value) + "\n");
}

uint8_t Ula::InterruptStatus()
{
	auto irqStatus = m_irqStatus;
	m_irqStatus &= ~PowerOnReset;
	return irqStatus;
}

void Ula::ScreenLow(uint8_t value)
{
	m_screenLow = value;
}

void Ula::ScreenHigh(uint8_t value)
{
	m_screenHigh = value;
}

uint8_t Ula::CassetteDataShift()
{
	return 0;
}

void Ula::CassetteDataShift(uint8_t)
{
}

void Ula::InterruptClearAndPaging(uint8_t value)
{
	m_romBankIndex = RomBankNr(value & 0x0f);
	if (value & 0x80)
		m_nmi = false;

	auto irqStatus = m_irqStatus;
	if (value & 0x40)
		irqStatus &= ~HighToneDetect;
	if (value & 0x20)
		irqStatus &= ~RealTimeClock;
	if (value & 0x10)
		irqStatus &= ~DisplayEnd;
	UpdateIrqStatus(m_irqEnable, irqStatus);
}

void Ula::Counter(uint8_t value)
{
	if (((m_miscControl & 0x06) >> 1) == 1 && value != m_counter)
		m_speaker(1'000'000 / (16 * (value + 1)));

	m_counter = value;
}

uint8_t Ula::MiscellaneousControl()
{
	return m_miscControl;
}

void Ula::MiscellaneousControl(uint8_t value)
{
	bool capsLock = value & 0x80;
	if (capsLock != CapsLock())
		m_capsLock(capsLock);
	bool cassetteMotor = value & 0x40;
	if (cassetteMotor != CassetteMotor())
		m_cassetteMotor(cassetteMotor);

	auto mode = (value & 0x06) >> 1;
	if (mode != ((m_miscControl & 0x06) >> 1))
		m_speaker(mode == 1 ? 1'000'000 / (16 * (m_counter + 1)) : 0);

	m_miscControl = value;
}

uint8_t Ula::Palette(int index)
{
	return m_palette[index];
}

void Ula::Palette(int index, uint8_t value)
{
	m_palette[index] = value;
}

uint32_t Ula::PaletteB(int index, int bit) const
{
	return m_palette[index] & (1 << bit) ? 0 : 0x0000FF;
}

uint32_t Ula::PaletteG(int index, int bit) const
{
	return m_palette[index] & (1 << bit) ? 0 : 0x00FF00;
}

uint32_t Ula::PaletteR(int index, int bit) const
{
	return m_palette[index] & (1 << bit) ? 0 : 0xFF0000;
}

std::array<uint32_t, 2> Ula::Palette2()
{
	return
	{
		PaletteB(0, 4) | PaletteG(1, 4) | PaletteR(1, 0),
		PaletteB(0, 6) | PaletteG(0, 2) | PaletteR(1, 2)
	};
}

std::array<uint32_t, 4> Ula::Palette4()
{
	return
	{
		PaletteB(0, 4) | PaletteG(1, 4) | PaletteR(1, 0),
		PaletteB(0, 5) | PaletteG(1, 5) | PaletteR(1, 1),
		PaletteB(0, 6) | PaletteG(0, 2) | PaletteR(1, 2),
		PaletteB(0, 7) | PaletteG(0, 3) | PaletteR(1, 3)
	};
}

std::array<uint32_t, 16> Ula::Palette16()
{
	return
	{
		PaletteB(0, 4) | PaletteG(1, 4) | PaletteR(1, 0), // 0
		PaletteB(6, 4) | PaletteG(7, 4) | PaletteR(7, 0), // 1
		PaletteB(0, 5) | PaletteG(1, 5) | PaletteR(1, 1), // 2
		PaletteB(6, 5) | PaletteG(7, 5) | PaletteR(7, 1), // 3
		PaletteB(2, 4) | PaletteG(3, 4) | PaletteR(3, 0), // 4
		PaletteB(4, 4) | PaletteG(5, 4) | PaletteR(5, 0), // 5
		PaletteB(2, 5) | PaletteG(3, 5) | PaletteR(3, 1), // 6
		PaletteB(4, 5) | PaletteG(5, 5) | PaletteR(5, 1), // 7
		PaletteB(0, 6) | PaletteG(0, 2) | PaletteR(1, 2), // 8
		PaletteB(6, 6) | PaletteG(6, 2) | PaletteR(7, 2), // 9
		PaletteB(0, 7) | PaletteG(0, 3) | PaletteR(1, 3), // 10
		PaletteB(6, 7) | PaletteG(6, 3) | PaletteR(6, 3), // 11
		PaletteB(2, 6) | PaletteG(2, 2) | PaletteR(3, 2), // 12
		PaletteB(4, 6) | PaletteG(4, 2) | PaletteR(5, 2), // 13
		PaletteB(2, 7) | PaletteG(2, 3) | PaletteR(3, 3), // 14
		PaletteB(4, 7) | PaletteG(4, 3) | PaletteR(4, 3), // 15
	};
}

void Ula::GenerateFrame(const uint8_t* ram, Image& image)
{
	int mode = (m_miscControl & 0x38) >> 3;

	size_t screenStart = ((m_screenHigh << 9) | (m_screenLow << 1)) & 0x7fc0;
	switch (mode)
	{
	case 0:
		GenerateMode0(ram, screenStart, Palette2(), image);
		m_videoCycles += 80 * 8 * 32 * 2;
		break;
	case 1:
		GenerateMode1(ram, screenStart, Palette4(), image);
		m_videoCycles += 40 * 2 * 8 * 32 * 2;
		break;
	case 2:
		GenerateMode2(ram, screenStart, Palette16(), image);
		m_videoCycles += 20 * 4 * 8 * 32 * 2;
		break;
	case 3:
		GenerateMode3(ram, screenStart, Palette2(), image);
		m_videoCycles += 80 * 8 * 25 * 2;
		break;
	case 4:
		GenerateMode4(ram, screenStart, Palette2(), image);
		m_videoCycles += 40 * 8 * 32 * 2;
		break;
	case 5:
		GenerateMode5(ram, screenStart, Palette4(), image);
		m_videoCycles += 20 * 2 * 8 * 32 * 2;
		break;
	case 6:
		GenerateMode6(ram, screenStart, Palette2(), image);
		m_videoCycles += 40 * 8 * 25 * 2;
		break;
	}

	UpdateIrqStatus(m_irqEnable, m_irqStatus | DisplayEnd);
	m_nextFrameCycle += VSyncCycles;
}

} // namespace DjeeDjay
