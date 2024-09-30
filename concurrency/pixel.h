#pragma once

#include <cstdint>

struct color {
	uint8_t a, b, g, r;
	color() { r = 0; g = 0; b = 0; a = 0; }
};

union pixel {
	color colorValue;
	uint32_t intValue;

	pixel() { intValue = 0; }
	pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		colorValue.r = r;
		colorValue.g = g;
		colorValue.b = b;
		colorValue.a = a;
	}
};

