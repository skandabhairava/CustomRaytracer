#pragma once
#include "../Units/Units.h"
#include "../Image/Image.h"
#include "../constants.h"

DD get_angle_static_ref(const Vector& point);

struct Material {
private:
	Image* image;

	Image* roughness;

	Image* normal;

	Image* emission;

	bool lock;

public:
	Material(); // never use this.
	Material(Image* image, Image* roughness, Image* normal, Image* emission);

	void render_lock() {
		if (this->lock) return;

		this->lock = true;

		if (this->image == nullptr) { 
			this->image = (Image*) &WHITE_BLANK_IMAGE;

		}

		if (this->roughness == nullptr) {
			this->roughness = (Image*) &BLACK_BLANK_IMAGE;
		}

		if (this->normal == nullptr) {
			this->normal = (Image*) &NORMAL_MAP_BLANK_IMAGE;
		}

		if (this->emission == nullptr) {
			this->emission = (Image*) &BLACK_BLANK_IMAGE;
		}
	}

	void render_unlock() {
		if (!this->lock) return;

		this->lock = false;
	}

	inline const Image* get_image() { return this->image; };
	inline const Image* get_roughness() { return this->roughness; };
	inline const Image* get_normal() { return this->normal; };
	inline const Image* get_emission() { return this->emission; };

	inline bool set_image(Image* image)
	{ 
		if (this->lock) return false;
		this->image = image;
		return true;
	};
	inline bool set_roughness(Image* roughness)
	{
		if (this->lock) return false;
		this->roughness = roughness;
		return true;
	};
	inline bool set_normal(Image* normal)
	{
		if (this->lock) return false;
		this->normal = normal;
		return true;
	};
	inline bool set_emission(Image* emission)
	{
		if (this->lock) return false;
		this->emission = emission;
		return true;
	};


	~Material();
	static Color get_pixel_from_uv(const DD& uv, const Image& img);
};