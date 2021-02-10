// (C) Copyright Gert-Jan de Vos 2021.

#include "stdafx.h"
#include <vector>
#include <fstream>
#include <iterator>
#include "resource.h"
#include "DjeeDjay/string_cast.h"
#include "DjeeDjay/Win32/Clipboard.h"
#include "MainFrame.h"

namespace DjeeDjay {

namespace {

std::vector<uint8_t> Load(const std::wstring& path)
{
	std::vector<uint8_t> data;
	std::ifstream fs(path, std::ios::binary);
	std::copy(std::istreambuf_iterator<char>(fs), std::istreambuf_iterator<char>(), std::back_inserter(data));
	return data;
}

std::vector<uint8_t> ExtractResourceData(int resourceId)
{
	CResource pResource;
	if (!pResource.Load(RT_RCDATA, MAKEINTRESOURCE(resourceId)))
		Win32::ThrowLastError("RT_RCDATA");

	auto p = static_cast<const char*>(pResource.Lock());
	return std::vector<uint8_t>(p, p + pResource.GetSize());
}

std::string ToString(ElectronKey value)
{
	switch (value)
	{
	case ElectronKey::Right: return "Right";
	case ElectronKey::Copy: return "Copy";
	case ElectronKey::Space: return "Space";
	case ElectronKey::Left: return "Left";
	case ElectronKey::Down: return "Down";
	case ElectronKey::Return: return "Return";
	case ElectronKey::Delete: return "Delete";
	case ElectronKey::Minus: return "-";
	case ElectronKey::Up: return "Up";
	case ElectronKey::Colon: return ":";
	case ElectronKey::Semicolon: return ";";
	case ElectronKey::Divides: return "/";
	case ElectronKey::Num0: return "0";
	case ElectronKey::Num1: return "1";
	case ElectronKey::Num2: return "2";
	case ElectronKey::Num3: return "3";
	case ElectronKey::Num4: return "4";
	case ElectronKey::Num5: return "5";
	case ElectronKey::Num6: return "6";
	case ElectronKey::Num7: return "7";
	case ElectronKey::Num8: return "8";
	case ElectronKey::Num9: return "9";
	case ElectronKey::A: return "A";
	case ElectronKey::B: return "B";
	case ElectronKey::C: return "C";
	case ElectronKey::D: return "D";
	case ElectronKey::E: return "E";
	case ElectronKey::F: return "F";
	case ElectronKey::G: return "G";
	case ElectronKey::H: return "H";
	case ElectronKey::I: return "I";
	case ElectronKey::J: return "J";
	case ElectronKey::K: return "K";
	case ElectronKey::L: return "L";
	case ElectronKey::M: return "M";
	case ElectronKey::N: return "N";
	case ElectronKey::O: return "O";
	case ElectronKey::P: return "P";
	case ElectronKey::Q: return "Q";
	case ElectronKey::R: return "R";
	case ElectronKey::S: return "S";
	case ElectronKey::T: return "T";
	case ElectronKey::U: return "U";
	case ElectronKey::V: return "V";
	case ElectronKey::W: return "W";
	case ElectronKey::X: return "X";
	case ElectronKey::Y: return "Y";
	case ElectronKey::Z: return "Z";
	case ElectronKey::Dot: return ".";
	case ElectronKey::Comma: return ",";
	case ElectronKey::Escape: return "Escape";
	case ElectronKey::CapsLk: return "Caps Lk";
	case ElectronKey::Ctrl: return "Ctrl";
	case ElectronKey::Shift: return "Shift";
	default: return "Unknown";
	}
}

ElectronKey MakeElectronKey(UINT keyCode)
{
	switch (keyCode)
	{
	case VK_RIGHT: return ElectronKey::Right;
	case VK_END: return ElectronKey::Copy;
	case VK_SPACE: return ElectronKey::Space;
	case VK_LEFT: return ElectronKey::Left;
	case VK_DOWN: return ElectronKey::Down;
	case VK_RETURN: return ElectronKey::Return;
	case VK_BACK: return ElectronKey::Delete;
	case VK_OEM_MINUS: return ElectronKey::Minus;
	case VK_UP: return ElectronKey::Up;
//	case VK_OEM_1: return ElectronKey::Colon;
	case VK_OEM_7: return ElectronKey::Colon;
	case VK_OEM_1: return ElectronKey::Semicolon;
	case VK_DIVIDE: return ElectronKey::Divides;
	case VK_OEM_2: return ElectronKey::Divides;
	case VK_NUMPAD0: return ElectronKey::Num0;
	case VK_NUMPAD1: return ElectronKey::Num1;
	case VK_NUMPAD2: return ElectronKey::Num2;
	case VK_NUMPAD3: return ElectronKey::Num3;
	case VK_NUMPAD4: return ElectronKey::Num4;
	case VK_NUMPAD5: return ElectronKey::Num5;
	case VK_NUMPAD6: return ElectronKey::Num6;
	case VK_NUMPAD7: return ElectronKey::Num7;
	case VK_NUMPAD8: return ElectronKey::Num8;
	case VK_NUMPAD9: return ElectronKey::Num9;
	case '0': return ElectronKey::Num0;
	case '1': return ElectronKey::Num1;
	case '2': return ElectronKey::Num2;
	case '3': return ElectronKey::Num3;
	case '4': return ElectronKey::Num4;
	case '5': return ElectronKey::Num5;
	case '6': return ElectronKey::Num6;
	case '7': return ElectronKey::Num7;
	case '8': return ElectronKey::Num8;
	case '9': return ElectronKey::Num9;
	case 'A': return ElectronKey::A;
	case 'B': return ElectronKey::B;
	case 'C': return ElectronKey::C;
	case 'D': return ElectronKey::D;
	case 'E': return ElectronKey::E;
	case 'F': return ElectronKey::F;
	case 'G': return ElectronKey::G;
	case 'H': return ElectronKey::H;
	case 'I': return ElectronKey::I;
	case 'J': return ElectronKey::J;
	case 'K': return ElectronKey::K;
	case 'L': return ElectronKey::L;
	case 'M': return ElectronKey::M;
	case 'N': return ElectronKey::N;
	case 'O': return ElectronKey::O;
	case 'P': return ElectronKey::P;
	case 'Q': return ElectronKey::Q;
	case 'R': return ElectronKey::R;
	case 'S': return ElectronKey::S;
	case 'T': return ElectronKey::T;
	case 'U': return ElectronKey::U;
	case 'V': return ElectronKey::V;
	case 'W': return ElectronKey::W;
	case 'X': return ElectronKey::X;
	case 'Y': return ElectronKey::Y;
	case 'Z': return ElectronKey::Z;
	case VK_DECIMAL: return ElectronKey::Dot;
	case VK_OEM_PERIOD: return ElectronKey::Dot;
	case VK_OEM_COMMA: return ElectronKey::Comma;
	case VK_ESCAPE: return ElectronKey::Escape;
	case VK_CAPITAL: return ElectronKey::CapsLk;
	case VK_CONTROL: return ElectronKey::Ctrl;
	case VK_SHIFT: return ElectronKey::Shift;
	default: return ElectronKey::None;
	}
}

} // namespace

BEGIN_UPDATE_UI_MAP2(MainFrame)
	UPDATE_ELEMENT(IDM_ELECTRON_MUTE, UPDUI_MENUPOPUP)
	UPDATE_ELEMENT(0, UPDUI_STATUSBAR)
	UPDATE_ELEMENT(1, UPDUI_STATUSBAR)
	UPDATE_ELEMENT(2, UPDUI_STATUSBAR)
END_UPDATE_UI_MAP()

BEGIN_MSG_MAP2(MainFrame)
	MSG_WM_CREATE(OnCreate)
	MSG_WM_CLOSE(OnClose)
	MSG_WM_GETMINMAXINFO(OnGetMinMaxInfo)
	MSG_WM_KEYDOWN(OnKeyDown)
	MSG_WM_KEYUP(OnKeyUp)
	MSG_WM_CONTEXTMENU(OnContextMenu)
	MSG_WM_DROPFILES(OnDropFiles)
	COMMAND_ID_HANDLER_EX(IDM_FILE_INSERT_ROM, OnFileInsertRom)
	COMMAND_ID_HANDLER_EX(IDM_ELECTRON_MUTE, OnMute)
	COMMAND_ID_HANDLER_EX(IDM_ELECTRON_COPY_SCREEN, OnCopyScreen)
	COMMAND_ID_HANDLER_EX(IDM_ELECTRON_FULL_SCREEN, OnFullScreen)
	COMMAND_ID_HANDLER_EX(IDM_ELECTRON_BREAK, OnElectronBreak)
	COMMAND_ID_HANDLER_EX(IDM_ELECTRON_RESTART, OnElectronRestart)
	COMMAND_ID_HANDLER_EX(ID_CPU_EXCEPTION, OnCpuException)
	COMMAND_ID_HANDLER_EX(ID_FRAME_COMPLETED, OnFrameCompleted)
	COMMAND_ID_HANDLER_EX(ID_CAPSLOCK_CHANGED, OnCapsLockChanged)
	COMMAND_ID_HANDLER_EX(ID_CASSETTEMOTOR_CHANGED, OnCassetteMotorChanged)
	COMMAND_ID_HANDLER_EX(ID_PLAY_SOUND, OnPlaySound)
	CHAIN_MSG_MAP(CUpdateUI<MainFrame>)
	CHAIN_MSG_MAP(CFrameWindowImpl<MainFrame>)
END_MSG_MAP()

MainFrame::MainFrame() :
	m_mute(false),
	m_stop(false),
	m_electron(ExtractResourceData(IDR_OS_ROM)),
	m_qChanged(false)
{
	m_electron.InstallRom(10, ExtractResourceData(IDR_BASIC_ROM));
}

BOOL MainFrame::OnIdle()
{
	UISetCheck(IDM_ELECTRON_MUTE, m_mute);
	UIUpdateToolBar();
	UIUpdateStatusBar();
	UIUpdateChildWindows();
	return FALSE;
}

LRESULT MainFrame::OnCreate(const CREATESTRUCT* /*pCreate*/)
{
	HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(hIconSmall, FALSE);

	_Module.GetMessageLoop()->AddMessageFilter(this);
	_Module.GetMessageLoop()->AddIdleHandler(this);

	HWND hWndCmdBar = m_cmdBar.Create(*this, rcDefault, nullptr, ATL_SIMPLE_CMDBAR_PANE_STYLE);

	m_cmdBar.AttachMenu(GetMenu());
	m_cmdBar.LoadImages(IDR_MAINFRAME);
	SetMenu(nullptr);

	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	CReBarCtrl rebar(m_hWndToolBar);
	AddSimpleReBarBand(hWndCmdBar);

	m_hWndStatusBar = m_statusBar.Create(*this);
	UIAddStatusBar(m_hWndStatusBar);

	int paneIds[] = { ID_DEFAULT_PANE, ID_CAPSLOCK_PANE, ID_CASSETTEMOTOR_PANE };
	m_statusBar.SetPanes(paneIds, 3, false);

	//HWND hWndToolBar = CreateSimpleToolBarCtrl(rebar, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
	//AddSimpleReBarBand(hWndToolBar, nullptr, true);
	//UIAddToolBar(hWndToolBar);
	rebar.LockBands(true);
	rebar.SetNotifyWnd(*this);

	UIAddChildWindowContainer(m_hWnd);

	m_hWndClient = m_imageView.Create(*this, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);
	m_imageView.BackgroundColor(GetSysColor(COLOR_BTNFACE));

	CRect cmdBarRect;
	m_cmdBar.GetWindowRect(cmdBarRect);
	CRect statusRect;
	m_statusBar.GetWindowRect(statusRect);
	ResizeClient(640, 512 + cmdBarRect.Height() + statusRect.Height());
	CRect frameRect, clientRect;
	GetWindowRect(frameRect);
	GetClientRect(clientRect);
	auto sz = frameRect.Size() - clientRect.Size();
	m_frameSize = CPoint(640 + sz.cx, 512 + cmdBarRect.Height() + statusRect.Height() + sz.cy);

	m_thread = std::thread([this]() { Run(); });

	DragAcceptFiles(true);
	return TRUE;
}

BOOL MainFrame::PreTranslateMessage(MSG* pMsg)
{
	return CFrameWindowImpl::PreTranslateMessage(pMsg);
}

void MainFrame::OnClose()
{
	RunElectron([this]() { m_stop = true; });
	m_thread.join();

	DestroyWindow();
}

void MainFrame::OnGetMinMaxInfo(MINMAXINFO* pInfo)
{
	if (GetWindowLong(GWL_STYLE) & WS_OVERLAPPEDWINDOW)
	{
		pInfo->ptMinTrackSize = m_frameSize;
		pInfo->ptMaxTrackSize = m_frameSize;
	}
}

void MainFrame::OnKeyDown(UINT nChar, UINT /*nRepCnt*/, UINT /*nFlags*/)
{
	auto key = MakeElectronKey(nChar);
//	OutputDebugStringA(("OnKeyDown(" + std::to_string(nChar) + ") -> " + ToString(key) + "\n").c_str());
	if (key != ElectronKey::None)
		RunElectron([this, key]() { m_electron.KeyDown(key); });
	else
		SetMsgHandled(false);
}

void MainFrame::OnKeyUp(UINT nChar, UINT /*nRepCnt*/, UINT /*nFlags*/)
{
	auto key = MakeElectronKey(nChar);
	if (key != ElectronKey::None)
		RunElectron([this, key]() { m_electron.KeyUp(key); });
	else
		SetMsgHandled(false);
}

void MainFrame::OnContextMenu(HWND /*hWnd*/, POINT pt)
{
	if (CPoint(pt) == CPoint(-1, -1))
	{
		CRect rect;
		m_imageView.GetClientRect(&rect);
		pt = rect.CenterPoint();
		m_imageView.ClientToScreen(&pt);
	}

	auto GetEnable = [&](bool enable) { return enable ? MF_ENABLED : MF_DISABLED; };

	CMenu menu;
	menu.LoadMenu(IDR_CONTEXT_MENU);

	CMenuHandle menuPopup(menu.GetSubMenu(0));
	menuPopup.CheckMenuItem(IDM_ELECTRON_MUTE, m_mute ? MF_CHECKED : 0);
	menuPopup.CheckMenuItem(IDM_ELECTRON_FULL_SCREEN, (GetWindowLong(GWL_STYLE) & WS_OVERLAPPEDWINDOW) ? 0 : MF_CHECKED);
	menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, *this);
}

