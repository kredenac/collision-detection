#include "Vector3.h"
#include <cstdlib>



Vector3::Vector3()
{
}


Vector3::~Vector3()
{
}

void Vector3::normalize()
{
	float len =  x * x + y * y + z * z;
	len = sqrtf(len);
	x /= len;
	y /= len;
	z /= len;
}

void Vector3::setLength(float length)
{
	normalize();
	x *= length;
	y *= length;
	z *= length;
}

Vector3 Vector3::randVec(bool normalize)
{
	auto v = Vector3(randf(), randf(), randf());
	if (normalize) {
		v.normalize();
	}
	return v;
}

Vector3 Vector3::randVec(Vector3 &min, Vector3 &max)
{
	auto v = randVec(false);
	v.x = v.x * (max.x - min.x) + min.x;
	v.y = v.y * (max.y - min.y) + min.y;
	v.z = v.z * (max.z - min.z) + min.z;
	return v;
}

float Vector3::randf()
{
	return rand() / static_cast <float> (RAND_MAX);
}