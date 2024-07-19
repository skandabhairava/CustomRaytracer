#include <math.h>

#include "Shader.h" //includes Units.h
#include "../Helper.h"

DD get_angle_static_ref(const Vector& point)
{

	// static reference is taken as (0, 1, 0); math stuff

	return {
		atan2(point.z, point.x * point.x + point.y * point.y), // phi/v -> latitude
		atan2(point.y, point.x) // lambda/theta/u -> longitude
	};
}

Material::Material()
{
	//this->image = new Image(Color::empty());
	this->image = nullptr;
	this->roughness = nullptr;
	this->normal = nullptr;
	this->emission = nullptr;
	this->lock = false;
}

Material::Material(Image* image, Image* roughness, Image* normal, Image* emission)
	: image(image), roughness(roughness), normal(normal), emission(emission), lock(false)
{}

Material::~Material()
{}

Color Material::get_pixel_from_uv(const DD& uv, const Image& img)
{
	ASSERT(uv.a >= 0.0 && uv.b >= 0.0);

	//a = latitude
	//b = longitude

	int x = min((int)(uv.a * img.width), img.width);
	int y = min((int)(uv.b * img.height), img.height);

	unsigned char* color = img.get_pixel(y, x);
	switch (img.filetype)
	{
	case FileType::Grayscale:
		return Color(color[0], color[0], color[0]);
	case FileType::RGB:
		return Color(color[0], color[1], color[2]);
	case FileType::RGBA:
		return Color(color[0], color[1], color[2], (color[3]) / 255.0);
	default:
		break;
	}
}
