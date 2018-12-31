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

	void response(Cuboid &b)
	{
		Cuboid &a = *this;

		const float eps = 1.001;
		// minimum translation vector
		auto mtv = a.pos - b.pos; // TODO take into account sizes 
		auto addSizes = a.size * 0.5f + b.size * 0.5F;

		mtv = mtv * eps;
		if (mtv.x > 0) { // a is to the left of b
			addSizes.x *= -1.f;
		}
		if (mtv.y > 0) {
			addSizes.y *= -1.f;
		}
		if (mtv.z > 0) {
			addSizes.z *= -1.f;
		}

		/*	|-----|
			|	|-|-----|
			|	|_|_____|
			|_____|

			currently mtv is distance between centers
			but sizes of two squares - distance
			is equal to minimal translation vector
		*/
		mtv = mtv * (-1.f) + addSizes;
		a.pos.x += mtv.x / 2;
		b.pos.x -= mtv.x / 2;
		
		a.pos.y += mtv.y / 2;
		b.pos.y -= mtv.y / 2;
		
		a.pos.z += mtv.z / 2;
		b.pos.z -= mtv.z / 2;
		auto aVel = calcResolution(a, b);
		auto bVel = calcResolution(b, a);

		a.vel = aVel;
		b.vel = bVel;
	}

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
	static Vector3 calcResolution(const Cuboid &a, const Cuboid &b)
	{
		// m1, m2 can be masses
		float m1 = 1.f;
		float m2 = 1.f;
		float scalar = 2 * m2 / (m1 + m2);
		auto posDiff = a.pos - b.pos;
		float posDiffLen = posDiff.length();
		scalar *= (a.vel - b.vel).dot(posDiff) / (posDiffLen * posDiffLen);
		return a.vel - (posDiff * scalar);
	}
	bool m_hasCollision;
};

