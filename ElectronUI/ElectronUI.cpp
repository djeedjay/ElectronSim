// (C) Copyright Gert-Jan de Vos 2021.

#include "stdafx.h"
#include "DjeeDjay/Win32/ComApi.h"
#include "resource.h"
#include "MainFrame.h"

CAppModule _Module;

namespace DjeeDjay {

int Main(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpstrCmdLine*/, int nCmdShow)
{
	Win32::ComInitialization com(Win32::ComInitialization::ApartmentThreaded);

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(nullptr, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	Win32::AppModuleInitializer module(_Module, nullptr, hInstance);
	Win32::MessageLoop theLoop(_Module);

	MainFrame mainFrame;

	if (mainFrame.CreateEx(nullptr) == nullptr)
		Win32::ThrowLastError("Main dialog creation failed!\n");

	mainFrame.ShowWindow(nCmdShow);

	return theLoop.Run();
}

} // namespace DjeeDjay

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpstrCmdLine, int nCmdShow)
try
{
	return DjeeDjay::Main(hInstance, hPrevInstance, lpstrCmdLine, nCmdShow);
}
catch (std::exception& ex)
{
	DjeeDjay::ShowError(nullptr, ex.what(), MB_ICONERROR | MB_OK);
	return -1;
}