void MainFrame::OnDropFiles(HDROP hDropInfo)
{
	struct DragGuard
	{
		DragGuard(HDROP hDropInfo) :
			hDropInfo(hDropInfo)
		{
		}

		~DragGuard()
		{
			DragFinish(hDropInfo);
		}

		HDROP hDropInfo;
	} guard(hDropInfo);


	if (DragQueryFile(hDropInfo, 0xFFFFFFFF, nullptr, 0) == 1)
	{
		std::vector<wchar_t> filename(DragQueryFile(hDropInfo, 0, nullptr, 0) + 1);
		if (DragQueryFile(hDropInfo, 0, filename.data(), static_cast<unsigned>(filename.size())))
			InstallRom(filename.data());
	}
}

void MainFrame::OnFileInsertRom(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	std::array<COMDLG_FILTERSPEC, 1> filters =
	{{
		{ L"ROM Files (*.rom)", L"*.rom" }
	}};
	CShellFileOpenDialog dlg(L"", FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST, nullptr, filters.data(), static_cast<UINT>(filters.size()));
	if (dlg.DoModal(*this) == IDOK)
	{
		CString fileName;
		dlg.GetFilePath(fileName);

		InstallRom(static_cast<const wchar_t*>(fileName));
	}
}

void MainFrame::OnMute(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	m_mute = !m_mute;
	if (m_mute)
		m_speaker.Stop();
}

