#pragma once
#include "Vector3.h"

class Box 
{
public:
	Vector3 pos;
	Vector3 size;
public:
	Box(const Vector3& pos, const Vector3& size) : pos(pos), size(size)
	{
	}

	bool isCollidingWith(const Box &b) const
	{
		bool interx = hasIntervalIntersection(pos.x, size.x, b.pos.x, b.size.x);
		bool intery = hasIntervalIntersection(pos.y, size.y, b.pos.y, b.size.y);
		bool interz = hasIntervalIntersection(pos.z, size.z, b.pos.z, b.size.z);
		return interx && intery && interz;
	}

private:
	bool hasIntervalIntersection(float posA, float sizeA, float posB, float sizeB) const
	{
		float mina, maxa, minb, maxb;
		mina = posA - sizeA / 2;
		maxa = posA + sizeA / 2;
		minb = posB - sizeB / 2;
		maxb = posB + sizeB / 2;
		return maxa >= minb && mina <= maxb;
	}
};

class Cuboid : public Box
{
public:
	Cuboid(const Vector3& pos, Vector3&& size)
		: Box(pos, size), vel(Vector3::randVec()), m_hasCollision(false)
	{
		vel.setLength(0.01f);
	}

	Cuboid(const Vector3& pos, float size) 
		: Cuboid(pos, Vector3(size, size, size))
	{
		
		vel.setLength(0.01f);
	}

	Cuboid(Vector3&& pos, float size)
		: Cuboid(pos, size)
	{
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

	Vector3 vel;

private:
	bool m_hasCollision;
};

