#pragma once
#include "Vector3.h"
#include <cstdlib>



class Cuboid
{
public:
	Cuboid(Vector3&& pos) : Cuboid(pos, Vector3(1, 1, 1))
	{
	}

	Cuboid(Vector3& pos, Vector3&& size) 
		: pos(pos), size(size), vel(randf(), randf(), randf()), m_hasCollision(false)
	{
		
		vel.setLength(0.01f);
	}

	~Cuboid();

	bool hasCollision() const 
	{
		return m_hasCollision;
	}

	void setColliding(bool isColliding) 
	{
		m_hasCollision = isColliding;
	}

	// gets left, right, up, down, front, back coordinates
	void getLRUDFB(float &l, float &r, float &u, float &d, float &f, float &b) const
	{
		l = pos.x - size.x / 2;
		r = pos.x + size.x / 2;
		u = pos.y + size.y / 2;
		d = pos.y - size.y / 2;
		f = pos.z + size.z / 2;
		b = pos.z - size.z / 2;
	}

	// gets bounds of object at position + velocity
	void getNextLRUDFB(float &l, float &r, float &u, float &d, float &f, float &b) const
	{
		getLRUDFB(l, r, u, d, f, b);
		l += vel.x;
		r += vel.x;
		u += vel.y;
		d += vel.y;
		f += vel.z;
		b += vel.z;
	}

	Vector3 pos;
	Vector3 size;
	Vector3 vel;

private:
	float randf()
	{
		return rand() / static_cast <float> (RAND_MAX);
	}

	bool m_hasCollision;
};

