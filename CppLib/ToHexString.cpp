// (C) Copyright Gert-Jan de Vos 2021.

#include <iomanip>
#include "DjeeDjay/StringBuilder.h"

namespace DjeeDjay {
namespace Detail {

std::string ToLowerHexString(unsigned long long value, int width)
{
	return StringBuilder() << std::setfill('0') << std::setw(width) << std::nouppercase << std::hex << value;
}

std::wstring ToLowerHexWString(unsigned long long value, int width)
{
	return WStringBuilder() << std::setfill(L'0') << std::setw(width) << std::nouppercase << std::hex << value;
}

std::string ToUpperHexString(unsigned long long value, int width)
{
	return StringBuilder() << std::setfill('0') << std::setw(width) << std::uppercase << std::hex << value;
}

std::wstring ToUpperHexWString(unsigned long long value, int width)
{
	return WStringBuilder() << std::setfill(L'0') << std::setw(width) << std::uppercase << std::hex << value;
}

} // namespace Detail
} // namespace DjeeDjay
