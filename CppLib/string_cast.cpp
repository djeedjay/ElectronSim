// (C) Copyright Gert-Jan de Vos 2021.

#include <cstdlib>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <clocale>
#include "DjeeDjay/string_cast.h"
#include "DjeeDjay/Win32/Win32Error.h"

namespace DjeeDjay {

std::string Narrow(const wchar_t* p, size_t length)
{
	if (length == 0)
		return std::string();

	int size = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, p, static_cast<int>(length), nullptr, 0, nullptr, nullptr);
	if (size == 0)
		Win32::ThrowLastError("WideCharToMultiByte");

	std::vector<char> buf(size);
	size = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, p, static_cast<int>(length), buf.data(), static_cast<int>(buf.size()), nullptr, nullptr);
	if (size == 0)
		Win32::ThrowLastError("WideCharToMultiByte");
	return std::string(buf.data(), size);
}

std::string Narrow(const wchar_t* p)
{
	return Narrow(p, std::wcslen(p));
}

std::string Narrow(const std::wstring& s)
{
	return Narrow(s.c_str(), s.length());
}

std::wstring Widen(const char* p, size_t length)
{
	if (length == 0)
		return std::wstring();

	int size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, p, static_cast<int>(length),  nullptr, 0);
	if (size == 0)
		Win32::ThrowLastError("MultiByteToWideChar");

	std::vector<wchar_t> buf(size);
	size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, p, static_cast<int>(length),  buf.data(), static_cast<int>(buf.size()));
	if (size == 0)
		Win32::ThrowLastError("MultiByteToWideChar");
	return std::wstring(buf.data(), size);
}

std::wstring Widen(const char* p)
{
	return Widen(p, std::strlen(p));
}

std::wstring Widen(const std::string& s)
{
	return Widen(s.c_str(), s.length());
}

WStr::WStr(std::wstring str) :
	m_str(std::move(str))
{
}

WStr::WStr(const std::string& str) :
	m_str(string_cast<std::wstring>(str))
{
}

WStr::operator const wchar_t*() const
{
	return m_str.c_str();
}

const wchar_t* WStr::c_str() const
{
	return m_str.c_str();
}

std::wstring WStr::str() const
{
	return m_str;
}

std::string AddCr(const std::string& s)
{
	std::string result;
	char prev = '\0';
	for (auto c : s)
	{
		if (c == '\n' && prev != '\r')
			result += '\r';
		result += c;
		prev = c;
	}
	return result;
}

std::wstring AddCr(const std::wstring& s)
{
	std::wstring result;
	wchar_t prev = L'\0';
	for (auto c : s)
	{
		if (c == L'\n' && prev != L'\r')
			result += L'\r';
		result += c;
		prev = c;
	}
	return result;
}

} // namespace DjeeDjay
