﻿// (C) Copyright Gert-Jan de Vos 2021.

#pragma once

#include <cstdint>
#include <array>
#include <functional>

namespace DjeeDjay {

class MOS6502;
class Image;

struct KeyboardBit
{
	KeyboardBit(int column, int bit);

	int column;
	int bit;
};

class Ula
{
public:
	using TraceEvent = std::function<void (const std::string& msg)>;

	explicit Ula(MOS6502& cpu);

	void Trace(TraceEvent slot);

	void InstallRom(int bank, std::vector<uint8_t> rom);
	uint8_t ReadKeyboard(uint16_t address);
	uint8_t ReadRom(uint16_t address);

	void KeyDown(const KeyboardBit& key);
	void KeyUp(const KeyboardBit& key);

	void UpdateTimers();
	bool ReadyForNextFrame() const;
	void GenerateFrame(const uint8_t* ram, Image& image);

	uint8_t Read(uint16_t address);
	void Write(uint16_t address, uint8_t value);

	uint8_t InterruptStatus();

	void ScreenLow(uint8_t value);
	void ScreenHigh(uint8_t value);

	uint8_t CassetteDataShift();
	void CassetteDataShift(uint8_t);

	void InterruptClearAndPaging(uint8_t value);

	void Counter(uint8_t);

	uint8_t MiscellaneousControl();
	void MiscellaneousControl(uint8_t value);

	uint8_t Palette(int index);
	void Palette(int index, uint8_t value);

private:
	void TriggerRtcInterrupt();
	void UpdateIrqStatus(uint8_t enable, uint8_t status);
	uint32_t PaletteR(int index, int bit) const;
	uint32_t PaletteG(int index, int bit) const;
	uint32_t PaletteB(int index, int bit) const;
	std::array<uint32_t, 2> Palette2();
	std::array<uint32_t, 4> Palette4();
	std::array<uint32_t, 16> Palette16();

	MOS6502& m_cpu;
	TraceEvent m_trace;
	std::array<std::vector<uint8_t>, 16> m_roms;
	std::array<uint8_t, 14> m_keyboard;

	uint64_t m_nextFrameCycle;
	uint64_t m_nextRtcCycle;

	bool m_nmi;
	uint8_t m_irqStatus;
	uint8_t m_irqEnable;
	uint8_t m_screenLow;
	uint8_t m_screenHigh;
	int m_romBankIndex;
	uint8_t m_miscControl;
	uint8_t m_palette[8];
};

} // namespace DjeeDjay