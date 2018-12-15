#include "Vector3.h"



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
