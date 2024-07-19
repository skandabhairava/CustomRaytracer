#pragma once
#include <vector>

#include "../Units/Units.h"
#include "../Image/Image.h"
#include "../Scene/Scene.h"
#include "../Shader/Shader.h"

class PointLighting : public Object3D {
public:
	double strength;

	PointLighting(int id, Vector origin_, Vector normal_, double strength);
	~PointLighting();
	virtual std::vector<ResultIntersection> intersect(const Line& line, const std::vector<Object3D*>& scene_objects, bool calc_brightness = true) override;
};

class Camera : public Object3D {
public:
	unsigned int height, width;
	int focal_length;
	float max_clip, min_clip;
	Vector orthogonals[3];

	Camera(int id, Vector origin_, Vector normal_, int height, int width, int focal_length, float max_clip, float min_clip);
	~Camera();

	inline Vector get_origin() const { return this->origin; };

	ResultIntersection ray_intersect_nearest_obj(const Scene& scene, const Line& ray, int shadow_bounce, int calc_brightness_id, int origin_id = -1);
	Image* render(Scene& scene_objects);
	virtual std::vector<ResultIntersection> intersect(const Line& line, const std::vector<Object3D*>& scene_objects, bool calc_brightness = true) override;
};

class Sphere : public Object3D {
public:
	Material material;
	double radius;

	Sphere(int id, Vector origin_, Vector normal_, Material material, double radius);
	~Sphere();
	virtual std::vector<ResultIntersection> intersect(const Line& line, const std::vector<Object3D*>& scene_objects, bool calc_brightness = true) override;
	DD get_uv(const Vector& point);
};