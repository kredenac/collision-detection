#pragma once
#include <cmath>
#include <cstdlib>

class Vector3
{
public:
	Vector3();
	~Vector3();
	float x;
	float y;
	float z;

	Vector3(float x, float y, float z) 
		: x(x), y(y), z(z)
	{
	}

	Vector3(float size)
		: x(1.f), y(1.f), z(1.f)
	{
		setLength(size);
	}

	void add(float x, float y, float z) 
	{
		this->x += x;
		this->y += y;
		this->z += z;
	}

	bool isZero() const
	{
		return !x && !y && !z;
	}


	static Vector3 randVec(bool normalize = true);
	static Vector3 randVec(Vector3 &min, Vector3 &max);
	
	static float randf();

	void normalize();

	void setLength(float length);

	float length() const;

	Vector3 operator + (const Vector3& v) const
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 operator - (const Vector3& v) const
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	Vector3 operator + (const float c) const
	{
		return Vector3(x + c, y + c, z + c);
	}

	Vector3 operator * (const float c) const
	{
		return Vector3(x * c, y * c, z * c);
	}

	float dot(const Vector3& A) const
	{
		return A.x*x + A.y*y + A.z*z;
	}

	// direction in radians
	float direction() const
	{
		return atan2f(y, x);
	}
};