void MainFrame::OnCopyScreen(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	std::unique_lock<std::mutex> lock(m_mtx);
	int scale = m_image.Width() < 640 ? 1 : 2;
	auto image = Upscale(m_image, scale * 320 / m_image.Width(), scale * 256 / m_image.Height());
	lock.unlock();
	Win32::CopyToClipboard(image, *this);
}

void MainFrame::OnFullScreen(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	DWORD dwStyle = GetWindowLong(GWL_STYLE);
	if (dwStyle & WS_OVERLAPPEDWINDOW)
		SetFullscreen();
	else
		SetWindowed();
}

void MainFrame::OnElectronBreak(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	RunElectron([this]() { m_electron.Break(); });
}

void MainFrame::OnElectronRestart(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	RunElectron([this]() { m_electron.Restart(); });
}

void MainFrame::OnCpuException(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	MessageBox(WStr(m_cpuExceptionMessage), L"CPU Exception", MB_ICONERROR | MB_OK);
}

void MainFrame::OnFrameCompleted(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	std::unique_lock<std::mutex> lock(m_mtx);
	m_imageView.Update(m_image);
	lock.unlock();
}

void MainFrame::OnCapsLockChanged(UINT uCode, int /*nID*/, HWND /*hwndCtrl*/)
{
	UISetText(ID_CAPSLOCK_PANE, uCode ? L"CAPS LOCK" : L"");
}

