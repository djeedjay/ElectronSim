// (C) Copyright Gert-Jan de Vos 2021.

#include <vector>
#include <iomanip>
#include <windows.h>
#include "DjeeDjay/StringBuilder.h"
#include "DjeeDjay/ToHexString.h"
#include "DjeeDjay/Win32/Win32Error.h"
#include "DjeeDjay/Win32/Resources.h"

#pragma comment(lib, "version.lib")

namespace DjeeDjay {
namespace Win32 {

std::ostream& operator<<(std::ostream& os, const FileVersion& v)
{
	return os << v.major << "." << v.minor << "." << v.patch << "." << v.revision;
}

bool operator==(const FileVersion& a, const FileVersion& b)
{
	return a.major == b.major && a.minor == b.minor && a.patch == b.patch && a.revision == b.revision;
}

bool operator!=(const FileVersion& a, const FileVersion& b)
{
	return !(a == b);
}

bool operator<(const FileVersion& a, const FileVersion& b)
{
	return
	a.major < b.major ||
	(a.major == b.major &&
		(a.minor < b.minor ||
		(a.minor == b.minor && (
			a.patch < b.patch ||
			(a.patch == b.patch && a.revision < b.revision)))));
}

bool operator>(const FileVersion& a, const FileVersion& b)
{
	return !(a < b) && a != b;
}

bool operator<=(const FileVersion& a, const FileVersion& b)
{
	return a == b || a < b;
}
bool operator>=(const FileVersion& a, const FileVersion& b)
{
	return a == b || a > b;
}

class FileVersionInfo
{
public:
	explicit FileVersionInfo(const std::wstring& filename);

	const void* QueryValue(const std::wstring& name, unsigned& size) const;
	const void* GetValue(const std::wstring& name, unsigned& size) const;
	const wchar_t* QueryVersionInfo(const std::wstring& name, int language, int codePage) const;

private:
	std::vector<BYTE> m_data;
};

FileVersionInfo::FileVersionInfo(const std::wstring& filename) :
	m_data(::GetFileVersionInfoSizeW(filename.c_str(), nullptr))
{
	if (m_data.empty())
		ThrowLastError("GetFileVersionInfoSizeW");

	if (!::GetFileVersionInfoW(filename.c_str(), 0, static_cast<DWORD>(m_data.size()), m_data.data()))
		ThrowLastError("GetFileVersionInfoW");
}

const void* FileVersionInfo::QueryValue(const std::wstring& name, unsigned& size) const
{
	void* pData = nullptr;
	if (!VerQueryValueW(m_data.data(), name.c_str(), &pData, &size))
		return nullptr;
	return pData;
}

const void* FileVersionInfo::GetValue(const std::wstring& name, unsigned& size) const
{
	auto p = QueryValue(name, size);
	if (!p)
		ThrowLastError("VerQueryValue");
	return p;
}

const wchar_t* FileVersionInfo::QueryVersionInfo(const std::wstring& name, int language, int codePage) const
{
	std::wstring path = WStringBuilder() << L"\\StringFileInfo\\" << ToHexWString(language, 4) << ToHexWString(codePage, 4) << L"\\" << name;
	unsigned size;
	return static_cast<const wchar_t*>(QueryValue(path.c_str(), size));
}

struct LanguageAndCodePage
{
	WORD language;
	WORD codePage;
};

std::wstring GetVersionInfo(const std::wstring& moduleFileName, const std::wstring& name)
{
	FileVersionInfo versionInfo(moduleFileName);

	unsigned size;
	auto translations = static_cast<const LanguageAndCodePage*>(versionInfo.GetValue(L"\\VarFileInfo\\Translation", size));

	for (unsigned i = 0; i < size / sizeof(LanguageAndCodePage); ++i)
	{
		if (auto p = versionInfo.QueryVersionInfo(name, translations[i].language, translations[i].codePage))
			return std::wstring(static_cast<const wchar_t*>(p));
	}

	return std::wstring();
}

std::wstring LoadString(HINSTANCE hInstance, unsigned id)
{
	const wchar_t* p = nullptr;
	int len = ::LoadStringW(hInstance, id, reinterpret_cast<wchar_t*>(&p), 0);
	if (len <= 0)
		return std::wstring();

	return std::wstring(p, static_cast<size_t>(len));
}

std::wstring LoadString(unsigned id)
{
	return LoadString(::GetModuleHandleW(nullptr), id);
}

} // namespace Win32
} // namespace DjeeDjay
