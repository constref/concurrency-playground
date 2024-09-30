#pragma once
#include <vector>
#include <numbers>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "lib/stb/stb_image_write.h"


struct Pixel
{
	uint8_t r, g, b;
};

class ImageGen
{
	int width, height;
public:
	ImageGen(int width, int height) : width(width), height(height) {}

	auto generateImage()
	{
		std::vector<Pixel> pixels(width * height);

		int xStart = width / 2;
		int yStart = height / 2;

		for (int r = 1; r < std::max(width, height) / 2; r += 1)
		{
			for (double theta = 0; theta < std::numbers::pi * 2; theta += (std::numbers::pi * 2) / (360 * 4))
			{
				int x = static_cast<int>(r * cos(theta));
				int y = static_cast<int>(r * sin(theta));

				x = std::clamp(x, (-width / 2), (width / 2) - 1);
				y = std::clamp(y, (-height / 2), (height / 2) - 1);
				Pixel &p = pixels[(yStart + y) * width + (xStart + x)];

				p.r = static_cast<uint8_t>(sin(theta) * 255);
				p.g = static_cast<uint8_t>(cos(theta) * 255);
				p.b = static_cast<uint8_t>(sin(theta) * 255);
			}
		}
		return pixels;
	}

	void exportPixels(const std::string &filename, const std::vector<Pixel> &pixels)
	{
		stbi_write_jpg("output.jpg", width, height, 3, (char *)pixels.data(), 100);
	}
};