void MainFrame::OnCassetteMotorChanged(UINT uCode, int /*nID*/, HWND /*hwndCtrl*/)
{
	UISetText(ID_CASSETTEMOTOR_PANE, uCode ? L"CASSETTE MOTOR" : L"");
}

void MainFrame::OnPlaySound(UINT uCode, int /*nID*/, HWND /*hwndCtrl*/)
{
	if (!m_mute)
	{
		if (uCode == 0)
			m_speaker.Stop();
		else
			m_speaker.Play(uCode);
	}
}

void MainFrame::SetFullscreen()
{
	MONITORINFO mi = { sizeof(mi) };
	if (GetWindowPlacement(&m_windowPlacement) && GetMonitorInfo(MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &mi))
	{
		m_imageView.BackgroundColor(RGB(0, 0, 0));
		DWORD dwStyle = GetWindowLong(GWL_STYLE);
		SetWindowLong(GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
		SetWindowPos(HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		::ShowWindow(m_hWndToolBar, SW_HIDE);
		::ShowWindow(m_hWndStatusBar, SW_HIDE);
		UpdateLayout();
	}
}

void MainFrame::SetWindowed()
{
	m_imageView.BackgroundColor(GetSysColor(COLOR_BTNFACE));
	DWORD dwStyle = GetWindowLong(GWL_STYLE);
	SetWindowLong(GWL_STYLE, dwStyle | (WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME));
	SetWindowPlacement(&m_windowPlacement);
	SetWindowPos(nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	::ShowWindow(m_hWndToolBar, SW_SHOW);
	::ShowWindow(m_hWndStatusBar, SW_SHOW);
	UpdateLayout();
}

void MainFrame::InstallRom(const std::wstring& filename)
{
	RunElectron([this, filename]()
	{
		m_electron.InstallRom(2, Load(filename.c_str()));
		m_electron.Break();
	});
}

void MainFrame::OnFrameCompleted(const Image& image)
{
	std::unique_lock<std::mutex> lock(m_mtx);
	m_image = image;
	lock.unlock();

	PostMessage(WM_COMMAND, ID_FRAME_COMPLETED);
}

void MainFrame::RunElectron(std::function<void ()> fn)
{
	std::unique_lock<std::mutex> lock(m_mtx);
	m_q.push_back(std::move(fn));
	lock.unlock();
	m_qChanged = true;
}

void MainFrame::Run()
{
	PostMessage(WM_COMMAND, MAKELONG(ID_CAPSLOCK_CHANGED, m_electron.CapsLock()));
	PostMessage(WM_COMMAND, MAKELONG(ID_CASSETTEMOTOR_CHANGED, m_electron.CassetteMotor()));

	m_electron.Trace([](const std::string& msg) { OutputDebugStringA(msg.c_str()); });
	m_electron.FrameCompleted([this](const Image& image) { OnFrameCompleted(image); });
	m_electron.CapsLock([this](bool value) { PostMessage(WM_COMMAND, MAKELONG(ID_CAPSLOCK_CHANGED, value)); });
	m_electron.CassetteMotor([this](bool value) { PostMessage(WM_COMMAND, MAKELONG(ID_CASSETTEMOTOR_CHANGED, value)); });
	m_electron.Speaker([this](int frequency) { PostMessage(WM_COMMAND, MAKELONG(ID_PLAY_SOUND, frequency)); });

	m_electron.Restart();

	try
	{
		while (!m_stop)
		{
			if (m_qChanged.exchange(false))
			{
				std::vector<std::function<void ()>> q;
				std::unique_lock<std::mutex> lock(m_mtx);
				m_q.swap(q);
				lock.unlock();
				for (auto& fn : q)
				{
					fn();
				}
			}
			m_electron.Step();
		}
	}
	catch (std::exception& ex)
	{
		m_cpuExceptionMessage = ex.what();
		PostMessage(WM_COMMAND, ID_CPU_EXCEPTION);
	}
}

} // namespace DjeeDjay
