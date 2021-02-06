// (C) Copyright Gert-Jan de Vos 2021.

#include "stdafx.h"
#include "resource.h"
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

} // namespace

BEGIN_UPDATE_UI_MAP2(MainFrame)
END_UPDATE_UI_MAP()

BEGIN_MSG_MAP2(MainFrame)
	MSG_WM_CREATE(OnCreate)
	MSG_WM_CLOSE(OnClose)
	CHAIN_MSG_MAP(CUpdateUI<MainFrame>)
	CHAIN_MSG_MAP(CFrameWindowImpl<MainFrame>)
END_MSG_MAP()

MainFrame::MainFrame() :
	m_image(640, 512),
	m_stop(false),
	m_electron(ExtractResourceData(IDR_OS_ROM))
{
	m_electron.InstallRom(ExtractResourceData(IDR_BASIC_ROM));

	uint32_t* p = m_image.Data();
	for (int y = 0; y < 256; ++y)
	{
		auto x = 5 * y / 2;
		p[2 * y * m_image.Width() + x] =
		p[2 * y * m_image.Width() + 639 - x] = RGB(0, 128, 255);
	}
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

void MainFrame::OnFrameCompleted()
{
}

void MainFrame::Run()
{
	m_electron.FrameCompleted([this]() { OnFrameCompleted(); });

	while (!m_stop)
		m_electron.Step();
}

} // namespace DjeeDjay
