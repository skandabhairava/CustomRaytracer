//#include <iostream>
#include <vector>
#include <math.h>
#include <algorithm>

#include "../ThreadPool/BS_thread_pool.hpp"
//#include "../ThreadPool/BS_thread_pool_utils.hpp"

#include "Objects.h"
#include "../Scene/Scene.h"
#include "../Helper.h"

PointLighting::PointLighting(int id, Vector origin_, Vector normal_, double strength)
{
	ASSERT(0. <= strength);
	ASSERT(strength <= 1.);

	this->id = id;
	this->origin = origin_;
	this->normal = normal_;
	this->strength = strength;
	this->type = ObjectType::PointLighting_;
}

PointLighting::~PointLighting()
{}

std::vector<ResultIntersection> PointLighting::intersect(const Line& line, const std::vector<Object3D*>& scene_objects, bool calc_brightness)
{
	std::vector<ResultIntersection> ret_list;
	if (calc_brightness) ret_list.push_back(ResultIntersection(ObjectType::PointLighting_, this->origin, Vector::empty(), Color::empty(), (this->origin - line.origin).magnitude(), this->strength, {0,0}));
	return ret_list;
}

Sphere::Sphere(int id, Vector origin_, Vector normal_, Material material, double radius)
{
	this->id = id;
	this->origin = origin_;
	this->normal = normal_;
	this->material = material;
	this->radius = radius;
	this->type = ObjectType::Sphere_;
}

Sphere::~Sphere()
{}

static double decimal_only(double x) {
	return x - (int)x;
}

DD Sphere::get_uv(const Vector& point) {
	// point should be normalised before passing to this function

	DD sphere_angle_relatively = get_angle_static_ref(this->normal); // relative to static ref
	DD point_angle_relatively = get_angle_static_ref(point);// relative to static ref

	DD lat_long = point_angle_relatively - sphere_angle_relatively;
	//return lat_long;

	// lat => [-pi/2, pi/2]
	// long => [-3pi/2, pi/2]

	lat_long.a = 1 - ((lat_long.a + PI/2.0) / (PI));

	lat_long.b = decimal_only(((lat_long.b  + 3.0*PI/2.0) / (2.0 * PI)) + 0.75); // [-3PI/2, PI/2] -> [0, 1] linear
	//  {0 > -pi/2 > -pi > -3pi/2 ^ pi/2 > 0} -> {0.5 > 0.25 > 0 ^ 1 > 0.75 > 0.5}

	return {
		lat_long.a, // phi / latitude 
		lat_long.b // lambda / longitude
	};
}

