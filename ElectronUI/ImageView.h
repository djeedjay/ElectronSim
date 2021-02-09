// (C) Copyright Gert-Jan de Vos 2021.

#pragma once

#include <atlcomcli.h>
#include <d2d1.h>
#include "DjeeDjay/Win32/AtlWinExt.h"
#include "ShowError.h"

namespace DjeeDjay {

class Image;

class ImageView :
	public CWindowImpl<ImageView, CWindow>,
	public ExceptionHandlerImpl<ImageView>
{
public:
	ImageView();

	COLORREF BackgroundColor() const;
	void BackgroundColor(COLORREF value);
	void Update(const Image& image);

private:
	DECLARE_MSG_MAP()

	void OnSize(UINT type, SIZE size);
	BOOL OnEraseBkGnd(HDC hdc);
	void OnPaint(HDC dc);
	LRESULT SendToParent(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void Destroy();
	void Paint(HDC hdc);

	COLORREF m_bgColor;
	CComPtr<ID2D1Factory> m_pIFactory;
	CComPtr<ID2D1HwndRenderTarget> m_pIRenderTarget;
	CComPtr<ID2D1SolidColorBrush> m_pIBrush;
	CComPtr<ID2D1Bitmap> m_pIBitmap;
	void CreateGraphicsResources();
	void DiscardGraphicsResources();
};

} // namespace DjeeDjay
