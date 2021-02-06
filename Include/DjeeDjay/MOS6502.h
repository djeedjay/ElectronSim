// (C) Copyright Gert-Jan de Vos 2021.

#pragma once

#include <cstdint>
#include <vector>
#include <stdexcept>
#include <string>

namespace DjeeDjay {

struct Memory
{
	virtual uint8_t Read(uint16_t offset) = 0;
	virtual void Write(uint16_t offset, uint8_t value) = 0;
};

class MOS6502
{
public:
	explicit MOS6502(Memory& memory);

	void NMI();
	void Reset(bool value);
	void IRQ(bool value);

	void Step();

	uint16_t PC() const;
	uint8_t A() const;
	uint8_t X() const;
	uint8_t Y() const;
	uint8_t P() const;
	uint8_t S() const;
	bool N() const;
	void N(bool value);
	bool V() const;
	void V(bool value);
	bool B() const;
	bool D() const;
	void D(bool value);
	bool I() const;
	void I(bool value);
	bool Z() const;
	void Z(bool value);
	bool C() const;
	void C(bool value);

	uint64_t Cycles() const;

private:
	uint8_t ReadPC();
	uint16_t ReadPC16();
	uint16_t Read16(uint16_t address);
	enum ImpliedType { Value };
	ImpliedType Implied(int duration = 2);
	uint16_t Immediate();
	uint16_t ZeroPage();
	uint16_t ZeroPageRW();
	uint8_t ZeroPageX();
	uint8_t ZeroPageXRW();
	uint8_t ZeroPageY();
	uint16_t Absolute(int duration = 4);
	uint16_t AbsoluteRW();
	uint16_t AbsoluteX(int duration = 4);
	uint16_t AbsoluteXRW();
	uint16_t AbsoluteY(int duration = 4);
	uint16_t Indirect();
	uint16_t IndirectX();
	uint16_t IndirectY(int duration = 5);
	void Push(uint8_t value);
	uint8_t Pull();

	template <int Bit>
	bool GetP() const
	{
		return p & (1 << Bit);
	}

	template <int Bit>
	void SetP(bool value)
	{
		p = (p & ~(1 << Bit)) | (value << Bit);
	}

	void CLC(ImpliedType);
	void SEC(ImpliedType);
	void CLI(ImpliedType);
	void SEI(ImpliedType);
	void CLV(ImpliedType);
	void CLD(ImpliedType);
	void SED(ImpliedType);
	uint8_t ASL_N(uint8_t arg);
	void ASL(ImpliedType);
	void ASL(uint16_t addr);
	uint8_t LSR_N(uint8_t arg);
	void LSR(ImpliedType);
	void LSR(uint16_t addr);
	uint8_t ROL_N(uint8_t arg);
	void ROL(ImpliedType);
	void ROL(uint16_t addr);
	uint8_t ROR_N(uint8_t arg);
	void ROR(ImpliedType);
	void ROR(uint16_t addr);
	void BRK(ImpliedType);
	void CMP(uint16_t addr);
	void AND(uint16_t addr);
	void BIT(uint16_t addr);
	void ORA(uint16_t addr);
	void ADC_N(uint8_t arg);
	void ADC(uint16_t addr);
	void SBC(uint16_t addr);
	void CPX(uint16_t addr);
	void CPY(uint16_t addr);
	void DEC(uint16_t addr);
	void EOR(uint16_t addr);
	void INC(uint16_t addr);
	void JMP(uint16_t addr);
	void JSR(uint16_t addr);
	void LDA(uint16_t addr);
	void LDX(uint16_t addr);
	void LDY(uint16_t addr);
	void NOP(ImpliedType);
	void RTI(ImpliedType);
	void RTS(ImpliedType);
	void STA(uint16_t addr);
	void STX(uint16_t addr);
	void STY(uint16_t addr);
	void TXS(ImpliedType);
	void TSX(ImpliedType);
	void PHA(ImpliedType);
	void PLA(ImpliedType);
	void PHP(ImpliedType);
	void PLP(ImpliedType);
	void TAX(ImpliedType);
	void TXA(ImpliedType);
	void DEX(ImpliedType);
	void INX(ImpliedType);
	void TAY(ImpliedType);
	void TYA(ImpliedType);
	void DEY(ImpliedType);
	void INY(ImpliedType);
	void Branch(bool condition);
	uint8_t Read(uint16_t address);
	void Write(uint16_t address, uint8_t value);

	Memory& m_memory;

	bool m_nmi = false;
	bool m_reset = false;
	bool m_irq = false;
	uint64_t cycle;
	uint16_t pc;
	uint8_t a;
	uint8_t x;
	uint8_t y;
	uint8_t s;
	uint8_t p;
};

struct MemoryReadError : std::runtime_error
{
	explicit MemoryReadError(uint16_t address);

	uint16_t address;
};

struct MemoryWriteError : std::runtime_error
{
	explicit MemoryWriteError(uint16_t address);

	uint16_t address;
};

struct InvalidOpcodeError : std::runtime_error
{
	explicit InvalidOpcodeError(uint8_t opcode);

	uint8_t opcode;
};

std::vector<uint8_t> Load(const std::string& path);

std::string ShowRegisters(const MOS6502& cpu);
std::string Disassemble(Memory& memory, uint16_t addr);

} // namespace DjeeDjay
