// (C) Copyright Gert-Jan de Vos 2021.

#include "DjeeDjay/Electron.h"

namespace DjeeDjay {

Electron::Electron(const std::vector<uint8_t>& rom) :
	m_memory(rom),
	m_cpu(m_memory)
{
}

void Electron::InstallRom(const std::vector<uint8_t>& rom)
{
	m_memory.InstallRom(rom);
}

void Electron::Reset()
{
	m_cpu.Reset(true);
	m_cpu.Step();
	m_cpu.Reset(false);
}

void Electron::FrameCompleted(FrameCompletedEvent slot)
{
	m_frameCompleted = slot;
}

void Electron::Step()
{
	m_cpu.Step();
}

} // namespace DjeeDjay
