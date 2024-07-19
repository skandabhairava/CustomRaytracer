#pragma once
#include <vector>
#include "../Units/Units.h"

class Scene {
private:
	Color background_color;
public:
	std::vector<Object3D*> objects;
	double ambient_lighting;
public:
	Scene(Color background_color_, double ambient_lighting_ = 1.);
	~Scene();
	bool add_obj(Object3D* obj);
	inline Color get_background_color() const { return this->background_color; };
};