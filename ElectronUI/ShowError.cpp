// (C) Copyright Gert-Jan de Vos 2021.

#include "stdafx.h"
#include <cstdlib>
#include "DjeeDjay/string_cast.h"
#include "DjeeDjay/Win32/Resources.h"
#include "resource.h"
#include "ShowError.h"

namespace DjeeDjay {

void ShowError(HWND hwndParent, const std::string& message, unsigned flags)
{
	ShowError(hwndParent, string_cast<std::wstring>(message), flags);
}

void ShowError(HWND hwndParent, const std::wstring& message, unsigned flags)
{
	if (::MessageBoxW(hwndParent, message.c_str(), Win32::LoadString(IDR_MAINFRAME).c_str(), flags) == IDCANCEL)
		std::exit(EXIT_FAILURE);
}

} // namespace DjeeDjay
