// (C) Copyright Gert-Jan de Vos 2021.

#pragma once

#include <atomic>
#include <thread>
#include "DjeeDjay/Win32/AtlWinExt.h"
#include "DjeeDjay/Electron.h"
#include "ShowError.h"
#include "Image.h"
#include "ImageView.h"

namespace DjeeDjay {

class MainFrame :
	public CFrameWindowImpl<MainFrame>,
	public CUpdateUI<MainFrame>,
	public CIdleHandler,
	public ExceptionHandlerImpl<MainFrame>
{
public:
	DECLARE_FRAME_WND_CLASS(nullptr, IDR_MAINFRAME)
	DECLARE_UPDATE_UI_MAP()

	MainFrame();

private:
	DECLARE_MSG_MAP()

	BOOL OnIdle() override;

	LRESULT OnCreate(const CREATESTRUCT* pCreate);
	void OnClose();

	void OnFrameCompleted();

	void Run();

	CCommandBarCtrl m_cmdBar;
	CMenu m_hMenu;
	ImageView m_imageView;
	Image m_image;
	std::atomic<bool> m_stop;
	std::thread m_thread;
	Electron m_electron;
};

} // namespace DjeeDjay