std::vector<ResultIntersection> Sphere::intersect(const Line& line, const std::vector<Object3D*>& scene_objects, bool calc_brightness) {
	double mu = pow(line.dir.x, 2) + pow(line.dir.y, 2) + pow(line.dir.z, 2);

	std::vector<ResultIntersection> ret_list;
	if (mu == 0) return ret_list;

	double lambda = -(line.dir.x * (line.origin.x - this->origin.x) + line.dir.y * (line.origin.y - this->origin.y) + line.dir.z * (line.origin.z - this->origin.z))/mu;

	// point on line where the ray is closest to the sphere (from world origin)
	Vector point = Vector(
		line.origin.x + line.dir.x*lambda,
		line.origin.y + line.dir.y*lambda,
		line.origin.z + line.dir.z*lambda
	);
	double dist = (point - this->origin).magnitude(); // dist b/w point on line and center of sphere

	// check if shortest dist is <= radius of sphere, if yes => it intersects

	if (dist > this->radius) return ret_list;

	// find intersection point vec
	Vector KP = (point - line.origin); // point on line where the ray is closest to the sphere (from line origin)
	if (KP.x == 0.0 && KP.y == 0.0 && KP.z == 0.0) KP.y = 0.001;
	Vector intersection_to_point = KP.normalize() * sqrt(pow(this->radius, 2) - pow(dist, 2)); // intersection to point on line

	Vector line_origin_to_intersection = KP - intersection_to_point;
	if (((Line)line).dir.dot(line_origin_to_intersection) > 0) { // doesnt intersect if intersection point is behind origin->dir
		// i.e if cam is turning away from the specified object (front of sphere)
		Vector intersection_point = line_origin_to_intersection + line.origin;
		Vector normal_intersection = (intersection_point - this->origin).normalize();

		DD UV = this->get_uv(normal_intersection);
		Color color = Material::get_pixel_from_uv(UV, *(this->material.get_image()));
		ResultIntersection res1 = ResultIntersection(ObjectType::Sphere_, std::move(intersection_point), std::move(normal_intersection), color, (intersection_point - line.origin).magnitude(), 0, std::move(UV));

		ret_list.push_back(std::move(res1));


		if (color.a == 1.0) return ret_list;
		// if the sphere is translucent, calculate intersection point of the back of the sphere
		if (dist == this->radius) return ret_list;
		// if the ray touches the edge, there will ony be 1 'interection point', hence return
	}

	Vector line_origin_to_intersection2 = KP + intersection_to_point;
	if (((Line)line).dir.dot(line_origin_to_intersection2) <= 0) return ret_list; // doesnt intersect if intersection point is behind origin->dir
	// i.e if cam is turning away from the specified object (in this case, camera is inside the sphere)

	Vector intersection_point2 = line_origin_to_intersection2 + line.origin;
	Vector normal_intersection = (intersection_point2 - this->origin).normalize();

	DD UV = this->get_uv(normal_intersection);
	Color color = Material::get_pixel_from_uv(UV, *(this->material.get_image()));
	ResultIntersection res2 = ResultIntersection(ObjectType::Sphere_, std::move(intersection_point2), std::move(normal_intersection), std::move(color), (intersection_point2 - line.origin).magnitude(), 0, std::move(UV));
	ret_list.push_back(std::move(res2));

	return ret_list;
}

Camera::Camera(int id, Vector origin_, Vector normal_, int height, int width, int focal_length, float max_clip, float min_clip)
{
	// Cam origin denotes the cam obj position, and NOT the pixel window
	// the pixel window is located[focal_length] distance in front of the camera object

	this->id = id;
	this->origin = origin_;
	this->normal = normal_;
	this->height = height;
	this->width = width;
	this->focal_length = focal_length;
	this->max_clip = max_clip;
	this->min_clip = min_clip + pow(10, 28)*FLT_MIN; // adding this reduced shadow-ray noise

	this->type = ObjectType::Camera_;

	this->orthogonals[0] = Vector(normal_.y, -normal_.x, 0).normalize();
	this->orthogonals[1] = normal_;
	this->orthogonals[2] = (this->orthogonals[0].cross(normal_)).normalize(); //cross
}

Camera::~Camera()
{}

