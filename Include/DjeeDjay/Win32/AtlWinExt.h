// (C) Copyright Gert-Jan de Vos 2021.

#pragma once

#include "DjeeDjay/Win32/HResultException.h"

namespace WTL {
struct _AtlDlgResizeMap;
} // namespace WTL;

#define DECLARE_DIALOG() \
	void DoInitTemplate();

#define BEGIN_DIALOG2(theClass, x, y, width, height) \
	void theClass::DoInitTemplate() \
	{ \
		bool bExTemplate = false; \
		short nX = x, nY = y, nWidth = width, nHeight = height; \
		LPCTSTR szCaption = NULL; \
		DWORD dwStyle = WS_POPUP | WS_BORDER | WS_SYSMENU; \
		DWORD dwExStyle = 0; \
		LPCTSTR szFontName = NULL; \
		WORD wFontSize = 0; \
		WORD wWeight = 0; \
		BYTE bItalic = 0; \
		BYTE bCharset = 0; \
		DWORD dwHelpID = 0; \
		ATL::_U_STRINGorID Menu = 0U; \
		ATL::_U_STRINGorID ClassName = 0U;

#define DECLARE_CONTROLS_MAP() \
	void DoInitControls();

#define BEGIN_CONTROLS_MAP2(theClass) \
	void theClass::DoInitControls() \
	{

#define DECLARE_DLGRESIZE_MAP() \
	static const _AtlDlgResizeMap* GetDlgResizeMap();

#define BEGIN_DLGRESIZE_MAP2(thisClass) \
	const _AtlDlgResizeMap* thisClass::GetDlgResizeMap() \
	{ \
		static const _AtlDlgResizeMap theMap[] = \
		{

#define DECLARE_UPDATE_UI_MAP() \
	static const CUpdateUIBase::_AtlUpdateUIMap* GetUpdateUIMap();

#define BEGIN_UPDATE_UI_MAP2(thisClass) \
	const CUpdateUIBase::_AtlUpdateUIMap* thisClass::GetUpdateUIMap() \
	{ \
		static const CUpdateUIBase::_AtlUpdateUIMap theMap[] = \
		{

#define DECLARE_MSG_MAP() \
	template <typename T, typename ... Exceptions> \
	friend class ::DjeeDjay::Win32::ExceptionHandler; \
	 \
	virtual BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID); \
	BOOL ProcessWindowMessageImpl2(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID);

// Alternative to ATL standard BEGIN_MSG_MAP() with try block:
#define BEGIN_MSG_MAP2(theClass) \
	BOOL theClass::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID) \
	try \
	{ \
		return ProcessWindowMessageImpl(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID); \
	} \
	catch (...) \
	{ \
		OnException(); \
		return FALSE; \
	} \
	 \
	BOOL theClass::ProcessWindowMessageImpl2(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID) \
	{ \
		BOOL bHandled = TRUE; \
		(hWnd); \
		(uMsg); \
		(wParam); \
		(lParam); \
		(lResult); \
		(bHandled); \
		switch(dwMsgMapID) \
		{ \
		case 0:

namespace DjeeDjay {
namespace Win32 {

template <typename T, typename ... Exceptions>
class ExceptionHandler;

template <typename T, typename Exception, typename ... Tail>
struct ExceptionHandler<T, Exception, Tail ...> : ExceptionHandler<T, Tail ...>
{
	BOOL ProcessWindowMessageImpl(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
	try
	{
		return ExceptionHandler<T, Tail ...>::ProcessWindowMessageImpl(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID);
	}
	catch (Exception& ex)
	{
		static_cast<T*>(this)->OnException(ex);
		return FALSE;
	}
};

template <typename T>
struct ExceptionHandler<T>
{
	BOOL ProcessWindowMessageImpl(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID)
	{
		return static_cast<T*>(this)->ProcessWindowMessageImpl2(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID);
	}
};

class AppModuleInitializer
{
public:
	AppModuleInitializer(WTL::CAppModule& module, ATL::_ATL_OBJMAP_ENTRY* pObjMap, HINSTANCE hInstance, const GUID* pLibID = nullptr) :
		m_module(module)
	{
		ThrowFailed(m_module.Init(pObjMap, hInstance, pLibID));
	}

	~AppModuleInitializer()
	{
		m_module.Term();
	}

private:
	WTL::CAppModule& m_module;
};

class MessageLoop : public WTL::CMessageLoop
{
public:
	explicit MessageLoop(WTL::CAppModule& module) :
		m_module(module)
	{
		m_module.AddMessageLoop(this);
	}

	~MessageLoop()
	{
		m_module.RemoveMessageLoop();
	}

private:
	WTL::CAppModule& m_module;
};

} // namespace Win32
} // namespace DjeeDjay
