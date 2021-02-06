// (C) Copyright Gert-Jan de Vos 2021.

#pragma once

#include <array>
#include "DjeeDjay/MOS6502.h"

namespace DjeeDjay {

class ElectronMemory : public Memory
{
public:
	explicit ElectronMemory(const std::vector<uint8_t>& rom);

	void InstallRom(const std::vector<uint8_t>& rom);

	uint8_t Read(uint16_t address) override;
	void Write(uint16_t address, uint8_t value) override;

	uint8_t IORead(uint16_t address);
	void IOWrite(uint16_t address, uint8_t value);

	uint8_t InterruptStatus();
	void InterruptStatus(uint8_t value);

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

} // namespace DjeeDjay
