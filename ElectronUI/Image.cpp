// (C) Copyright Gert-Jan de Vos 2021.

#include "Image.h"

namespace DjeeDjay {

Image::Image(int width, int height) :
	m_width(width),
	m_height(height),
	m_data(height * width)
{
}

int Image::Width() const
{
	return m_width;
}

int Image::Height() const
{
	return m_height;
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

} // namespace DjeeDjay
