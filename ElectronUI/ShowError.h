// (C) Copyright Gert-Jan de Vos 2021.

#pragma once

#include <string>
#include "DjeeDjay/Win32/AtlWinExt.h"

namespace DjeeDjay {

void ShowError(HWND hwndParent, const std::string& message, unsigned flags = MB_ICONERROR | MB_OKCANCEL);
void ShowError(HWND hwndParent, const std::wstring& message, unsigned flags = MB_ICONERROR | MB_OKCANCEL);

template <typename T>
class ExceptionHandlerImpl : public Win32::ExceptionHandler<T, std::exception, std::bad_alloc>
{
public:
	void OnException()
	{
		DoShowError("Unknown exception");
	}

	void OnException(const std::bad_alloc&)
	{
		DoShowError("Out of memory");
	}

	void OnException(const std::exception& ex)
	{
		DoShowError(ex.what());
	}

private:
	void DoShowError(const std::string& message)
	{
		unsigned flags = MB_ICONERROR | MB_OK;
		if (message == m_lastErrorMsg)
			flags |= MB_OKCANCEL;
		m_lastErrorMsg = message;
		ShowError(*static_cast<const T*>(this), message, flags);
	}

	std::string m_lastErrorMsg;
};

} // namespace DjeeDjay
