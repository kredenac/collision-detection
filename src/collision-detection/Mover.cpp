#include "Mover.h"

Mover::Mover(float left, float right, float up, float down, float front, float back)
	: left(left), right(right), up(up), down(down), front(front), back(back)
{
}


Mover::~Mover()
{
}

Cuboid Mover::getBounds() const
{
	float x = (left + right) / 2;
	float y = (up + down) / 2;
	float z = (back + front) / 2;

	float sizex = fabsf(left - right) *2;
	float sizey = fabsf(up - down) * 2;
	float sizez = fabsf(front - back) * 2;


	Vector3 pos = Vector3(x, y, z);
	return Cuboid(pos, Vector3(sizex, sizey, sizez));
}


void Mover::moveItems(std::vector<Cuboid> &items) const
{
	for (auto& x : items) {
		reflectVelocity(x);
		x.pos = x.pos + x.vel;
	}
}

bool Mover::willBeOutsideBounds(const Cuboid &c) const
{
	float l, r, u, d, f, b;
	c.getNextLRUDFB(l, r, u, d, f, b);

	bool xout = r < left || l > right;
	bool yout = u < down || d > up;
	bool zout = f < back || b > front;
	return xout || yout || zout;
}

void Mover::reflectVelocity(Cuboid &c) const
{
	float l, r, u, d, f, b;
	c.getNextLRUDFB(l, r, u, d, f, b);

	if (r < left || l > right) {
		c.vel.x *= -1;
	}
		
	if (u < down || d > up) {
		c.vel.y *= -1;
	}
		
	if (f < back || b > front) {
		c.vel.z *= -1;
	}
}