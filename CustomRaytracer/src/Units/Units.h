#pragma once
#include <string>
#include <vector>

/*
class V3 {
public:
	double x, y, z;

public:
//	D operator+(const D& other);
//	D operator-(const D& other);
//	D operator*(double other);
//	D operator/(const D& other);

//	std::string operator~(); // __repr__
};
*/

struct DD {
	double a, b;

	DD operator-(DD& other) {
		return {
			this->a - other.a,
			this->b - other.b
		};
	}
};

class Color{
public:
	unsigned char r, g, b;
	float a;
public:
	Color operator+(const Color& other);
	Color operator-(const Color& other);
	Color operator*(double other);

	Color& operator*=(double other);
	Color& operator+=(const Color& other);

	std::string operator~(); // __repr__
public:
	Color();
	Color(unsigned int r_, unsigned int g_, unsigned int b_, float a_=1.0f);
	Color(unsigned int hex, float a=1.0f);
	~Color();

	inline static const Color empty() { return { 0, 0, 0, 0.0f }; };
};

class Vector{
public:
	double x, y, z;
public:
	Vector operator+(const Vector& other);
	Vector operator-(const Vector& other);
	Vector operator*(double other);
	Vector operator/(double other);

	std::string operator~(); // __repr__
public:
	Vector();
	Vector(double x_, double y_, double z_);
	~Vector();

	inline static const Vector empty() { return {0.0, 0.0, 0.0}; };
	double magnitude();
	Vector normalize(double magnitude = -1.);
	Vector operator+(double other);
	Vector operator-(double other);
	double dot(const Vector& other); // dot
	Vector cross(const Vector& other); // cross
};

class Line {
public:
	Vector origin;
	Vector dir;

	Line(Vector origin_, Vector dir_);
	~Line();
	std::string operator~(); // __repr__
};

enum ObjectType {
	Sphere_,
	PointLighting_,
	Camera_,
	NONE
};

struct ResultIntersection {
	ObjectType intersecting;
	Vector intersection_point, normal;
	Color color_of_intersection;
	double distance, brightness;
	DD UV;

	ResultIntersection(ObjectType intersecting, Vector intersection_point, Vector normal, Color color_of_intersection, double distance, double brightness, DD UV);
	~ResultIntersection();
};

class Object3D {
public:
	int id;
	Vector origin;
	Vector normal;
	ObjectType type;

	//Object3D(Vector origin_, Vector normal_, ObjectType type_);
	//virtual ~Object3D() {};
	virtual std::vector<ResultIntersection> intersect(const Line& line, const std::vector<Object3D*>& scene_objects, bool calc_brightness = true) = 0;
};