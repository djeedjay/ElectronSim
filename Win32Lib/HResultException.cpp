// (C) Copyright Gert-Jan de Vos 2021.

#include <sstream>
#include "DjeeDjay/string_cast.h"
#include "DjeeDjay/Win32/ComApi.h"

namespace DjeeDjay {
namespace Win32 {

HResultExeption::HResultExeption(HRESULT hr) :
	Win32Error(hr, "HRESULT Error"),
	m_iid(IID_NULL),
	m_helpContext(0)
{
}

HResultExeption::HResultExeption(HRESULT hr, const std::wstring& description) :
	Win32Error(hr, "HRESULT Error"),
	m_iid(IID_NULL),
	m_description(description),
	m_helpContext(0)
{
}

HResultExeption::HResultExeption(HRESULT hr, const std::wstring& source, const std::wstring& description) :
	Win32Error(hr, "HRESULT Error"),
	m_iid(IID_NULL),
	m_source(source),
	m_description(description),
	m_helpContext(0)
{
}

HResultExeption::HResultExeption(HRESULT hr, const std::wstring& source, const std::wstring& description, const std::wstring& helpFile, DWORD helpContext) :
	Win32Error(hr, "HRESULT Error"),
	m_iid(IID_NULL),
	m_source(source),
	m_description(description),
	m_helpFile(helpFile),
	m_helpContext(helpContext)
{
}

HRESULT HResultExeption::HResult() const
{
	return GetError();
}

GUID HResultExeption::Guid() const
{
	return m_iid;
}

std::wstring HResultExeption::Source() const
{
	return m_source;
}

std::wstring HResultExeption::Description() const
{
	return m_description;
}

std::wstring HResultExeption::HelpFile() const
{
	return m_helpFile;
}

DWORD HResultExeption::HelpContext() const
{
	return m_helpContext;
}

const char* HResultExeption::what() const noexcept
{
	std::ostringstream ss;
	ss << Win32Error::what();
	if (!m_source.empty())
		ss << " in  " << string_cast<std::string>(m_source) << ": ";
	ss << string_cast<std::string>(m_description);
	m_what = ss.str();
	return m_what.c_str();
}

void ThrowFailed(HRESULT hr)
{
	if (FAILED(hr))
		throw HResultExeption(hr);
}

void ThrowFailed(HRESULT hr, const std::wstring& description)
{
	if (FAILED(hr))
		throw HResultExeption(hr, description);
}

void ThrowFailed(HRESULT hr, const std::wstring& source, const std::wstring& description)
{
	if (FAILED(hr))
		throw HResultExeption(hr, source, description);
}

void ThrowFailed(HRESULT hr, const std::wstring& source, const std::wstring& description, const std::wstring& helpFile, DWORD helpContext)
{
	if (FAILED(hr))
		throw HResultExeption(hr, source, description, helpFile, helpContext);
}

ATL::CComPtr<ICreateErrorInfo> CreateErrorInfo()
{
	ATL::CComPtr<ICreateErrorInfo> pICreateErrorInfo;
	ThrowFailed(::CreateErrorInfo(&pICreateErrorInfo));
	return pICreateErrorInfo;
}

void SetErrorInfo(const GUID& iid, BSTR source, BSTR description, BSTR helpFile, DWORD helpContext)
{
	auto pICreateErrorInfo = CreateErrorInfo();
	ThrowFailed(pICreateErrorInfo->SetGUID(iid));
	ThrowFailed(pICreateErrorInfo->SetSource(source));
	ThrowFailed(pICreateErrorInfo->SetDescription(description));
	ThrowFailed(pICreateErrorInfo->SetHelpFile(helpFile));
	ThrowFailed(pICreateErrorInfo->SetHelpContext(helpContext));
	ThrowFailed(SetErrorInfo(0, GetInterface<IErrorInfo>(pICreateErrorInfo)));
}

HRESULT HResultFromException(const GUID& iid)
{
	try
	{
		throw;
	}
	catch (HResultExeption& e)
	{
		SetErrorInfo(iid, ATL::CComBSTR(e.Source().c_str()), ATL::CComBSTR(e.Description().c_str()), ATL::CComBSTR(e.HelpFile().c_str()), e.HelpContext());
		return e.HResult();
	}
	catch (std::exception& e)
	{
		SetErrorInfo(iid, ATL::CComBSTR(), ATL::CComBSTR(e.what()), ATL::CComBSTR(), 0);
		return E_FAIL;
	}
	catch (...)
	{
		SetErrorInfo(iid, ATL::CComBSTR(), ATL::CComBSTR(), ATL::CComBSTR(), 0);
		return E_FAIL;
	}
}

HRESULT HResultFromException()
{
	return HResultFromException(IID_NULL);
}

} // namespace Win32
} // namespace DjeeDjay
