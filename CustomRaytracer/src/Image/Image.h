#pragma once
#include <vector>
#include <string>
#include "../Units/Units.h"

enum FileType {
	Grayscale = 1,
	RGB = 3,
	RGBA = 4,
};

class Image {
public:
	unsigned char* pixels;
	int height, width;
	FileType filetype;

	Image(unsigned int width, unsigned int height, FileType filetype);
	Image(const Color& single_color);
	Image(std::string filename, FileType filetype);
	~Image();

	void compile(const std::string& filename, bool png = true);
	//inline unsigned char* get_pixels() { return (this->pixels); };
	inline unsigned char* get_pixel(int y, int x) const {
		return (this->pixels) + (y)*(this->width)*this->filetype + (x)*this->filetype; 
	};

	void set_pixel(unsigned int y, unsigned int x, unsigned char* channels);
};