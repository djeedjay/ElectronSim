// Copyright (c) 2018 by Phenom-World B.V.
// All rights reserved. This file includes confidential and proprietary information of Phenom-World B.V.

#pragma once

#include <string>
#include "DjeeDjay/NonCopyable.h"
#include "DjeeDjay/Win32/Handle.h"
#include "DjeeDjay/Image.h"

namespace DjeeDjay {

class ConstImageRef;

namespace Win32 {

class Clipboard : NonCopyable
{
public:
	explicit Clipboard(HWND owner);
	~Clipboard();

	void Clear();

	HGlobalLock GetData(unsigned format);

	void Add(const std::wstring& format, const void* data, size_t size);
	void Add(UINT format, const void* data, size_t size);
	void Add(const std::wstring& format, const std::string& text);
	void Add(const std::wstring& format, const std::wstring& text);
	void Add(UINT format, const std::string& text);
	void Add(UINT format, const std::wstring& text);
	void Add(const std::string& text);
	void Add(const std::wstring& text);
	void Add(const Image& image);
};

void CopyToClipboard(const std::wstring& format, const void* data, size_t size, HWND owner);
void CopyToClipboard(UINT format, const void* data, size_t size, HWND owner);
void CopyToClipboard(const std::string& text, HWND owner);
void CopyToClipboard(const std::wstring& text, HWND owner);
void CopyToClipboard(const Image& image, HWND owner);

} // namespace Win32
} // namespace DjeeDjay
