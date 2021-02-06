// (C) Copyright Gert-Jan de Vos 2021.

#include "DjeeDjay/MOS6502.h"

namespace DjeeDjay {

namespace {

uint8_t MSB(uint16_t value)
{
	return value >> 8;
}

uint8_t LSB(uint16_t value)
{
	return value & 0xFF;
}

} // namespace

MemoryReadError::MemoryReadError(uint16_t address) :
	std::runtime_error("Read error"),
	address(address)
{
}

MemoryWriteError::MemoryWriteError(uint16_t address) :
	std::runtime_error("Write error"),
	address(address)
{
}

InvalidOpcodeError::InvalidOpcodeError(uint8_t opcode) :
	std::runtime_error("Opcode error"),
	opcode(opcode)
{
}

MOS6502::MOS6502(Memory& memory) :
	m_memory(memory)
{
}

void MOS6502::NMI()
{
	m_nmi = true;
}

void MOS6502::Reset(bool value)
{
	m_reset = value;
}

void MOS6502::IRQ(bool value)
{
	m_irq = value;
}

void MOS6502::Step()
{
	if (m_reset)
	{
		cycle = 0;
		pc = Read16(0xfffc);
		p = 0x20;
		s = 0xff;
		return;
	}
	if (m_nmi)
	{
		Push(MSB(pc));
		Push(LSB(pc));
		Push(p | 0x20);
		m_nmi = false;
		pc = Read16(0xfffa);
		cycle += 7;
		return;
	}
	if (!I() && m_irq)
	{
		Push(MSB(pc));
		Push(LSB(pc));
		Push(p | 0x20);
		I(true);
		pc = Read16(0xfffe);
		cycle += 7;
		return;
	}

	uint8_t opcode = ReadPC();
	switch (opcode)
	{
	case 0x00: return BRK(Implied(7));
	case 0x01: return ORA(IndirectX());
	case 0x05: return ORA(ZeroPage());
	case 0x06: return ASL(ZeroPageRW());
	case 0x08: return PHP(Implied(3));
	case 0x09: return ORA(Immediate());
	case 0x0d: return ORA(Absolute());
	case 0x0e: return ASL(AbsoluteRW());
	case 0x0a: return ASL(Implied());
	case 0x10: return Branch(!N());
	case 0x11: return ORA(IndirectY());
	case 0x15: return ORA(ZeroPageX());
	case 0x16: return ASL(ZeroPageXRW());
	case 0x18: return CLC(Implied());
	case 0x19: return ORA(AbsoluteY());
	case 0x1d: return ORA(AbsoluteX());
	case 0x1e: return ASL(AbsoluteXRW());
	case 0x20: return JSR(Absolute(6));
	case 0x21: return AND(IndirectX());
	case 0x24: return BIT(ZeroPage());
	case 0x25: return AND(ZeroPage());
	case 0x26: return ROL(ZeroPageRW());
	case 0x28: return PLP(Implied(4));
	case 0x29: return AND(Immediate());
	case 0x2a: return ROL(Implied());
	case 0x2c: return BIT(Absolute());
	case 0x2d: return AND(Absolute());
	case 0x2e: return ROL(AbsoluteRW());
	case 0x30: return Branch(N());
	case 0x31: return AND(IndirectY());
	case 0x35: return AND(ZeroPageX());
	case 0x36: return ROL(ZeroPageXRW());
	case 0x38: return SEC(Implied());
	case 0x39: return AND(AbsoluteY());
	case 0x3d: return AND(AbsoluteX());
	case 0x3e: return ROL(AbsoluteXRW());
	case 0x40: return RTI(Implied(6));
	case 0x41: return EOR(IndirectX());
	case 0x45: return EOR(ZeroPage());
	case 0x46: return LSR(ZeroPageRW());
	case 0x48: return PHA(Implied(3));
	case 0x49: return EOR(Immediate());
	case 0x4a: return LSR(Implied());
	case 0x4c: return JMP(Absolute(3));
	case 0x4d: return EOR(Absolute());
	case 0x4e: return LSR(AbsoluteRW());
	case 0x50: return Branch(!V());
	case 0x51: return EOR(IndirectY());
	case 0x55: return EOR(ZeroPageX());
	case 0x59: return EOR(AbsoluteY());
	case 0x5d: return EOR(AbsoluteX());
	case 0x56: return LSR(ZeroPageXRW());
	case 0x58: return CLI(Implied());
	case 0x5e: return LSR(AbsoluteXRW());
	case 0x60: return RTS(Implied(6));
	case 0x61: return ADC(IndirectX());
	case 0x65: return ADC(ZeroPage());
	case 0x66: return ROR(ZeroPageRW());
	case 0x68: return PLA(Implied(4));
	case 0x69: return ADC(Immediate());
	case 0x6a: return ROR(Implied());
	case 0x6c: return JMP(Indirect());
	case 0x6d: return ADC(Absolute());
	case 0x6e: return ROR(AbsoluteRW());
	case 0x70: return Branch(V());
	case 0x71: return ADC(IndirectY());
	case 0x75: return ADC(ZeroPageX());
	case 0x76: return ROR(ZeroPageXRW());
	case 0x78: return SEI(Implied());
	case 0x79: return ADC(AbsoluteY());
	case 0x7d: return ADC(AbsoluteX());
	case 0x7e: return ROR(AbsoluteXRW());
	case 0x81: return STA(IndirectX());
	case 0x84: return STY(ZeroPage());
	case 0x85: return STA(ZeroPage());
	case 0x86: return STX(ZeroPage());
	case 0x88: return DEY(Implied());
	case 0x8a: return TXA(Implied());
	case 0x8c: return STY(Absolute());
	case 0x8d: return STA(Absolute());
	case 0x8e: return STX(Absolute());
	case 0x90: return Branch(!C());
	case 0x91: return STA(IndirectY(6));
	case 0x94: return STY(ZeroPageX());
	case 0x95: return STA(ZeroPageX());
	case 0x96: return STX(ZeroPageY());
	case 0x98: return TYA(Implied());
	case 0x9a: return TXS(Implied());
	case 0x99: return STA(AbsoluteY(5));
	case 0x9d: return STA(AbsoluteX(5));
	case 0xa0: return LDY(Immediate());
	case 0xa1: return LDA(IndirectX());
	case 0xa2: return LDX(Immediate());
	case 0xa4: return LDY(ZeroPage());
	case 0xa5: return LDA(ZeroPage());
	case 0xa6: return LDX(ZeroPage());
	case 0xa8: return TAY(Implied());
	case 0xa9: return LDA(Immediate());
	case 0xaa: return TAX(Implied());
	case 0xac: return LDY(Absolute());
	case 0xad: return LDA(Absolute());
	case 0xae: return LDX(Absolute());
	case 0xb0: return Branch(C());
	case 0xb1: return LDA(IndirectY());
	case 0xb4: return LDY(ZeroPageX());
	case 0xb5: return LDA(ZeroPageX());
	case 0xb6: return LDX(ZeroPageY());
	case 0xb8: return CLV(Implied());
	case 0xb9: return LDA(AbsoluteY());
	case 0xba: return TSX(Implied());
	case 0xbc: return LDY(AbsoluteX());
	case 0xbd: return LDA(AbsoluteX());
	case 0xbe: return LDX(AbsoluteY());
	case 0xc0: return CPY(Immediate());
	case 0xc1: return CMP(IndirectX());
	case 0xc4: return CPY(ZeroPage());
	case 0xc5: return CMP(ZeroPage());
	case 0xc6: return DEC(ZeroPageRW());
	case 0xc8: return INY(Implied());
	case 0xc9: return CMP(Immediate());
	case 0xca: return DEX(Implied());
	case 0xcc: return CPY(Absolute());
	case 0xcd: return CMP(Absolute());
	case 0xce: return DEC(AbsoluteRW());
	case 0xd0: return Branch(!Z());
	case 0xd1: return CMP(IndirectY());
	case 0xd5: return CMP(ZeroPageX());
	case 0xd6: return DEC(ZeroPageXRW());
	case 0xd8: return CLD(Implied());
	case 0xd9: return CMP(AbsoluteY());
	case 0xdd: return CMP(AbsoluteX());
	case 0xde: return DEC(AbsoluteXRW());
	case 0xe0: return CPX(Immediate());
	case 0xe1: return SBC(IndirectX());
	case 0xe4: return CPX(ZeroPage());
	case 0xe5: return SBC(ZeroPage());
	case 0xe6: return INC(ZeroPageRW());
	case 0xe8: return INX(Implied());
	case 0xe9: return SBC(Immediate());
	case 0xea: return NOP(Implied(2));
	case 0xec: return CPX(Absolute());
	case 0xed: return SBC(Absolute());
	case 0xee: return INC(AbsoluteRW());
	case 0xf0: return Branch(Z());
	case 0xf1: return SBC(IndirectY());
	case 0xf5: return SBC(ZeroPageX());
	case 0xf6: return INC(ZeroPageXRW());
	case 0xf8: return SED(Implied());
	case 0xf9: return SBC(AbsoluteY());
	case 0xfd: return SBC(AbsoluteX());
	case 0xfe: return INC(AbsoluteXRW());
	default: throw InvalidOpcodeError(opcode);
	}
}

uint16_t MOS6502::PC() const
{
	return pc;
}

uint8_t MOS6502::A() const
{
	return a;
}

uint8_t MOS6502::X() const
{
	return x;
}

uint8_t MOS6502::Y() const
{
	return y;
}

uint8_t MOS6502::P() const
{
	return p;
}

uint8_t MOS6502::S() const
{
	return s;
}

bool MOS6502::N() const
{
	return GetP<7>();
}

void MOS6502::N(bool value)
{
	SetP<7>(value);
}

bool MOS6502::V() const
{
	return GetP<6>();
}

void MOS6502::V(bool value)
{
	SetP<6>(value);
}

bool MOS6502::B() const
{
	return GetP<4>();
}

bool MOS6502::D() const
{
	return GetP<3>();
}

void MOS6502::D(bool value)
{
	SetP<3>(value);
}

bool MOS6502::I() const
{
	return GetP<2>();
}

void MOS6502::I(bool value)
{
	SetP<2>(value);
}

bool MOS6502::Z() const
{
	return GetP<1>();
}

void MOS6502::Z(bool value)
{
	SetP<1>(value);
}

bool MOS6502::C() const
{
	return GetP<0>();
}

void MOS6502::C(bool value)
{
	SetP<0>(value);
}

uint64_t MOS6502::Cycles() const
{
	return cycle;
}

uint8_t MOS6502::ReadPC()
{
	return Read(pc++);
}

uint16_t MOS6502::ReadPC16()
{
	uint16_t lsb = ReadPC();
	uint16_t msb = ReadPC();
	return (msb << 8) | lsb;
}

uint16_t MOS6502::Read16(uint16_t address)
{
	uint16_t lsb = Read(address);
	uint16_t msb = Read(address + 1);
	return (msb << 8) | lsb;
}

MOS6502::ImpliedType MOS6502::Implied(int duration)
{
	cycle += duration;
	return ImpliedType::Value;
}

uint16_t MOS6502::Immediate()
{
	cycle += 2;
	return pc++;
}

uint16_t MOS6502::ZeroPage()
{
	cycle += 3;
	return ReadPC();
}

uint16_t MOS6502::ZeroPageRW()
{
	cycle += 5;
	return ReadPC();
}

uint8_t MOS6502::ZeroPageX()
{
	cycle += 4;
	return ReadPC() + x;
}

uint8_t MOS6502::ZeroPageXRW()
{
	cycle += 6;
	return ReadPC() + x;
}

uint8_t MOS6502::ZeroPageY()
{
	cycle += 4;
	return ReadPC() + y;
}

uint16_t MOS6502::Absolute(int duration)
{
	cycle += duration;
	return ReadPC16();
}

uint16_t MOS6502::AbsoluteRW()
{
	cycle += 6;
	return ReadPC16();
}

uint16_t MOS6502::AbsoluteX(int duration)
{
	uint16_t lsb = ReadPC() + x;
	uint16_t msb = ReadPC();

	if (lsb > 0xff && duration == 4)
		duration = 5;

	cycle += duration;
	return (msb << 8) + lsb;
}

uint16_t MOS6502::AbsoluteXRW()
{
	cycle += 7;
	return ReadPC16() + x;
}

uint16_t MOS6502::AbsoluteY(int duration)
{
	uint16_t lsb = ReadPC() + y;
	uint16_t msb = ReadPC();

	if (lsb > 0xff && duration == 4)
		duration = 5;

	cycle += duration;
	return (msb << 8) + lsb;
}

uint16_t MOS6502::Indirect()
{
	cycle += 5;
	uint8_t a0 = ReadPC();
	uint8_t a1 = ReadPC();
	uint8_t addr0 = Read((a1 << 8) | a0);
	uint8_t addr1 = Read((a1 << 8) | (a0 + 1));
	return (addr1 << 8) | addr0;
}

uint16_t MOS6502::IndirectX()
{
	cycle += 6;
	return Read16(static_cast<uint8_t>(Read(pc++) + x));
}

uint16_t MOS6502::IndirectY(int duration)
{
	uint16_t addr = Read(pc++);
	uint16_t lsb = Read(addr) + y;
	uint16_t msb = Read(addr + 1);

	if (lsb > 0xff && duration == 5)
		duration = 6;

	cycle += duration;
	return (msb << 8) + lsb;
}

void MOS6502::Push(uint8_t value)
{
	Write(0x0100 | s, value);
	--s;
}

uint8_t MOS6502::Pull()
{
	++s;
	return Read(0x0100 | s);
}

void MOS6502::CLC(ImpliedType)
{
	C(false);
}

void MOS6502::SEC(ImpliedType)
{
	C(true);
}

void MOS6502::CLI(ImpliedType)
{
	I(false);
}

void MOS6502::SEI(ImpliedType)
{
	I(true);
}

void MOS6502::CLV(ImpliedType)
{
	V(false);
}

void MOS6502::CLD(ImpliedType)
{
	D(false);
}

void MOS6502::SED(ImpliedType)
{
	D(true);
}

uint8_t MOS6502::ASL_N(uint8_t arg)
{
	C(arg & 0x80);
	arg = arg << 1;
	N(arg & 0x80);
	Z(arg == 0);
	return arg;
}

void MOS6502::ASL(ImpliedType)
{
	a = ASL_N(a);
}

void MOS6502::ASL(uint16_t addr)
{
	Write(addr, ASL_N(Read(addr)));
}

uint8_t MOS6502::LSR_N(uint8_t arg)
{
	C(arg & 0x01);
	arg = arg >> 1;
	N(arg & 0x80);
	Z(arg == 0);
	return arg;
}

void MOS6502::LSR(ImpliedType)
{
	a = LSR_N(a);
}

void MOS6502::LSR(uint16_t addr)
{
	Write(addr, LSR_N(Read(addr)));
}

uint8_t MOS6502::ROL_N(uint8_t arg)
{
	uint8_t c = C();
	uint8_t result = (arg << 1) | c;
	C(arg & 0x80);
	N(result & 0x80);
	Z(result == 0);
	return result;
}

void MOS6502::ROL(ImpliedType)
{
	a = ROL_N(a);
}

void MOS6502::ROL(uint16_t addr)
{
	Write(addr, ROL_N(Read(addr)));
}

uint8_t MOS6502::ROR_N(uint8_t arg)
{
	uint8_t c = C();
	uint8_t result = (c << 7) | (arg >> 1);
	C(arg & 0x01);
	N(result & 0x80);
	Z(result == 0);
	return result;
}

void MOS6502::ROR(ImpliedType)
{
	a = ROR_N(a);
}

void MOS6502::ROR(uint16_t addr)
{
	Write(addr, ROR_N(Read(addr)));
}

void MOS6502::BRK(ImpliedType)
{
	pc += 1;
	Push(MSB(pc));
	Push(LSB(pc));
	Push(p | 0x30);
	I(true);
	pc = Read16(0xfffe);
}

void MOS6502::CMP(uint16_t addr)
{
	uint8_t arg = Read(addr);
	N((a - arg) & 0x80);
	C(a >= arg);
	Z(a == arg);
}

void MOS6502::AND(uint16_t addr)
{
	a &= Read(addr);
	N(a & 0x80);
	Z(a == 0);
}

void MOS6502::BIT(uint16_t addr)
{
	auto arg = Read(addr);
	N(arg & 0x80);
	V(arg & 0x40);
	Z((arg & a) == 0);
}

void MOS6502::ORA(uint16_t addr)
{
	a |= Read(addr);
	N(a & 0x80);
	Z(a == 0);
}

void MOS6502::ADC_N(uint8_t arg)
{
	uint16_t sum = a + arg + C();
	C(sum > 255);
	V((a ^ sum) & (arg ^ sum) & 0x80);
	a = static_cast<uint8_t>(sum);
	Z(a == 0);
	N(a & 0x80);
}

void MOS6502::ADC(uint16_t addr)
{
	ADC_N(Read(addr));
}

void MOS6502::SBC(uint16_t addr)
{
	ADC_N(~Read(addr));
}

void MOS6502::CPX(uint16_t addr)
{
	uint8_t arg = Read(addr);
	N((x - arg) & 0x80);
	C(x >= arg);
	Z(x == arg);
}

void MOS6502::CPY(uint16_t addr)
{
	uint8_t arg = Read(addr);
	N((y - arg) & 0x80);
	C(y >= arg);
	Z(y == arg);
}

void MOS6502::DEC(uint16_t addr)
{
	uint8_t value = Read(addr) - 1;
	N(value & 0x80);
	Z(value == 0);
	Write(addr, value);
}

void MOS6502::EOR(uint16_t addr)
{
	a ^= Read(addr);
	N(a & 0x80);
	Z(a == 0);
}

void MOS6502::INC(uint16_t addr)
{
	uint8_t value = Read(addr) + 1;
	Write(addr, value);
	N(value & 0x80);
	Z(value == 0);
}

void MOS6502::JMP(uint16_t addr)
{
	pc = addr;
}

void MOS6502::JSR(uint16_t addr)
{
	uint16_t next = pc - 1;
	Push(MSB(next));
	Push(LSB(next));
	pc = addr;
}

void MOS6502::LDA(uint16_t addr)
{
	a = Read(addr);
	N(a & 0x80);
	Z(a == 0);
}

void MOS6502::LDX(uint16_t addr)
{
	x = Read(addr);
	N(x & 0x80);
	Z(x == 0);
}

void MOS6502::LDY(uint16_t addr)
{
	y = Read(addr);
	N(y & 0x80);
	Z(y == 0);
}

void MOS6502::NOP(ImpliedType)
{
}

void MOS6502::RTI(ImpliedType)
{
	p = Pull();
	uint16_t addr = Pull();
	addr |= Pull() << 8;
	pc = addr;
}

void MOS6502::RTS(ImpliedType)
{
	uint16_t addr = Pull();
	addr |= Pull() << 8;
	pc = addr + 1;
}

void MOS6502::STA(uint16_t addr)
{
	Write(addr, a);
}

void MOS6502::STX(uint16_t addr)
{
	Write(addr, x);
}

void MOS6502::STY(uint16_t addr)
{
	Write(addr, y);
}

void MOS6502::TXS(ImpliedType)
{
	s = x;
}

void MOS6502::TSX(ImpliedType)
{
	x = s;
	N(x & 0x80);
	Z(x == 0);
}

void MOS6502::PHA(ImpliedType)
{
	Push(a);
}

void MOS6502::PLA(ImpliedType)
{
	a = Pull();
	Z(a == 0);
	N(a & 0x80);
}

void MOS6502::PHP(ImpliedType)
{
	Push(p | 0x30);
}

void MOS6502::PLP(ImpliedType)
{
	p = Pull();
}

void MOS6502::TAX(ImpliedType)
{
	x = a;
	N(x & 0x80);
	Z(x == 0);
}

void MOS6502::TXA(ImpliedType)
{
	a = x;
	N(a & 0x80);
	Z(a == 0);
}

void MOS6502::DEX(ImpliedType)
{
	--x;
	N(x & 0x80);
	Z(x == 0);
}

void MOS6502::INX(ImpliedType)
{
	++x;
	N(x & 0x80);
	Z(x == 0);
}

void MOS6502::TAY(ImpliedType)
{
	y = a;
	N(y & 0x80);
	Z(y == 0);
}

void MOS6502::TYA(ImpliedType)
{
	a = y;
	N(a & 0x80);
	Z(a == 0);
}

void MOS6502::DEY(ImpliedType)
{
	--y;
	N(y & 0x80);
	Z(y == 0);
}

void MOS6502::INY(ImpliedType)
{
	++y;
	N(y & 0x80);
	Z(y == 0);
}

void MOS6502::Branch(bool condition)
{
	if (condition)
	{
		uint16_t dst = pc + static_cast<int8_t>(ReadPC());
		if ((dst & 0xff00) != (pc & 0xff00))
			cycle += 1;
		pc = dst;
		cycle += 1;
	}
	else
	{
		pc += 1;
	}
	cycle += 2;
}

uint8_t MOS6502::Read(uint16_t address)
{
	return m_memory.Read(address);
}

void MOS6502::Write(uint16_t address, uint8_t value)
{
	m_memory.Write(address, value);
}

} // namespace DjeeDjay
