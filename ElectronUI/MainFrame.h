// (C) Copyright Gert-Jan de Vos 2021.

#pragma once

#include <atomic>
#include <thread>
#include <mutex>
#include <vector>
#include "DjeeDjay/Win32/AtlWinExt.h"
#include "DjeeDjay/Electron.h"
#include "DjeeDjay/Image.h"
#include "ShowError.h"
#include "ImageView.h"

namespace DjeeDjay {

struct KeyEvent
{
	static KeyEvent Down(ElectronKey key);
	static KeyEvent Up(ElectronKey key);

	enum class Type { Down, Up } type;
	ElectronKey key;
};

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
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnCpuException(UINT uCode, int nID, HWND hwndCtrl);
	void OnFrameCompleted(UINT uCode, int nID, HWND hwndCtrl);

	void OnFrameCompleted(const Image& image);
	void SendKeyEvent(const KeyEvent& event);
	void Run();

	CCommandBarCtrl m_cmdBar;
	CMenu m_hMenu;
	ImageView m_imageView;
	std::mutex m_mtx;
	Image m_image;
	std::atomic<bool> m_stop;
	std::thread m_thread;
	Electron m_electron;
	std::vector<KeyEvent> m_keys;
	std::atomic<bool> m_keysChanged;
	std::string m_cpuExceptionMessage;
};

} // namespace DjeeDjay
