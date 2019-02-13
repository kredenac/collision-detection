#include "Cuboid.h"
#include "Vector3.h"

Cuboid::~Cuboid()
{
}

Box::Box(const Vector3& pos, const Vector3& size) : pos(pos), size(size)
{
}

bool Box::isCollidingWith(const Box &b) const
{
	bool interx = hasIntervalIntersection(pos.x, size.x, b.pos.x, b.size.x);
	bool intery = hasIntervalIntersection(pos.y, size.y, b.pos.y, b.size.y);
	bool interz = hasIntervalIntersection(pos.z, size.z, b.pos.z, b.size.z);
	return interx && intery && interz;
}

bool Box::hasIntervalIntersection(float posA, float sizeA, float posB, float sizeB)
{
	float mina, maxa, minb, maxb;
	mina = posA - sizeA / 2;
	maxa = posA + sizeA / 2;
	minb = posB - sizeB / 2;
	maxb = posB + sizeB / 2;
	return maxa >= minb && mina <= maxb;
}

Cuboid::Cuboid(const Vector3& pos, Vector3&& size, float speed)
	: Box(pos, size), vel(Vector3::randVec()), m_hasCollision(false)
{
	vel.setLength(speed);
}

Cuboid::Cuboid(const Vector3& pos, float size, float speed)
	: Cuboid(pos, Vector3(size, size, size), speed)
{

}

Cuboid::Cuboid(Vector3&& pos, float size)
	: Cuboid(pos, size)
{
}

void Cuboid::response(Cuboid &b)
{
	Cuboid &a = *this;
	auto aVel = calcResolution(a, b);
	auto bVel = calcResolution(b, a);
	const float eps = 1.001f;
	auto centerDist = a.pos - b.pos;
	auto addSizes = a.size * 0.5f + b.size * 0.5F;

	centerDist = centerDist * eps;
	if (centerDist.x > 0) { // a is to the left of b
		addSizes.x *= -1.f;
	}
	if (centerDist.y > 0) {
		addSizes.y *= -1.f;
	}
	if (centerDist.z > 0) {
		addSizes.z *= -1.f;
	}

	//	|-----|
	//	|	|-|-----|
	//	|	|_|_____|
	//	|_____|
	//
	//	currently mtv is distance between centers
	//	but sizes of two squares - distance
	//	is equal to minimal translation vector
	
	// minimum translation vector

	// this is how it should be: auto mtv = centerDist * (-1.f) + addSizes;

	// this is circle-like resolution
	auto mtv = centerDist;
	if (mtv.isZero()) {
		mtv = addSizes;
	}
	mtv.setLength(addSizes.length() - centerDist.length());

	a.pos.x += mtv.x / 2;
	b.pos.x -= mtv.x / 2;

	a.pos.y += mtv.y / 2;
	b.pos.y -= mtv.y / 2;

	a.pos.z += mtv.z / 2;
	b.pos.z -= mtv.z / 2;

	if (a.pos.x != a.pos.x) {
		//throw std::runtime_error("nan");
	}

	a.vel = aVel;
	b.vel = bVel;
}

bool Cuboid::hasCollision() const
{
	return m_hasCollision;
}

void Cuboid::setColliding(bool isColliding)
{
	m_hasCollision = isColliding;
}

// gets left, right, up, down, front, back coordinates (front > back)
void Cuboid::getLRUDFB(float &l, float &r, float &u, float &d, float &f, float &b) const
{
	l = left();
	r = right();
	u = up();
	d = down();
	f = front();
	b = back();
}

float Cuboid::left() const
{
	return  pos.x - size.x / 2;
}

float Cuboid::right() const
{
	return pos.x + size.x / 2;
}

float Cuboid::up() const
{
	return pos.y + size.y / 2;
}

float Cuboid::down() const
{
	return pos.y - size.y / 2;
}

float Cuboid::front() const
{
	return  pos.z + size.z / 2;
}

float Cuboid::back() const
{
	return  pos.z - size.z / 2;
}

Vector3 Cuboid::calcResolution(const Cuboid &a, const Cuboid &b)
{
	// m1, m2 can be masses
	float m1 = 1.f;
	float m2 = 1.f;
	float scalar = 2 * m2 / (m1 + m2);
	auto posDiff = a.pos - b.pos;
	float posDiffLen = posDiff.length();
	if (posDiffLen == 0) {
		posDiffLen = 0.0001f;
	}
	scalar *= (a.vel - b.vel).dot(posDiff) / (posDiffLen * posDiffLen);
	if (a.vel.x != a.vel.x) {
		//throw std::runtime_error("nan");
	}
	return a.vel - (posDiff * scalar);
}
