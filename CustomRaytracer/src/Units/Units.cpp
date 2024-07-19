#include <sstream>
#include <math.h>
#include <iostream>

#include "Units.h"
#include "../Helper.h"


//////////////////////////////////////////////////////
///            COLOR

Color Color::operator+(const Color& other)
{
	//return Color(this->x + other.x, this->y + other.y, this->z + other.z);
	return Color(min(max(this->r + other.r, 0), 255), min(max(this->g + other.g, 0), 255), min(max(this->b + other.b, 0), 255));
}

Color Color::operator-(const Color& other)
{
	return Color(min(max(this->r - other.r, 0), 255), min(max(this->g - other.g, 0), 255), min(max(this->b - other.b, 0), 255));
}

Color Color::operator*(double other)
{
	ASSERT(0. <= other);
	//ASSERT(other <= 1.);
	//if (other > 1.0) {
	//	other = 1.0;
	//}
	return Color(min(max((int)(this->r * other), 0), 255), min(max((int)(this->g * other), 0), 255), min(max((int)(this->b * other), 0), 255), this->a);
}

Color& Color::operator*=(double other)
{
	ASSERT(0. <= other);
	ASSERT(other <= 1.);

	this->r *= other;
	this->g *= other;
	this->b *= other;

	this->r = min(max(this->r, 0), 255);
	this->g = min(max(this->g, 0), 255);
	this->b = min(max(this->b, 0), 255);

	return *this;
}

Color& Color::operator+=(const Color& other)
{
	this->r += other.r;
	this->g += other.g;
	this->b += other.b;

	this->r = min(max(this->r, 0), 255);
	this->g = min(max(this->g, 0), 255);
	this->b = min(max(this->b, 0), 255);

	return *this;
}

std::string Color::operator~()
{
	std::stringstream str;
	str << "R" << (int)this->r << " - G" << (int)this->g << " - B" << (int)this->b << " - A" << this->a;

	return str.str();
}

Color::Color()
{
	this->r = 0;
	this->b = 0;
	this->g = 0;
	this->a = 1;
}

Color::Color(unsigned int r_, unsigned int g_, unsigned int b_, float a_)
	: r(r_), g(g_), b(b_), a(a_)
{}

Color::Color(unsigned int hex, float a)
{
	(this->r) = ((hex >> 16) & 0xFF);
	(this->g) = ((hex >> 8) & 0xFF);
	(this->b) = ((hex) & 0xFF);
	(this->a) = a;
}

Color::~Color()
{}

////////////////////////////////////////////////////////
////            VECTOR

Vector::Vector()
	: x(0), y(0), z(0)
{}

Vector::Vector(double x_, double y_, double z_)
	: x(x_), y(y_), z(z_)
{}

Vector::~Vector()
{}

Vector Vector::operator+(const Vector& other) // VECTOR + VECTOR
{
	return Vector(this->x + other.x, this->y + other.y, this->z + other.z);
}

Vector Vector::operator-(const Vector& other) // VECTOR - VECTOR
{
	return Vector(this->x - other.x, this->y - other.y, this->z - other.z);
}

Vector Vector::operator*(double other) // VECTOR * DOUBLE
{
	return Vector(this->x * other, this->y * other, this->z * other);
}

Vector Vector::operator/(double other) // VECTOR / DOUBLE
{
	return Vector(this->x / other, this->y / other, this->z / other);
}

std::string Vector::operator~() // __repr__
{
	std::stringstream str;
	str << "x: " << this->x << " | y: " << this->y << " | z: " << this->z;

	return str.str();
}

double Vector::magnitude()
{
	return sqrt(pow(this->x, 2) + pow(this->y, 2) + pow(this->z, 2));
}

Vector Vector::normalize(double magnitude)
{
	if (magnitude == -1.) magnitude = this->magnitude();
	return Vector(this->x/magnitude, this->y/magnitude, this->z/magnitude);
}

Vector Vector::operator+(double other) // VECTOR + DOUBLE
{
	return Vector(this->x + other, this->y + other, this->z + other);
}

Vector Vector::operator-(double other) // VECTOR - DOUBLE
{
	return Vector(this->x - other, this->y - other, this->z - other);
}

double Vector::dot(const Vector& other) // dot
{
	return (this->x * other.x) + (this->y * other.y) + (this->z * other.z);
}

Vector Vector::cross(const Vector& other) // cross 
{
	return Vector{
		this->y * other.z - this->z * other.y,
		this->z * other.x - this->x * other.z,
		this->x * other.y - this->y * other.x,
	};
}

/////////////////////////////////////////////////////////////////
///                 LINE

Line::Line(Vector origin_, Vector dir_)
	: origin(origin_)
{
	(this->dir) = dir_.normalize();
}

Line::~Line()
{
}

std::string Line::operator~() // __repr__ 
{
	std::stringstream str;
	str << "Origin: (" << ~(this->origin) << "), Dir(" << ~(this->dir) << ")";

	return str.str();
}

/////////////////////////////////////////////////////////////////
///                OBJECT3D
/*
Object3D::Object3D(Vector origin_, Vector normal_, ObjectType type_)
	: origin(origin_), type(type_)
{
	(this->normal) = normal_.normalize();
}


Object3D::~Object3D()
{
}
*/

ResultIntersection::ResultIntersection(ObjectType intersecting, Vector intersection_point, Vector normal, Color color_of_intersection, double distance, double brightness, DD UV)
	: intersecting(intersecting), intersection_point(intersection_point), normal(normal), color_of_intersection(color_of_intersection), distance(distance), brightness(brightness), UV(UV)
{}

ResultIntersection::~ResultIntersection()
{}
