// (C) Copyright Gert-Jan de Vos 2021.

#include <utility>
#include <iterator>
#include <sstream>
#include <iomanip>
#include "DjeeDjay/Win32/Handle.h"
#include "DjeeDjay/Win32/Win32Error.h"
#include "DjeeDjay/Win32/Clipboard.h"

namespace DjeeDjay {
namespace Win32 {

Clipboard::Clipboard(HWND owner)
{
	if (!OpenClipboard(owner))
		ThrowLastError("OpenClipboard");
}

Clipboard::~Clipboard()
{
	CloseClipboard();
}

void Clipboard::Clear()
{
	EmptyClipboard();
}

HGlobalLock Clipboard::GetData(unsigned format)
{
	if (auto hData = GetClipboardData(format))
		return Win32::GlobalLock(hData);

	return HGlobalLock();
}

void Clipboard::Add(const std::wstring& format, const void* data, size_t size)
{
	UINT formatId = RegisterClipboardFormat(format.c_str());
	if (formatId != 0)
		Add(formatId, data, size);
}

void Clipboard::Add(UINT format, const void* data, size_t size)
{
	Win32::HGlobal hdst(GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, size));
	auto lock = Win32::GlobalLock(hdst.get());
	{
		auto src = static_cast<const char*>(data);
		auto dst = static_cast<char*>(lock.get());
		std::copy(src, src + size, stdext::checked_array_iterator<char*>(dst, size));
	}
	SetClipboardData(format, hdst.release());
}

void Clipboard::Add(const std::wstring& format, const std::string& text)
{
	Add(format, text.data(), text.size());
}

void Clipboard::Add(const std::wstring& format, const std::wstring& text)
{
	Add(format, text.data(), text.size() * sizeof(wchar_t));
}

void Clipboard::Add(UINT format, const std::string& text)
{
	Add(format, text.data(), text.size());
}

void Clipboard::Add(UINT format, const std::wstring& text)
{
	Add(format, text.data(), text.size() * sizeof(wchar_t));
}

void Clipboard::Add(const std::string& text)
{
	Add(CF_TEXT, text.c_str(), text.size() + 1);
}

void Clipboard::Add(const std::wstring& text)
{
	Add(CF_UNICODETEXT, text.c_str(), (text.size() + 1) * sizeof(wchar_t));
}

void Clipboard::Add(const Image& image)
{
	auto size = image.Stride() * image.Height();
	Win32::HGlobal hMem(GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(BITMAPINFOHEADER) + size));
	if (!hMem)
		Win32::ThrowLastError("GlobalAlloc");

	auto lock = Win32::GlobalLock(hMem.get());
	BITMAPINFO& bmpInfo = *reinterpret_cast<BITMAPINFO*>(lock.get());

	ZeroMemory(&bmpInfo, sizeof(BITMAPINFOHEADER));
	bmpInfo.bmiHeader.biSize = sizeof(bmpInfo.bmiHeader);
	bmpInfo.bmiHeader.biWidth = image.Width();
	bmpInfo.bmiHeader.biHeight = -image.Height();
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	bmpInfo.bmiHeader.biSizeImage = size;
	bmpInfo.bmiHeader.biXPelsPerMeter = 0;
	bmpInfo.bmiHeader.biYPelsPerMeter = 0;
	bmpInfo.bmiHeader.biClrUsed = 0;
	bmpInfo.bmiHeader.biClrImportant = 0;

	void* bmpData = static_cast<char*>(lock.get()) + sizeof(BITMAPINFOHEADER);
	auto src = image.Data();
	auto dst = static_cast<uint32_t*>(bmpData);
	for (int y = 0; y < image.Height(); ++y)
		for (int x = 0; x < image.Width(); ++x)
			*dst++ = *src++ | 0xFF000000;
	lock.reset();

	SetClipboardData(CF_DIB, hMem.release());
}

void CopyToClipboard(const std::wstring& format, const void* data, size_t size, HWND owner)
{
	Clipboard clipboard(owner);
	clipboard.Clear();
	clipboard.Add(format, data, size);
}

void CopyToClipboard(UINT format, const void* data, size_t size, HWND owner)
{
	Clipboard clipboard(owner);
	clipboard.Clear();
	clipboard.Add(format, data, size);
}

void CopyToClipboard(const std::string& text, HWND owner)
{
	Clipboard clipboard(owner);
	clipboard.Clear();
	clipboard.Add(text);
}

void CopyToClipboard(const std::wstring& text, HWND owner)
{
	Clipboard clipboard(owner);
	clipboard.Clear();
	clipboard.Add(text);
}

void CopyToClipboard(const Image& image, HWND owner)
{
	Clipboard clipboard(owner);
	clipboard.Clear();
	clipboard.Add(image);
}

} // namespace Win32
} // namespace DjeeDjay
