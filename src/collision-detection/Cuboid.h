#pragma once
#include "Vector3.h"
#include <stdexcept>

class Box
{
public:
	// position of box center
	Vector3 pos;

	// box size
	Vector3 size;
public:
	Box(const Vector3& pos, const Vector3& size);

	// returns if it is colliding with given box
	bool isCollidingWith(const Box &b) const;

private:
	// helper function for interval intersection
	static bool hasIntervalIntersection(float posA, float sizeA, float posB, float sizeB);
};

class Cuboid : public Box
{
public:
	Cuboid(const Vector3& pos, Vector3&& size, float speed = 1.f);

	Cuboid(const Vector3& pos, float size, float speed = 1.f);

	Cuboid(Vector3&& pos, float size);

	~Cuboid();

	// collision response for collision with given cuboid
	void response(Cuboid &b);

	bool hasCollision() const;

	void setColliding(bool isColliding);

	// gets left, right, up, down, front, back coordinates (front > back)
	void getLRUDFB(float &l, float &r, float &u, float &d, float &f, float &b) const;

	// gets coordinate of left bound
	float left() const;

	// gets coordinate of right bound
	float right() const;

	// gets coordinate of upper bound
	float up() const;

	// gets coordinate of lower bound
	float down() const;

	// gets coordinate of front bound
	float front() const;

	// gets coordinate of back bound
	float back() const;

	// velocity vector
	Vector3 vel;

private:
	// calculates new position of a in collision with b
	static Vector3 calcResolution(const Cuboid &a, const Cuboid &b);

	bool m_hasCollision;
};

