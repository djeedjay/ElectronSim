// (C) Copyright Gert-Jan de Vos 2021.

#include <windows.h>
#include "DjeeDjay/NonCopyable.h"
#include "DjeeDjay/StringBuilder.h"
#include "DjeeDjay/ToHexString.h"
#include "DjeeDjay/Win32/Handle.h"
#include "DjeeDjay/Win32/Win32Error.h"

namespace DjeeDjay {
namespace Win32 {

Win32Error::Win32Error(unsigned long rc, const char* msg) :
	std::runtime_error(msg),
	m_rc(rc)
{
}

Win32Error::Win32Error(unsigned long rc, const std::string& msg) :
	std::runtime_error(msg),
	m_rc(rc)
{
}

unsigned long Win32Error::GetError() const
{
	return m_rc;
}

const char* Win32Error::what() const noexcept
{
	m_what = StringBuilder() << std::runtime_error::what() << ": " << GetErrorMessage(m_rc);
	return m_what.c_str();
}

std::string GetErrorMessage(DWORD rc)
{
	char* p = nullptr;
	DWORD length = FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, // DWORD dwFlags,
		nullptr, // lpSource
		rc, // dwMessageId
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // dwLanguageId
		reinterpret_cast<char*>(&p), // lpBuffer
		0, // nSize
		nullptr // Arguments
	);
	if (length == 0)
		return ToUpperHexString(rc);

	HLocal lp(p);
	while (length > 0 && isspace(p[length - 1]))
		--length;

	return std::string(p, p + length);
}

std::string GetErrorMessage(HRESULT rc)
{
	return GetErrorMessage(static_cast<DWORD>(rc));
}

void ThrowLastError(const char* msg)
{
	throw Win32Error(GetLastError(), msg);
}

void ThrowLastError(const std::string& msg)
{
	throw Win32Error(GetLastError(), msg);
}

} // namespace Win32
} // namespace DjeeDjay