ResultIntersection Camera::ray_intersect_nearest_obj(const Scene& scene, const Line& ray, int shadow_bounce, int calc_brightness_id, int origin_id)
{
	//std::cout << "CLOSEST: " << ~(Color)background_color << std::endl;
	std::vector<ResultIntersection> objects_intersecting;
	for (auto& object : scene.objects) {
		if (object == nullptr) continue;

		//if (object->id == origin_id) continue;

		if (object->type == ObjectType::PointLighting_ && object->id == calc_brightness_id) {
			PointLighting* object_p = (PointLighting*)object;
			std::vector<ResultIntersection> results = object_p->intersect(ray, scene.objects, true);

			for (auto& res : results) {
				objects_intersecting.push_back(std::move(res));
			}

			continue;
		}

		if (object->type != ObjectType::Sphere_) continue;
		Sphere* object_s = (Sphere*)object;
		
		std::vector<ResultIntersection> results = object_s->intersect(ray, scene.objects, (origin_id >= 0)? false : true);

		for (auto& res : results) {
			if (res.intersecting != ObjectType::NONE && res.distance < this->max_clip && res.distance > this->min_clip) {
				res.brightness = min(1.0, max(0.0, scene.ambient_lighting + Material::get_pixel_from_uv(res.UV, *(object_s->material.get_emission())).r)); // AMBIENT term

				//res.brightness = 1.0;
				if (shadow_bounce > 0 || res.brightness < 1.0) {
					// calc res's brightness towards PointLighting in scene.objects
					for (auto& object_2 : scene.objects) {
						if (res.brightness >= 1.0) break;
						if (object == nullptr) continue;

						if (object_2->type != ObjectType::PointLighting_) continue;
						PointLighting* object_p_2 = (PointLighting*)object_2;

						Vector light_direction = (object_p_2->origin - res.intersection_point);
						Line point_light_line = Line(res.intersection_point, light_direction);
						ResultIntersection point_light_ray = this->ray_intersect_nearest_obj(scene, point_light_line, shadow_bounce - 1, object_p_2->id, object_s->id);

						// inverse square law??
						//std::cout << point_light_ray.brightness << " :: ";
						//point_light_ray.brightness = point_light_ray.brightness * min(max(800.0/pow(light_direction.magnitude() + 100.0, 1.2), 0.0), 1.0);
						//std::cout << light_direction.magnitude() << " :: " << point_light_ray.brightness << std::endl;

						// brightness is calculated with 3 terms
						// ambient + diffuse + specular
						//  base   + normal. + view angle.

						// DIFFUSE 
						//Vector normal_intersection = (res.intersection_point - object_s->origin).normalize();
						Vector intersection_to_light = object_p_2->origin - res.intersection_point;


						Vector normal_orthogonal[3];
						normal_orthogonal[0] = Vector(res.normal.y, -res.normal.x, 0).normalize();
						normal_orthogonal[1] = res.normal;
						normal_orthogonal[2] = (normal_orthogonal[0].cross(res.normal)).normalize();

						// new normal = 
						Color d_normal = Material::get_pixel_from_uv(res.UV, *(object_s->material.get_normal()));
						Vector d_normal_v = Vector((double)d_normal.r, (double)d_normal.b, (double)d_normal.g);  // exchanging b and g due to industrial standards of 'Z'
						d_normal_v = ((d_normal_v / 255.0) * 2.0) - 1.0;

						//d_normal_v = Vector(0.0, 1.0, 0.0);

						res.normal = ((normal_orthogonal[0] * d_normal_v.x) + (normal_orthogonal[1] * d_normal_v.y) + (normal_orthogonal[2] * d_normal_v.z)).normalize();

						double diffuse = (max(0.0, res.normal.dot(intersection_to_light.normalize())) * point_light_ray.brightness);

						// SPECULAR
						//light_direction = light_direction.normalize();
						Vector reflected_ray = (res.normal * 2*res.normal.dot(light_direction)) - light_direction;
						Vector view_vec = (Vector)ray.origin - res.intersection_point;

						double dot = max(0., reflected_ray.normalize().dot(view_vec.normalize()));
						//Color color = Material::get_pixel_from_uv(UV, *(this->material.image));
						double roughness = (7.5 / 300.0) * Material::get_pixel_from_uv(res.UV, *(object_s->material.get_roughness())).r + 1;
						double specular = (pow(max(0.0, dot), pow(2, roughness)));
	
						//double specular_factor = \frac{ \left(.9 - .1\right) }{6}x + \frac{ \left(.9 - .1\right) }{6};
						double specular_factor = (0.55 / 10) * roughness + 0.33333;
						res.brightness = (res.brightness + diffuse * 0.8 + specular * specular_factor) *
							1;

						//__debugbreak();
					}
				}
				objects_intersecting.push_back(std::move(res));
			}
		}
	}

	ResultIntersection closest_point = ResultIntersection(ObjectType::NONE, Vector(ray.dir) * this->max_clip, Vector::empty(), scene.get_background_color(), this->max_clip, scene.ambient_lighting, {0, 0});
	if (objects_intersecting.empty()) return closest_point;

	std::sort(objects_intersecting.begin(), objects_intersecting.end(), [](const ResultIntersection& a, const ResultIntersection& b) {
		return a.distance < b.distance;
	});

	objects_intersecting.push_back(closest_point); // closest point is currently the "background"

	/*
	for (auto& i : objects_intersecting) {
		std::cout << ~i.color_of_intersection << std::endl;
	}
	*/

	closest_point.intersecting = objects_intersecting[0].intersecting;
	Color base = objects_intersecting[objects_intersecting.size() - 1].color_of_intersection * objects_intersecting[objects_intersecting.size() - 1].brightness;
	
	Color added = objects_intersecting[objects_intersecting.size() - 1 - 1].color_of_intersection * objects_intersecting[objects_intersecting.size() - 1 - 1].brightness;
	
	double base_brightness = objects_intersecting[objects_intersecting.size() - 1].brightness;

	for (int i = objects_intersecting.size() - 1 - 1; i >= 0; i--) {
		
		float mix_a = 1.0 - (1.0 - added.a) * (1.0 - base.a);

		double div_by_mix_a = 1.0 / (mix_a);
		base.r = (int)((added.r * added.a * div_by_mix_a) + (base.r * base.a * (1 - added.a) * div_by_mix_a));
		base.g = (int)((added.g * added.a * div_by_mix_a) + (base.g * base.a * (1 - added.a) * div_by_mix_a));
		base.b = (int)((added.b * added.a * div_by_mix_a) + (base.b * base.a * (1 - added.a) * div_by_mix_a));
		base.a = mix_a;

		base_brightness = min(1.0, max(0.0, (base_brightness * (double)(1 - objects_intersecting[i].color_of_intersection.a) + objects_intersecting[i].brightness)));
		
		if (i != 0) {
			added = objects_intersecting[i - 1].color_of_intersection * objects_intersecting[i - 1].brightness;
		}
	}

	closest_point.color_of_intersection = base;
	closest_point.distance = objects_intersecting[0].distance;
	closest_point.intersection_point = objects_intersecting[0].intersection_point;
	closest_point.brightness = base_brightness;
	closest_point.UV = objects_intersecting[0].UV;

	return closest_point;
}

