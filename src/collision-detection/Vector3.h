#pragma once
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

	void add(float x, float y, float z) 
	{
		this->x += x;
		this->y += y;
		this->z += z;
	}

	Vector3 operator + (const Vector3& v) const
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 operator + (const float c) const
	{
		return Vector3(x + c, y + c, z + c);
	}

	float dot(const Vector3& A) const
	{
		return A.x*x + A.y*y + A.z*z;
	}
};
