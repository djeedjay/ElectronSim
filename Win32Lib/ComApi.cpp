// (C) Copyright Gert-Jan de Vos 2021.

#include <cassert>
#include <windows.h>
#include "DjeeDjay/Win32/ComApi.h"

namespace DjeeDjay {
namespace Win32 {

namespace {

DWORD GetCoInit(ComInitialization::CoInit value)
{
	switch (value)
	{
	case ComInitialization::ApartmentThreaded:
	case ComInitialization::ReuseOrApartmentThreaded:
		return COINIT_APARTMENTTHREADED;

	case ComInitialization::Multithreaded:
	case ComInitialization::ReuseOrMultithreaded:
		 return COINIT_MULTITHREADED;

	default:
		assert(!"Unexpected CoInit");
		return value;
	}
}

bool IgnoreCoInitError(ComInitialization::CoInit value)
{
	switch (value)
	{
	case ComInitialization::ApartmentThreaded:
	case ComInitialization::Multithreaded:
		return false;

	case ComInitialization::ReuseOrApartmentThreaded:
	case ComInitialization::ReuseOrMultithreaded:
		 return true;

	default:
		assert(!"Unexpected CoInit");
		return false;
	}
}

} // namespace

ComInitialization::ComInitialization(CoInit coInit)
{
	auto hr = ::CoInitializeEx(nullptr, GetCoInit(coInit));
	m_initialized = SUCCEEDED(hr);

	if (!(hr == RPC_E_CHANGED_MODE && IgnoreCoInitError(coInit)))
		ThrowFailed(hr);
}

ComInitialization::~ComInitialization()
{
	if (m_initialized)
		::CoUninitialize();
}

} // namespace Win32
} // namespace DjeeDjay
