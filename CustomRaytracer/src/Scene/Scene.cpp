#include "Scene.h"
#include "iostream"
#include "../Helper.h"

Scene::Scene(Color background_color_, double ambient_lighting_)
	: background_color(background_color_)
{
	ASSERT(0. <= ambient_lighting);
	ASSERT(ambient_lighting <= 1.);

	//std::cout << "SCENE: " << ~background_color_ << std::endl;
	this->background_color.a = 1.0;
	(this->ambient_lighting) = ambient_lighting_;

	this->background_color *= ambient_lighting;
}

Scene::~Scene()
{}

bool Scene::add_obj(Object3D* obj)
{
	if (obj == nullptr) return false;
	if (obj->id < 0) return false;

	for (auto& object : this->objects) {
		if (object == nullptr) continue;

		if (object->id == obj->id) {
			std::cout << "OBJECT ID '" << object->id << "' ALREADY EXISTS IN SCENE" << std::endl;
			return false;
		}
	}
	this->objects.push_back(obj);

	//std::cout << "SCENE2: " << ~this->background_color << std::endl;
	return true;
}
