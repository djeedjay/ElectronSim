// (C) Copyright Gert-Jan de Vos 2021.

#include "stdafx.h"
#include "resource.h"
#include "DjeeDjay/string_cast.h"
#include "MainFrame.h"

namespace DjeeDjay {

namespace {

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
	case VK_OEM_1: return ElectronKey::Semicolon;
	case VK_DIVIDE: return ElectronKey::Divides;
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

KeyEvent KeyEvent::Down(ElectronKey key)
{
	return KeyEvent{ Type::Down, key };
}

KeyEvent KeyEvent::Up(ElectronKey key)
{
	return KeyEvent{ Type::Up, key };
}

BEGIN_UPDATE_UI_MAP2(MainFrame)
END_UPDATE_UI_MAP()

BEGIN_MSG_MAP2(MainFrame)
	MSG_WM_CREATE(OnCreate)
	MSG_WM_CLOSE(OnClose)
	MSG_WM_KEYDOWN(OnKeyDown)
	MSG_WM_KEYUP(OnKeyUp)
	COMMAND_ID_HANDLER_EX(ID_CPU_EXCEPTION, OnCpuException)
	COMMAND_ID_HANDLER_EX(ID_FRAME_COMPLETED, OnFrameCompleted)
	CHAIN_MSG_MAP(CUpdateUI<MainFrame>)
	CHAIN_MSG_MAP(CFrameWindowImpl<MainFrame>)
END_MSG_MAP()

MainFrame::MainFrame() :
	m_image(640, 512),
	m_stop(false),
	m_electron(ExtractResourceData(IDR_OS_ROM)),
	m_keysChanged(false)
{
	m_electron.InstallRom(10, ExtractResourceData(IDR_BASIC_ROM));
}

BOOL MainFrame::OnIdle()
{
	//UpdateUI();
	UIUpdateToolBar();
	UIUpdateStatusBar();
	UIUpdateChildWindows();
	return FALSE;
}

LRESULT MainFrame::OnCreate(const CREATESTRUCT* /*pCreate*/)
{
	SetWindowText(L"Acorn Electron");

	HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(hIconSmall, FALSE);

	_Module.GetMessageLoop()->AddIdleHandler(this);

	HWND hWndCmdBar = m_cmdBar.Create(*this, rcDefault, nullptr, ATL_SIMPLE_CMDBAR_PANE_STYLE);

	m_cmdBar.AttachMenu(GetMenu());
	m_cmdBar.LoadImages(IDR_MAINFRAME);
	SetMenu(nullptr);

	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	CReBarCtrl rebar(m_hWndToolBar);
	AddSimpleReBarBand(hWndCmdBar);

	CreateSimpleStatusBar();
	UIAddStatusBar(m_hWndStatusBar);

	//HWND hWndToolBar = CreateSimpleToolBarCtrl(rebar, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
	//AddSimpleReBarBand(hWndToolBar, nullptr, true);
	//UIAddToolBar(hWndToolBar);
	rebar.LockBands(true);
	rebar.SetNotifyWnd(*this);

	UIAddChildWindowContainer(m_hWnd);

	m_hWndClient = m_imageView.Create(*this, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);
	m_imageView.BackgroundColor(GetSysColor(COLOR_BTNFACE));
	m_imageView.Update(m_image);

	m_thread = std::thread([this]() { Run(); });

	return TRUE;
}

void MainFrame::OnClose()
{
	m_stop = true;
	m_thread.join();

	DestroyWindow();
}

void MainFrame::OnKeyDown(UINT nChar, UINT /*nRepCnt*/, UINT /*nFlags*/)
{
	auto key = MakeElectronKey(nChar);
	if (key != ElectronKey::None)
		SendKeyEvent(KeyEvent::Down(key));
}

void MainFrame::OnKeyUp(UINT nChar, UINT /*nRepCnt*/, UINT /*nFlags*/)
{
	auto key = MakeElectronKey(nChar);
	if (key != ElectronKey::None)
		SendKeyEvent(KeyEvent::Up(key));
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

void MainFrame::OnFrameCompleted(const Image& image)
{
	std::unique_lock<std::mutex> lock(m_mtx);
	m_image = image;
	lock.unlock();

	PostMessage(WM_COMMAND, ID_FRAME_COMPLETED);
}

void MainFrame::SendKeyEvent(const KeyEvent& event)
{
	std::unique_lock<std::mutex> lock(m_mtx);
	m_keys.push_back(event);
	lock.unlock();
	m_keysChanged = true;
}

void MainFrame::Run()
{
	m_electron.Trace([](const std::string& msg) { OutputDebugStringA(msg.c_str()); });
	m_electron.FrameCompleted([this](const Image& image) { OnFrameCompleted(image); });

	m_electron.Reset();

	try
	{
		while (!m_stop)
		{
			if (m_keysChanged.exchange(false))
			{
				std::vector<KeyEvent> keys;
				std::unique_lock<std::mutex> lock(m_mtx);
				m_keys.swap(keys);
				lock.unlock();
				for (auto& key : keys)
				{
					switch (key.type)
					{
					case KeyEvent::Type::Down: m_electron.KeyDown(key.key); break;
					case KeyEvent::Type::Up: m_electron.KeyUp(key.key); break;
					}
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
