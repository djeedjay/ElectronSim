// (C) Copyright Gert-Jan de Vos 2021.

#include "stdafx.h"
#include "DjeeDjay/Win32/HResultException.h"
#include "DjeeDjay/Image.h"
#include "DjeeDjay/Electron.h"
#include "ImageView.h"

#pragma comment(lib, "d2d1")

namespace DjeeDjay {

BEGIN_MSG_MAP2(ImageView)
	MSG_WM_SIZE(OnSize)
	MSG_WM_ERASEBKGND(OnEraseBkGnd)
	MSG_WM_PAINT(OnPaint)
	MESSAGE_HANDLER_EX(WM_KEYDOWN, SendToParent)
	MESSAGE_HANDLER_EX(WM_KEYUP, SendToParent)
END_MSG_MAP()

ImageView::ImageView()
{
	Win32::ThrowFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pIFactory));
}

COLORREF ImageView::BackgroundColor() const
{
	return m_bgColor;
}

void ImageView::BackgroundColor(COLORREF value)
{
	m_bgColor = value;
	Invalidate(FALSE);
}

void ImageView::Update(const Image& image)
{
	CreateGraphicsResources();

	D2D1_SIZE_U size = {};
	if (m_pIBitmap)
		size = m_pIBitmap->GetPixelSize();

	if (m_pIBitmap && size.width == static_cast<unsigned>(image.Width()) && size.height == static_cast<unsigned>(image.Height()))
	{
		auto rect = D2D1::RectU(0, 0, size.width, size.height);
		Win32::ThrowFailed(m_pIBitmap->CopyFromMemory(&rect, image.Data(), image.Stride()));
	}
	else
	{
		auto bmpProps = D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8X8_UNORM, D2D1_ALPHA_MODE_IGNORE));
		CComPtr<ID2D1Bitmap> pIBitmap;
		Win32::ThrowFailed(m_pIRenderTarget->CreateBitmap(D2D1::SizeU(image.Width(), image.Height()), image.Data(), image.Stride(), bmpProps, &pIBitmap));
		m_pIBitmap = pIBitmap;
	}
	Invalidate(FALSE);
}

void ImageView::OnSize(UINT /*type*/, SIZE size)
{
	if (m_pIRenderTarget != nullptr)
		m_pIRenderTarget->Resize(D2D1::SizeU(size.cx, size.cy));
	Invalidate(FALSE);
}

BOOL ImageView::OnEraseBkGnd(HDC /*hdc*/)
{
	return TRUE;
}

void ImageView::OnPaint(HDC hdc)
{
	if (hdc)
	{
		Paint(hdc);
	}
	else
	{
		CClientDC dc(*this);
		Paint(dc);
	}
}

void ImageView::Destroy()
{
	DiscardGraphicsResources();
}

void ImageView::Paint(HDC /*hdc*/)
{
	CRect rect;
	GetClientRect(rect);

	CreateGraphicsResources();

	PAINTSTRUCT ps;
	BeginPaint(&ps);

	m_pIRenderTarget->BeginDraw();
	auto viewSize = m_pIRenderTarget->GetSize();
	auto imageSize = m_pIBitmap->GetPixelSize();

	float scale = std::min(viewSize.width / imageSize.width, viewSize.height / imageSize.height);
	float width = scale * imageSize.width;
	float height = scale * imageSize.height;

	m_pIBrush->SetColor(D2D1::ColorF(m_bgColor, 1.0f));
	m_pIRenderTarget->FillRectangle(D2D1::RectF(0, 0, viewSize.width, viewSize.height), m_pIBrush);

	m_pIRenderTarget->DrawBitmap(m_pIBitmap, D2D1::RectF((viewSize.width - width)/2, (viewSize.height - height)/2, (viewSize.width + width)/2, (viewSize.height + height)/2));

	auto hr = m_pIRenderTarget->EndDraw();
	if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
	{
		DiscardGraphicsResources();
	}

	EndPaint(&ps);
}

LRESULT ImageView::SendToParent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return GetParent().SendMessage(uMsg, wParam, lParam);
}

void ImageView::CreateGraphicsResources()
{
	if (m_pIRenderTarget != nullptr)
		return;

	CRect rect;
	GetClientRect(rect);

	Win32::ThrowFailed(m_pIFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(*this, D2D1::SizeU(rect.right, rect.bottom)), &m_pIRenderTarget));
	Win32::ThrowFailed(m_pIRenderTarget->CreateSolidColorBrush(D2D1::ColorF(m_bgColor, 1.0f), &m_pIBrush));
}

void ImageView::DiscardGraphicsResources()
{
	m_pIRenderTarget = nullptr;
	m_pIBrush = nullptr;
	m_pIBitmap = nullptr;
}

} // namespace DjeeDjay
