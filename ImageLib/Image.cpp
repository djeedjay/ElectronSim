// (C) Copyright Gert-Jan de Vos 2021.

#include "DjeeDjay/Image.h"

namespace DjeeDjay {

Image::Image() :
	m_width(0),
	m_height(0)
{
}

void Image::Resize(int width, int height)
{
	if (width != m_width || height != m_height)
	{
		m_width = width;
		m_height = height;
		m_data.resize(height * width);
	}
}

int Image::Width() const
{
	return m_width;
}

int Image::Height() const
{
	return m_height;
}

uint32_t Image::operator()(int x, int y) const
{
	return m_data[y * m_width + x];
}

uint32_t& Image::operator()(int x, int y)
{
	return m_data[y * m_width + x];
}

uint32_t* Image::Data()
{
	return m_data.data();
}

const uint32_t* Image::Data() const
{
	return m_data.data();
}

int Image::Stride() const
{
	return m_width * sizeof(uint32_t);
}

Image Upscale(const Image& image, int nx, int ny)
{
	Image result;
	result.Resize(nx * image.Width(), ny * image.Height());
	for (int y = 0; y < image.Height(); ++y)
		for (int iy = 0; iy < ny; ++iy)
			for (int x = 0; x < image.Width(); ++x)
				for (int ix = 0; ix < nx; ++ix)
					result(nx * x + ix, ny * y + iy) = image(x, y);
	return result;
}

} // namespace DjeeDjay
