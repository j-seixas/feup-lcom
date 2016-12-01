#include "tools.h"

int rgb(unsigned char r, unsigned char g, unsigned char b) {
	if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255)
		return -1;

	unsigned char red = r >> 3;
	unsigned char green = g >> 2;
	unsigned char blue = b >> 3;

	return (red << (5 + 6)) | (green << 5) | blue;
}
