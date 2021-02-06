// (C) Copyright Gert-Jan de Vos 2021.

#pragma once

#include <atlcomcli.h>
#include "DjeeDjay/NonCopyable.h"
#include "DjeeDjay/Identity.h"
#include "DjeeDjay/Win32/HResultException.h"

namespace DjeeDjay {
namespace Win32 {

class ComInitialization : NonCopyable
{
public:
	enum CoInit
	{
		ApartmentThreaded,
		Multithreaded,
		ReuseOrApartmentThreaded,
		ReuseOrMultithreaded
	};

	explicit ComInitialization(CoInit coInit);
	~ComInitialization();

private:
	bool m_initialized;
};

class OleInitialization : NonCopyable
{
public:
	OleInitialization();
	~OleInitialization();
};

template <typename Itf>
ATL::CComPtr<Itf> CreateComInstance(const CLSID& clsid, IUnknown* pIUnkOuter = nullptr, DWORD dwClsContext = CLSCTX_ALL)
{
	ATL::CComPtr<Itf> pItf;
	ThrowFailed(pItf.CoCreateInstance(clsid, pIUnkOuter, dwClsContext));
	return pItf;
}

template <typename Itf>
ATL::CComPtr<Itf> CreateComInstance(const wchar_t* progID, IUnknown* pIUnkOuter = nullptr, DWORD dwClsContext = CLSCTX_ALL)
{
	ATL::CComPtr<Itf> pItf;
	ThrowFailed(pItf.CoCreateInstance(progID, pIUnkOuter, dwClsContext));
	return pItf;
}

template <typename Itf>
ATL::CComPtr<Itf> GetInterface(IUnknown* pIUnknown)
{
	ATL::CComPtr<Itf> pItf;
	if (pIUnknown)
		ThrowFailed(pIUnknown->QueryInterface(&pItf));
	return pItf;
}

class SafeArrayAccess : NonCopyable
{
public:
	explicit SafeArrayAccess(SAFEARRAY* pSa);
	~SafeArrayAccess();

	void* Ptr() const;

private:
	SAFEARRAY* m_pSa;
	void* m_pData;
};

template <typename Itf>
ATL::CComPtr<Itf> PSCreateMemoryPropertyStore()
{
	ATL::CComPtr<Itf> pItf;
	ThrowFailed(PSCreateMemoryPropertyStore(__uuidof(pItf), reinterpret_cast<void**>(&pItf)));
	return pItf;
}

template <typename T>
HRESULT ComRetVal(IdentityType<T> value, T* pRetVal)
{
	if (!pRetVal)
		return E_POINTER;
	*pRetVal = value;
	return S_OK;
}

template <typename Itf>
HRESULT ComRetVal(const ATL::CComPtr<Itf>& pItf, Itf** pRetVal)
{
	return pItf.CopyTo(pRetVal);
}

inline HRESULT ComRetVal(const ATL::CComBSTR& value, BSTR* pRetVal)
{
	return value.CopyTo(pRetVal);
}

} // namespace Win32
} // namespace DjeeDjay
