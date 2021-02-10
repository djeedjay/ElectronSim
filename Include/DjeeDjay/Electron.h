// (C) Copyright Gert-Jan de Vos 2021.

#pragma once

#include <array>
#include <functional>
#include <chrono>
#include "DjeeDjay/Image.h"
#include "DjeeDjay/MOS6502.h"
#include "DjeeDjay/Electron/Ula.h"

namespace DjeeDjay {

enum class ElectronKey
{
	None,
	Right,
	Copy,
	Space,
	Left,
	Down,
	Return,
	Delete,
	Minus,
	Up,
	Colon,
	Semicolon,
	Divides,
	Num0,
	Num1,
	Num2,
	Num3,
	Num4,
	Num5,
	Num6,
	Num7,
	Num8,
	Num9,
	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,
	Dot,
	Comma,
	Escape,
	CapsLk,
	Ctrl,
	Shift
};

class Electron : public Memory
{
public:
	using TraceEvent = std::function<void (const std::string& msg)>;
	using FrameCompletedEvent = std::function<void (const Image& image)>;
	using CapsLockEvent = Ula::CapsLockEvent;
	using CassetteMotorEvent = Ula::CassetteMotorEvent;
	using SpeakerEvent = Ula::SpeakerEvent;

	explicit Electron(const std::vector<uint8_t>& rom);

	void InstallRom(int bank, std::vector<uint8_t> rom);

	void Restart();
	void Break();

	void KeyDown(ElectronKey key);
	void KeyUp(ElectronKey key);

	void Trace(TraceEvent slot);
	void FrameCompleted(FrameCompletedEvent slot);
	void CapsLock(CapsLockEvent slot);
	void CassetteMotor(CassetteMotorEvent slot);
	void Speaker(SpeakerEvent slot);

	bool CapsLock() const;
	bool CassetteMotor() const;

	void Step();

	// Memory
	uint8_t Read(uint16_t address) override;
	void Write(uint16_t address, uint8_t value) override;

private:
	MOS6502 m_cpu;
	std::array<uint8_t, 0x8000> m_ram;
	std::array<uint8_t, 0x4000> m_os;
	Ula m_ula;
	Image m_image;
	std::chrono::steady_clock::time_point m_startTime;
	uint64_t m_oneMhzCycles;

	TraceEvent m_trace;
	FrameCompletedEvent m_frameCompleted;
};

} // namespace DjeeDjay
