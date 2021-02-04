#include "Simulator.h"
#include "MOS6502.h"

namespace Simulator {

namespace {

std::string Hex4(int n)
{
	return n < 10 ? std::string(1, '0' + n) : std::string(1, 'A' + n - 10);
}

std::string Hex8(uint8_t n)
{
	return Hex4(n >> 4) + Hex4(n & 0x0f);
}

std::string Hex16(uint16_t n)
{
	return Hex8(n >> 8) + Hex8(n & 0xff);
}

std::string Binary(uint16_t addr, uint8_t b0)
{
	return Hex16(addr) + ": " + Hex8(b0) + "      ";
}

std::string Binary(uint16_t addr, uint8_t b0, uint8_t b1)
{
	return Hex16(addr) + ": " + Hex8(b0) + " " + Hex8(b1) + "   ";
}

std::string Binary(uint16_t addr, uint8_t b0, uint8_t b1, uint8_t b2)
{
	return Hex16(addr) + ": " + Hex8(b0) + " " + Hex8(b1) + " " + Hex8(b2);
}

std::string Accumulator(const std::string& ins, Memory& memory, uint16_t addr)
{
	auto b0 = memory.Read(addr);
	return Binary(addr, b0) + " " + ins + " A";
}

std::string Implied(const std::string& ins, Memory& memory, uint16_t addr)
{
	auto b0 = memory.Read(addr);
	return Binary(addr, b0) + " " + ins;
}

std::string Immediate(const std::string& ins, Memory& memory, uint16_t addr)
{
	auto b0 = memory.Read(addr);
	auto b1 = memory.Read(addr + 1);
	return Binary(addr, b0, b1) + " " + ins + " #&" + Hex8(b1);
}

std::string ZeroPage(const std::string& ins, Memory& memory, uint16_t addr)
{
	auto b0 = memory.Read(addr);
	auto b1 = memory.Read(addr + 1);
	return Binary(addr, b0, b1) + " " + ins + " &" + Hex8(b1);
}

std::string ZeroPageX(const std::string& ins, Memory& memory, uint16_t addr)
{
	auto b0 = memory.Read(addr);
	auto b1 = memory.Read(addr + 1);
	return Binary(addr, b0, b1) + " " + ins + " &" + Hex8(b1) + ",X";
}

std::string ZeroPageY(const std::string& ins, Memory& memory, uint16_t addr)
{
	auto b0 = memory.Read(addr);
	auto b1 = memory.Read(addr + 1);
	return Binary(addr, b0, b1) + " " + ins + " &" + Hex8(b1) + ",Y";
}

std::string Relative(const std::string& ins, Memory& memory, uint16_t addr)
{
	auto b0 = memory.Read(addr);
	auto b1 = memory.Read(addr + 1);
	return Binary(addr, b0, b1) + " " + ins + " &" + Hex16(addr + 2 + static_cast<int8_t>(b1));
}

std::string Absolute(const std::string& ins, Memory& memory, uint16_t addr)
{
	auto b0 = memory.Read(addr);
	auto b1 = memory.Read(addr + 1);
	auto b2 = memory.Read(addr + 2);
	return Binary(addr, b0, b1, b2) + " " + ins + " &" + Hex8(b2) + Hex8(b1);
}

std::string AbsoluteX(const std::string& ins, Memory& memory, uint16_t addr)
{
	auto b0 = memory.Read(addr);
	auto b1 = memory.Read(addr + 1);
	auto b2 = memory.Read(addr + 2);
	return Binary(addr, b0, b1, b2) + " " + ins + " &" + Hex8(b2) + Hex8(b1) + ",X";
}

std::string AbsoluteY(const std::string& ins, Memory& memory, uint16_t addr)
{
	auto b0 = memory.Read(addr);
	auto b1 = memory.Read(addr + 1);
	auto b2 = memory.Read(addr + 2);
	return Binary(addr, b0, b1, b2) + " " + ins + " &" + Hex8(b2) + Hex8(b1) + ",Y";
}

std::string Indirect(const std::string& ins, Memory& memory, uint16_t addr)
{
	auto b0 = memory.Read(addr);
	auto b1 = memory.Read(addr + 1);
	return Binary(addr, b0, b1) + " " + ins + " (&" + Hex8(b1) + ")";
}

std::string IndirectX(const std::string& ins, Memory& memory, uint16_t addr)
{
	auto b0 = memory.Read(addr);
	auto b1 = memory.Read(addr + 1);
	return Binary(addr, b0, b1) + " " + ins + " (&" + Hex8(b1) + ",X)";
}

std::string IndirectY(const std::string& ins, Memory& memory, uint16_t addr)
{
	auto b0 = memory.Read(addr);
	auto b1 = memory.Read(addr + 1);
	return Binary(addr, b0, b1) + " " + ins + " (&" + Hex8(b1) + "),Y";
}

std::string Unknown(Memory& memory, uint16_t addr)
{
	auto b0 = memory.Read(addr);
	return Binary(addr, b0) + " ???";
}

std::string Status(const MOS6502& cpu)
{
	return std::string() +
		(cpu.N() ? "N" : "-") +
		(cpu.V() ? "V" : "-") +
		"-" +
		(cpu.B() ? "B" : "-") +
		(cpu.D() ? "D" : "-") +
		(cpu.I() ? "I" : "-") +
		(cpu.Z() ? "Z" : "-") +
		(cpu.C() ? "C" : "-");
}

} // namespace

std::string ShowRegisters(const MOS6502& cpu)
{
	return "A=" + Hex8(cpu.A()) + " X=" + Hex8(cpu.X()) + " Y=" + Hex8(cpu.Y()) + " " + Status(cpu);
}

std::string Disassemble(Memory& memory, uint16_t addr)
{
	switch (memory.Read(addr))
	{
	case 0x00: return Implied("BRK", memory, addr);
	case 0x01: return IndirectX("ORA", memory, addr);
	case 0x05: return ZeroPage("ORA", memory, addr);
	case 0x06: return ZeroPage("ASL", memory, addr);
	case 0x08: return Implied("PHP", memory, addr);
	case 0x09: return Immediate("ORA", memory, addr);
	case 0x0A: return Accumulator("ASL", memory, addr);
	case 0x0D: return Absolute("ORA", memory, addr);
	case 0x0E: return Absolute("ASL", memory, addr);
	case 0x10: return Relative("BPL", memory, addr);
	case 0x11: return IndirectY("ORA", memory, addr);
	case 0x15: return ZeroPageX("ORA", memory, addr);
	case 0x16: return ZeroPageX("ASL", memory, addr);
	case 0x18: return Implied("CLC", memory, addr);
	case 0x19: return AbsoluteY("ORA", memory, addr);
	case 0x1D: return AbsoluteX("ORA", memory, addr);
	case 0x1E: return AbsoluteX("ASL", memory, addr);
	case 0x20: return Absolute("JSR", memory, addr);
	case 0x21: return IndirectX("AND", memory, addr);
	case 0x24: return ZeroPage("BIT", memory, addr);
	case 0x25: return ZeroPage("AND", memory, addr);
	case 0x26: return ZeroPage("ROL", memory, addr);
	case 0x28: return Implied("PLP", memory, addr);
	case 0x29: return Immediate("AND", memory, addr);
	case 0x2A: return Accumulator("ROL", memory, addr);
	case 0x2C: return Absolute("BIT", memory, addr);
	case 0x2D: return Absolute("AND", memory, addr);
	case 0x2E: return Absolute("ROL", memory, addr);
	case 0x30: return Relative("BMI", memory, addr);
	case 0x31: return IndirectY("AND", memory, addr);
	case 0x35: return ZeroPageX("AND", memory, addr);
	case 0x36: return ZeroPageX("ROL", memory, addr);
	case 0x38: return Implied("SEC", memory, addr);
	case 0x39: return AbsoluteY("AND", memory, addr);
	case 0x3D: return AbsoluteX("ORA", memory, addr);
	case 0x3E: return AbsoluteX("ASL", memory, addr);
	case 0x40: return Implied("RTI", memory, addr);
	case 0x41: return IndirectX("EOR", memory, addr);
	case 0x45: return ZeroPage("EOR", memory, addr);
	case 0x46: return ZeroPage("LSR", memory, addr);
	case 0x48: return Implied("PHA", memory, addr);
	case 0x49: return Immediate("EOR", memory, addr);
	case 0x4A: return Accumulator("LSR", memory, addr);
	case 0x4C: return Absolute("JMP", memory, addr);
	case 0x4D: return Absolute("EOR", memory, addr);
	case 0x4E: return Absolute("LSR", memory, addr);
	case 0x50: return Relative("BVC", memory, addr);
	case 0x51: return IndirectY("EOR", memory, addr);
	case 0x55: return ZeroPageX("EOR", memory, addr);
	case 0x56: return ZeroPageX("LSR", memory, addr);
	case 0x58: return Implied("CLI", memory, addr);
	case 0x59: return AbsoluteY("EOR", memory, addr);
	case 0x5D: return AbsoluteX("EOR", memory, addr);
	case 0x5E: return AbsoluteX("LSR", memory, addr);
	case 0x60: return Implied("RTS", memory, addr);
	case 0x61: return IndirectX("ADC", memory, addr);
	case 0x65: return ZeroPage("ADC", memory, addr);
	case 0x66: return ZeroPage("ROR", memory, addr);
	case 0x68: return Implied("PLA", memory, addr);
	case 0x69: return Immediate("ADC", memory, addr);
	case 0x6A: return Accumulator("ROR", memory, addr);
	case 0x6C: return Indirect("JMP", memory, addr);
	case 0x6D: return Absolute("ADC", memory, addr);
	case 0x6E: return Absolute("ROR", memory, addr);
	case 0x70: return Relative("BCS", memory, addr);
	case 0x71: return IndirectY("ADC", memory, addr);
	case 0x75: return ZeroPageX("ADC", memory, addr);
	case 0x76: return ZeroPageX("ROR", memory, addr);
	case 0x78: return Implied("SEI", memory, addr);
	case 0x79: return AbsoluteY("ADC", memory, addr);
	case 0x7D: return AbsoluteX("ADC", memory, addr);
	case 0x7E: return AbsoluteX("ROR", memory, addr);
	case 0x81: return IndirectX("STA", memory, addr);
	case 0x84: return ZeroPage("STY", memory, addr);
	case 0x85: return ZeroPage("STA", memory, addr);
	case 0x86: return ZeroPage("STX", memory, addr);
	case 0x88: return Implied("DEY", memory, addr);
	case 0x8A: return Implied("TXA", memory, addr);
	case 0x8C: return Absolute("STY", memory, addr);
	case 0x8D: return Absolute("STA", memory, addr);
	case 0x8E: return Absolute("STX", memory, addr);
	case 0x90: return Relative("BCC", memory, addr);
	case 0x91: return IndirectY("STA", memory, addr);
	case 0x94: return ZeroPage("STY", memory, addr);
	case 0x95: return ZeroPageX("STA", memory, addr);
	case 0x96: return ZeroPageY("STX", memory, addr);
	case 0x98: return Implied("TYA", memory, addr);
	case 0x99: return AbsoluteY("STA", memory, addr);
	case 0x9A: return Implied("TXS", memory, addr);
	case 0x9D: return AbsoluteX("STA", memory, addr);
	case 0xA0: return Immediate("LDY", memory, addr);
	case 0xA1: return IndirectX("LDA", memory, addr);
	case 0xA2: return Immediate("LDX", memory, addr);
	case 0xA4: return ZeroPage("LDY", memory, addr);
	case 0xA5: return ZeroPage("LDA", memory, addr);
	case 0xA6: return ZeroPage("LDX", memory, addr);
	case 0xA8: return Implied("TAY", memory, addr);
	case 0xA9: return Immediate("LDA", memory, addr);
	case 0xAA: return Implied("TAX", memory, addr);
	case 0xAC: return Absolute("LDY", memory, addr);
	case 0xAD: return Absolute("LDA", memory, addr);
	case 0xAE: return Absolute("LDX", memory, addr);
	case 0xB0: return Relative("BCS", memory, addr);
	case 0xB1: return IndirectY("LDA", memory, addr);
	case 0xB4: return ZeroPage("LDY", memory, addr);
	case 0xB5: return ZeroPageX("LDA", memory, addr);
	case 0xB6: return ZeroPageY("LDX", memory, addr);
	case 0xB8: return Implied("CLV", memory, addr);
	case 0xB9: return AbsoluteY("LDA", memory, addr);
	case 0xBA: return Implied("TSX", memory, addr);
	case 0xBC: return AbsoluteX("LDY", memory, addr);
	case 0xBD: return AbsoluteX("LDA", memory, addr);
	case 0xBE: return AbsoluteY("LDX", memory, addr);
	case 0xC0: return Immediate("CPY", memory, addr);
	case 0xC1: return IndirectX("CMP", memory, addr);
	case 0xC4: return ZeroPage("CPY", memory, addr);
	case 0xC5: return ZeroPage("CMP", memory, addr);
	case 0xC6: return ZeroPage("DEC", memory, addr);
	case 0xC8: return Implied("INY", memory, addr);
	case 0xC9: return Immediate("CMP", memory, addr);
	case 0xCA: return Implied("DEX", memory, addr);
	case 0xCC: return Absolute("CPY", memory, addr);
	case 0xCD: return Absolute("CMP", memory, addr);
	case 0xCE: return Absolute("DEC", memory, addr);
	case 0xD0: return Relative("BNE", memory, addr);
	case 0xD1: return IndirectY("CMP", memory, addr);
	case 0xD5: return ZeroPageX("CMP", memory, addr);
	case 0xD6: return ZeroPageX("DEC", memory, addr);
	case 0xD8: return Implied("CLD", memory, addr);
	case 0xD9: return AbsoluteY("CMP", memory, addr);
	case 0xDD: return AbsoluteX("CMP", memory, addr);
	case 0xDE: return AbsoluteX("DEC", memory, addr);
	case 0xE0: return Immediate("CPX", memory, addr);
	case 0xE1: return IndirectX("SBC", memory, addr);
	case 0xE4: return ZeroPage("CPX", memory, addr);
	case 0xE5: return ZeroPage("SBC", memory, addr);
	case 0xE6: return ZeroPage("INC", memory, addr);
	case 0xE8: return Implied("INX", memory, addr);
	case 0xE9: return Immediate("SBC", memory, addr);
	case 0xEA: return Implied("NOP", memory, addr);
	case 0xEC: return Absolute("CPX", memory, addr);
	case 0xED: return Absolute("SBC", memory, addr);
	case 0xEE: return Absolute("INC", memory, addr);
	case 0xF0: return Relative("BEQ", memory, addr);
	case 0xF1: return IndirectY("SBC", memory, addr);
	case 0xF5: return ZeroPageX("SBC", memory, addr);
	case 0xF6: return ZeroPageX("INC", memory, addr);
	case 0xF8: return Implied("SED", memory, addr);
	case 0xF9: return AbsoluteY("SBC", memory, addr);
	case 0xFD: return AbsoluteX("SBC", memory, addr);
	case 0xFE: return AbsoluteX("INC", memory, addr);
	default: return Unknown(memory, addr);
	}
}

} // namespace Simulator
