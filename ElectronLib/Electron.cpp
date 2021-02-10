// (C) Copyright Gert-Jan de Vos 2021.

#include <cassert>
#include <iomanip>
#include <thread>
#include "DjeeDjay/ToHexString.h"
#include "DjeeDjay/Electron.h"

namespace DjeeDjay {

namespace {

KeyboardBit ToKeyboardBit(ElectronKey key)
{
	switch (key)
	{
	case ElectronKey::Right: return KeyboardBit(0, 0);
	case ElectronKey::Copy: return KeyboardBit(0, 1);
	case ElectronKey::Space: return KeyboardBit(0, 3);
	case ElectronKey::Left: return KeyboardBit(1, 0);
	case ElectronKey::Down: return KeyboardBit(1, 1);
	case ElectronKey::Return: return KeyboardBit(1, 2);
	case ElectronKey::Delete: return KeyboardBit(1, 3);
	case ElectronKey::Minus: return KeyboardBit(2, 0);
	case ElectronKey::Up: return KeyboardBit(2, 1);
	case ElectronKey::Colon: return KeyboardBit(2, 2);
	case ElectronKey::Semicolon: return KeyboardBit(3, 2);
	case ElectronKey::Divides: return KeyboardBit(3, 3);
	case ElectronKey::Num0: return KeyboardBit(3, 0);
	case ElectronKey::Num1: return KeyboardBit(12, 0);
	case ElectronKey::Num2: return KeyboardBit(11, 0);
	case ElectronKey::Num3: return KeyboardBit(10, 0);
	case ElectronKey::Num4: return KeyboardBit(9, 0);
	case ElectronKey::Num5: return KeyboardBit(8, 0);
	case ElectronKey::Num6: return KeyboardBit(7, 0);
	case ElectronKey::Num7: return KeyboardBit(6, 0);
	case ElectronKey::Num8: return KeyboardBit(5, 0);
	case ElectronKey::Num9: return KeyboardBit(4, 0);
	case ElectronKey::A: return KeyboardBit(12, 2);
	case ElectronKey::B: return KeyboardBit(8, 3);
	case ElectronKey::C: return KeyboardBit(10, 3);
	case ElectronKey::D: return KeyboardBit(10, 2);
	case ElectronKey::E: return KeyboardBit(10, 1);
	case ElectronKey::F: return KeyboardBit(9, 2);
	case ElectronKey::G: return KeyboardBit(8, 2);
	case ElectronKey::H: return KeyboardBit(7, 2);
	case ElectronKey::I: return KeyboardBit(5, 1);
	case ElectronKey::J: return KeyboardBit(6, 2);
	case ElectronKey::K: return KeyboardBit(5, 2);
	case ElectronKey::L: return KeyboardBit(4, 2);
	case ElectronKey::M: return KeyboardBit(6, 3);
	case ElectronKey::N: return KeyboardBit(7, 3);
	case ElectronKey::O: return KeyboardBit(4, 1);
	case ElectronKey::P: return KeyboardBit(3, 1);
	case ElectronKey::Q: return KeyboardBit(12, 1);
	case ElectronKey::R: return KeyboardBit(9, 1);
	case ElectronKey::S: return KeyboardBit(11, 2);
	case ElectronKey::T: return KeyboardBit(8, 1);
	case ElectronKey::U: return KeyboardBit(6, 1);
	case ElectronKey::V: return KeyboardBit(9, 3);
	case ElectronKey::W: return KeyboardBit(11, 1);
	case ElectronKey::X: return KeyboardBit(11, 3);
	case ElectronKey::Y: return KeyboardBit(7, 1);
	case ElectronKey::Z: return KeyboardBit(12, 3);
	case ElectronKey::Dot: return KeyboardBit(4, 3);
	case ElectronKey::Comma: return KeyboardBit(5, 3);
	case ElectronKey::Escape: return KeyboardBit(13, 0);
	case ElectronKey::CapsLk: return KeyboardBit(13, 1);
	case ElectronKey::Ctrl: return KeyboardBit(13, 2);
	case ElectronKey::Shift: return KeyboardBit(13, 3);
	default:
		assert(!"Unexpected ElectronKey");
		return KeyboardBit(0, 2);
	}
}

} // namespace

Electron::Electron(const std::vector<uint8_t>& rom) :
	m_cpu(*this),
	m_ula(m_cpu)
{
	if (rom.size() != 0x4000)
		throw std::runtime_error("Bad ROM size");
	std::copy(rom.begin(), rom.end(), m_os.begin());
}

void Electron::InstallRom(int bank, std::vector<uint8_t> rom)
{
	m_ula.InstallRom(bank, std::move(rom));
}

void Electron::Restart()
{
	m_cpu.Reset(true);
	m_ula.Restart();
	m_cpu.Step();
	m_cpu.Reset(false);
	m_startTime = std::chrono::steady_clock::now();
	m_oneMhzCycles = 0;
}

void Electron::Break()
{
	m_cpu.Reset(true);
	m_ula.Reset();
	m_cpu.Step();
	m_cpu.Reset(false);
	m_startTime = std::chrono::steady_clock::now();
	m_oneMhzCycles = 0;
}

void Electron::KeyDown(ElectronKey key)
{
	m_ula.KeyDown(ToKeyboardBit(key));
}

void Electron::KeyUp(ElectronKey key)
{
	m_ula.KeyUp(ToKeyboardBit(key));
}

void Electron::Trace(TraceEvent slot)
{
	m_ula.Trace(slot);
	m_trace = slot;
}

void Electron::FrameCompleted(FrameCompletedEvent slot)
{
	m_frameCompleted = slot;
}

void Electron::CapsLock(CapsLockEvent slot)
{
	m_ula.CapsLock(slot);
}

void Electron::CassetteMotor(CassetteMotorEvent slot)
{
	m_ula.CassetteMotor(slot);
}

bool Electron::CapsLock() const
{
	return m_ula.CapsLock();
}

bool Electron::CassetteMotor() const
{
	return m_ula.CassetteMotor();
}

void Electron::Speaker(SpeakerEvent slot)
{
	return m_ula.Speaker(slot);
}

using CpuCycles = std::chrono::duration<uint64_t, std::ratio<1, 2'000'000>>;

void Electron::Step()
{
	m_ula.UpdateTimers();
	if (m_ula.ReadyForNextFrame())
	{
		m_ula.GenerateFrame(m_ram.data(), m_image);
		m_frameCompleted(m_image);
		std::this_thread::sleep_until(m_startTime + CpuCycles(m_cpu.Cycles() + m_oneMhzCycles + m_ula.OneMHzCycles() + m_ula.VideoCycles()));
	}
	m_cpu.Step();
}

uint8_t Electron::Read(uint16_t address)
{
	if (address < 0x8000)
		return m_ram[address];
	else if (address < 0xc000)
		return m_ula.ReadRom(address);
	else if (address >= 0xfe00 && address < 0xff00)
		return m_ula.Read(address);
	else
		return m_os[address - 0xc000];
}

void Electron::Write(uint16_t address, uint8_t value)
{
	if (address < 0x8000)
		m_ram[address] = value;
	else if (address >= 0xfe00 && address < 0xff00)
		return m_ula.Write(address, value);
	else
		m_trace("Invalid write " + ToHexString(address) + ", " + ToHexString(value) + "\n");
}

} // namespace DjeeDjay
