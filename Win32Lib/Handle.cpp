// (C) Copyright Gert-Jan de Vos 2021.

#include <memory>
#include <windows.h>
#include "DjeeDjay/Win32/Handle.h"
#include "DjeeDjay/Win32/Win32Error.h"

namespace DjeeDjay {
namespace Win32 {

void HLocalDeleter::operator()(HLOCAL p)
{
	::LocalFree(p);
}

void HGlobalDeleter::operator()(HGLOBAL p)
{
	::GlobalFree(p);
}

void HGlobalLockDeleter::operator()(pointer p)
{
	::GlobalUnlock(p);
}

HGlobalLock GlobalLock(HGLOBAL hGlobal)
{
	if (auto p = ::GlobalLock(hGlobal))
		return HGlobalLock(p);
	ThrowLastError("GlobalLock");
	return nullptr;
}

void VirtualAllocDeleter::operator()(void* p)
{
	::VirtualFree(p, 0, MEM_RELEASE);
}

void HandleDeleter::operator()(HANDLE h) const
{
	if (IsValidHandle(h))
		::CloseHandle(h);
}

bool IsValidHandle(void* h)
{
	return h != nullptr && h != INVALID_HANDLE_VALUE;
}

bool IsValidHandle(const Handle& h)
{
	return IsValidHandle(h.get());
}

} // namespace Win32
} // namespace DjeeDjay
