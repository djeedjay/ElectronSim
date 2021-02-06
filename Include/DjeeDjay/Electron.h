// (C) Copyright Gert-Jan de Vos 2021.

#pragma once

#include <functional>
#include "DjeeDjay/MOS6502.h"
#include "DjeeDjay/ElectronMemory.h"

namespace DjeeDjay {

class Electron
{
public:
	using FrameCompletedEvent = std::function<void ()>;

	explicit Electron(const std::vector<uint8_t>& rom);

	void InstallRom(const std::vector<uint8_t>& rom);

	void Reset();

	void FrameCompleted(FrameCompletedEvent slot);

	void Step();

private:
	ElectronMemory m_memory;
	MOS6502 m_cpu;
	FrameCompletedEvent m_frameCompleted;
};

} // namespace DjeeDjay
