// (C) Copyright Gert-Jan de Vos 2021.

#pragma once

#include <cstdint>
#include <cstdint>
#include <vector>

namespace DjeeDjay {

class Image
{
public:
	Image();

	void Resize(int width, int height);

	int Width() const;
	int Height() const;

	uint32_t operator()(int x, int y) const;
	uint32_t& operator()(int x, int y);

	uint32_t* Data();
	const uint32_t* Data() const;
	int Stride() const;

private:
	int m_width;
	int m_height;
	std::vector<uint32_t> m_data;
};

Image Upscale(const Image& image, int nx, int ny);

} // namespace DjeeDjay
