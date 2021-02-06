// (C) Copyright Gert-Jan de Vos 2021.

#pragma once

#include <string>
#include <atlcomcli.h>
#include "DjeeDjay/Win32/Win32Error.h"

namespace DjeeDjay {
namespace Win32 {

class HResultExeption : public Win32Error
{
public:
	explicit HResultExeption(HRESULT hr);
	HResultExeption(HRESULT hr, const std::wstring& description);
	HResultExeption(HRESULT hr, const std::wstring& source, const std::wstring& description);
	HResultExeption(HRESULT hr, const std::wstring& source, const std::wstring& description, const std::wstring& helpFile, DWORD helpContext);

	HRESULT HResult() const;
	GUID Guid() const;
	std::wstring Source() const;
	std::wstring Description() const;
	std::wstring HelpFile() const;
	DWORD HelpContext() const;

	const char* what() const noexcept override;

private:
	GUID m_iid;
	std::wstring m_source;
	std::wstring m_description;
	std::wstring m_helpFile;
	DWORD m_helpContext;
	mutable std::string m_what;
};

void ThrowFailed(HRESULT hr);
void ThrowFailed(HRESULT hr, const std::wstring& description);
void ThrowFailed(HRESULT hr, const std::wstring& source, const std::wstring& description);
void ThrowFailed(HRESULT hr, const std::wstring& source, const std::wstring& description, const std::wstring& helpFile, DWORD helpContext);

HRESULT HResultFromException(const GUID& iid);
HRESULT HResultFromException();

} // namespace Win32
} // namespace DjeeDjay