Image* Camera::render(Scene& scene)
{
	// multiply all height and width with 2
	Image* render = new Image(this->width, this->height, FileType::RGB);

	for (Object3D* obj : scene.objects) {
		if (obj->type != ObjectType::Sphere_) continue;
		Sphere* objs = (Sphere*)obj;
		objs->material.render_lock();
	}
	
	BS::thread_pool pool;

	//BS::timer tmr;
	//tmr.start();

	
	pool.detach_loop<unsigned int>(0, this->width * this->height,
		[&render, this, scene](unsigned int i)
		{
			// [(i//h, i%h) for i in l] where l is width * heights
			int x = 2 * (i / this->height);
			int z = 2 * (i % this->height);
			
			int dx = x - this->width;
			int dz = z - this->height;

			//if (x == this->width && z == this->height) __debugbreak();

			Line ray1 = Line(this->origin, (this->orthogonals[1] * this->focal_length * 2) + (this->orthogonals[0] * dx) + (this->orthogonals[2] * dz));
			Line ray2 = Line(this->origin, (this->orthogonals[1] * this->focal_length * 2) + (this->orthogonals[0] * dx + 1) + (this->orthogonals[2] * dz));
			Line ray3 = Line(this->origin, (this->orthogonals[1] * this->focal_length * 2) + (this->orthogonals[0] * dx) + (this->orthogonals[2] * dz + 1));
			Line ray4 = Line(this->origin, (this->orthogonals[1] * this->focal_length * 2) + (this->orthogonals[0] * dx + 1) + (this->orthogonals[2] * dz + 1));

			ResultIntersection intersection1 = this->ray_intersect_nearest_obj(scene, ray1, 1, -1, -1);
			ResultIntersection intersection2 = this->ray_intersect_nearest_obj(scene, ray2, 1, -1, -1);
			ResultIntersection intersection3 = this->ray_intersect_nearest_obj(scene, ray3, 1, -1, -1);
			ResultIntersection intersection4 = this->ray_intersect_nearest_obj(scene, ray4, 1, -1, -1);

			// using 3 as i dont want heap allocated, and renderer only works on rgb, so 3 is static
			unsigned char channels[3] = {
				(intersection1.color_of_intersection.r + intersection2.color_of_intersection.r + intersection3.color_of_intersection.r + intersection4.color_of_intersection.r) / 4,
				(intersection1.color_of_intersection.g + intersection2.color_of_intersection.g + intersection3.color_of_intersection.g + intersection4.color_of_intersection.g) / 4,
				(intersection1.color_of_intersection.b + intersection2.color_of_intersection.b + intersection3.color_of_intersection.b + intersection4.color_of_intersection.b) / 4
			};

			render->set_pixel((this->height * 2 - z - 1) / 2, x / 2, channels);
		}, 4);
	pool.wait();
	

	//tmr.stop();
	//std::cout << "The elapsed time was " << tmr.ms() << " ms.\n";

	/*
	for (unsigned int x = 0; x < this->width*2; x+=2)
		for (unsigned int z = 0; z < this->height*2; z+=2) {
			int dx = x - this->width;
			int dz = z - this->height;

			//if (x == this->width && z == this->height) __debugbreak();

			Line ray1 = Line(this->origin, (this->orthogonals[1] * this->focal_length*2) + (this->orthogonals[0] * dx) + (this->orthogonals[2] * dz));
			Line ray2 = Line(this->origin, (this->orthogonals[1] * this->focal_length*2) + (this->orthogonals[0] * dx + 1) + (this->orthogonals[2] * dz));
			Line ray3 = Line(this->origin, (this->orthogonals[1] * this->focal_length*2) + (this->orthogonals[0] * dx) + (this->orthogonals[2] * dz + 1));
			Line ray4 = Line(this->origin, (this->orthogonals[1] * this->focal_length*2) + (this->orthogonals[0] * dx + 1) + (this->orthogonals[2] * dz + 1));

			ResultIntersection intersection1 = this->ray_intersect_nearest_obj(scene, ray1, 1, -1, -1);
			ResultIntersection intersection2 = this->ray_intersect_nearest_obj(scene, ray2, 1, -1, -1);
			ResultIntersection intersection3 = this->ray_intersect_nearest_obj(scene, ray3, 1, -1, -1);
			ResultIntersection intersection4 = this->ray_intersect_nearest_obj(scene, ray4, 1, -1, -1);

			// using 3 as i dont want heap allocated, and renderer only works on rgb, so 3 is static
			unsigned char channels[3] = {
				(intersection1.color_of_intersection.r + intersection2.color_of_intersection.r + intersection3.color_of_intersection.r + intersection4.color_of_intersection.r) / 4,
				(intersection1.color_of_intersection.g + intersection2.color_of_intersection.g + intersection3.color_of_intersection.g + intersection4.color_of_intersection.g) / 4,
				(intersection1.color_of_intersection.b + intersection2.color_of_intersection.b + intersection3.color_of_intersection.b + intersection4.color_of_intersection.b) / 4
			};

			render->set_pixel((this->height * 2 - z - 1) / 2, x / 2, channels);
			
		}
	*/

	//this->width *= 2;
	//this->height *= 2;
	for (Object3D* obj : scene.objects) {
		if (obj->type != ObjectType::Sphere_) continue;
		Sphere* objs = (Sphere*)obj;
		objs->material.render_unlock();
	}
	return render;
}

std::vector<ResultIntersection> Camera::intersect(const Line& line, const std::vector<Object3D*>& scene_objects, bool calc_brightness)
{
	return std::vector<ResultIntersection>();
}
