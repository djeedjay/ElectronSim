// (C) Copyright Gert-Jan de Vos 2021.

#pragma once

#include <memory>
#include <windows.h>

namespace DjeeDjay {
namespace Win32 {

struct HLocalDeleter
{
	typedef HLOCAL pointer;
	void operator()(HLOCAL p);
};

typedef std::unique_ptr<void, HLocalDeleter> HLocal;

struct HGlobalDeleter
{
	typedef HGLOBAL pointer;
	void operator()(HGLOBAL p);
};

typedef std::unique_ptr<void, HGlobalDeleter> HGlobal;

struct HGlobalLockDeleter
{
	typedef HGLOBAL pointer;
	void operator()(pointer p);
};

typedef std::unique_ptr<void, HGlobalLockDeleter> HGlobalLock;

HGlobalLock GlobalLock(HGLOBAL hGlobal);

struct VirtualAllocDeleter
{
	void operator()(void* p);
};

typedef std::unique_ptr<void, VirtualAllocDeleter> VirtualAllocPtr;

struct HandleDeleter
{
	void operator()(void* h) const;
};

typedef std::unique_ptr<void, HandleDeleter> Handle;

bool IsValidHandle(void* h);
bool IsValidHandle(const Handle& h);

} // namespace Win32
} // namespace DjeeDjay
