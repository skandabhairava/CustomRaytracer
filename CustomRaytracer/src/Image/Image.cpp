#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

#include "Image.h"
#include "../Helper.h"

Image::Image(unsigned int width, unsigned int height, FileType filetype)
	:height(height), width(width), filetype(filetype)
{
	this->pixels = new unsigned char[height*width*filetype];
	memset(this->pixels, 0, this->width*this->height*filetype);
}

Image::Image(const Color& single_color)
{
	this->height = 1;
	this->width = 1;
	
	unsigned char* channels = nullptr;

	if (single_color.r == single_color.g && single_color.g == single_color.b) {
		this->filetype = FileType::Grayscale;
		channels = new unsigned char[FileType::Grayscale];
		channels[0] = single_color.r;
	}
	else if (single_color.a == 1.0) {
		this->filetype = FileType::RGB;
		channels = new unsigned char[FileType::RGB];
		channels[0] = single_color.r;
		channels[1] = single_color.g;
		channels[2] = single_color.b;
	}
	else {
		this->filetype = FileType::RGBA;
		channels = new unsigned char[FileType::RGBA];
		channels[0] = single_color.r;
		channels[1] = single_color.g;
		channels[2] = single_color.b;
		channels[3] = single_color.a*255;
	}

	this->pixels = new unsigned char[this->height * this->width * this->filetype];

	this->set_pixel(0, 0, channels);
	delete[] channels;
}

Image::Image(std::string filename, FileType filetype)
{

	ASSERT(filetype == FileType::RGBA || filetype == FileType::Grayscale);

	this->filetype = filetype;

	std::ifstream img;
	img.open(filename + ".pam", std::fstream::binary);

	if (!img.good()) {
		img.close();

		ASSERT(false); // image name might be wrong
	}

	/*
		P7
		WIDTH _
		HEIGHT _
		DEPTH 4
		MAXVAL 255
		TUPLTYPE RGB_ALPHA
		ENDHDR
	*/

	std::string line;
	std::stringstream s_line;
	std::string type;
	short unsigned int val;

	ASSERT(std::getline(img, line));
	//std::cout << line << std::endl;
	ASSERT(line == "P7");

	ASSERT(std::getline(img, line));
	//std::cout << line << std::endl;
	s_line << line;
	ASSERT(s_line >> type >> val);
	ASSERT(type == "WIDTH");
	this->width = val;
	s_line.clear();

	ASSERT(std::getline(img, line));
	//std::cout << line << std::endl;
	s_line << line;
	ASSERT(s_line >> type >> val);
	ASSERT(type == "HEIGHT");
	this->height = val;
	s_line.clear();

	ASSERT(std::getline(img, line));
	//std::cout << line << std::endl;
	s_line << line;
	ASSERT(s_line >> type >> val);
	ASSERT(type == "DEPTH" && val == 4);
	s_line.clear();

	ASSERT(std::getline(img, line));
	//std::cout << line << std::endl;
	s_line << line;
	ASSERT(s_line >> type >> val);
	ASSERT(type == "MAXVAL" && val == 255);
	s_line.clear();

	ASSERT(std::getline(img, line));
	//std::cout << line << std::endl;
	s_line << line;
	ASSERT(s_line >> type);
	ASSERT(type == "TUPLTYPE");
	ASSERT(s_line >> type);
	ASSERT(type == "RGB_ALPHA");
	s_line.clear();

	ASSERT(std::getline(img, line));
	//std::cout << line << std::endl;
	s_line << line;
	ASSERT(s_line >> type);
	ASSERT(type == "ENDHDR");
	s_line.clear();

	this->pixels = new unsigned char[height * width * this->filetype];
	unsigned char channels[4];

	for (int i = 0; i < this->width * this->height; i++) {
		// [(i//h, i%h) for i in l] where l is width * heights
		int x = (i / this->height); // col
		int z = (i % this->height); // row

		//unsigned char* slice = new unsigned char[4];

		img.read((char*)channels, 4);
		this->set_pixel(z, x, channels);
	}

	img.close();
}

Image::~Image()
{
	delete[] this->pixels;
}

void Image::compile(const std::string& filename, bool png)
{
	// create and open a new file
	std::ofstream img;
	std::stringstream out;

	switch (this->filetype) {

	case FileType::RGBA:
		img.open(filename + ".pam", std::ios::binary);
		if (!img.good()) {
			ASSERT(false);
		}

		out << "P7\nWIDTH " << this->width << "\nHEIGHT " << this->height << "\nDEPTH 4\nMAXVAL 255\nTUPLTYPE RGB_ALPHA\nENDHDR\n";
		for (int i = 0; i < this->height; i++) {
			for (int j = 0; j < this->width; j++)
				for (int rgb = 0; rgb < this->filetype; rgb++)
					out << (char)*(this->get_pixel(i, j) + rgb);
			//img << std::endl;
		}
		img << out.str();

		img.close();
		// echo y | ffmpeg -i test.ppm test.png
		if (png) {
			system((std::string("echo y | ffmpeg -i ") + filename + ".pam " + filename + ".png >nul 2>nul").c_str());
			remove((filename + ".pam").c_str());
		}

		break;


	case FileType::RGB:
		img.open(filename + ".ppm");
		if (!img.good()) {
			ASSERT(false);
		}

		out << "P3\n" << this->width << " " << this->height << "\n" << "255\n";
		for (int i = 0; i < this->height; i++) {
			for (int j = 0; j < this->width; j++)
				for (int rgb = 0; rgb < this->filetype; rgb++)
					out << (int)*(this->get_pixel(i, j) + rgb) << " ";
			//img << std::endl;
		}
		img << out.str();

		img.close();
		// echo y | ffmpeg -i test.ppm test.png
		if (png) {
			system((std::string("echo y | ffmpeg -i ") + filename + ".ppm " + filename + ".png >nul 2>nul").c_str());
			remove((filename + ".ppm").c_str());
		}

		break;
	}

}

void Image::set_pixel(unsigned int y, unsigned int x, unsigned char* channels) {
	unsigned char* pixel = this->get_pixel(y, x);

	for (int i = 0; i < this->filetype; i++) {
		*(pixel + i) = channels[i];
	}
}