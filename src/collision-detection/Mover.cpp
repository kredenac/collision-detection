#include "Mover.h"

Mover::Mover(float left, float right, float up, float down, float front, float back, float speedScale)
	: left(left), right(right), up(up), down(down), front(front), back(back), speedScale(speedScale)
{
	if (front < back) {
		throw std::runtime_error("front < back");
	}
}

Mover::Mover(Vector3 &min, Vector3 &max, float speedScale)
	: Mover(min.x, max.x, max.y, min.y, max.z, min.z, speedScale)
{
}

Mover::Mover()
{

}

Mover::~Mover()
{
}

Box Mover::getBounds() const
{
	float x = (left + right) / 2;
	float y = (up + down) / 2;
	float z = (back + front) / 2;

	float sizex = fabsf(left - right);
	float sizey = fabsf(up - down);
	float sizez = fabsf(front - back);

	Vector3 pos = Vector3(x, y, z);
	return Box(pos, Vector3(sizex, sizey, sizez));
}


void Mover::moveItems(std::vector<Cuboid> &items, float delta) const
{
	for (auto& c : items) {
		c.pos = c.pos + c.vel * (delta * speedScale);
	}
	ensureWithinBounds(items, delta);
}

void Mover::ensureWithinBounds(std::vector<Cuboid> &items, float delta) const
{
	for (auto& c : items) {
		reflectVelocity(c, delta * speedScale);
	}
}

void Mover::reflectVelocity(Cuboid &c, float withSpeed) const
{
	float l, r, u, d, f, b;
	c.getLRUDFB(l, r, u, d, f, b);
	l += std::min(c.vel.x*withSpeed, 0.f);
	r += std::max(c.vel.x*withSpeed, 0.f);
	u += std::max(c.vel.y*withSpeed, 0.f);
	d += std::min(c.vel.y*withSpeed, 0.f);
	f += std::max(c.vel.z*withSpeed, 0.f);
	b += std::min(c.vel.z*withSpeed, 0.f);

	// if it is outisde bounds, then push it back in and change its velocity
	bool leftOut = l <= left;
	if (leftOut || r >= right) {
		// if x is too low, and is negative (would be even lower) then 
		// it's in a wrong direction
		bool isWrongDirection = leftOut ? c.vel.x < 0 : c.vel.x > 0;
		c.vel.x *= isWrongDirection ? -1 : 1;
		c.pos.x = leftOut ? left + c.size.x / 2 : right - c.size.x / 2;
	}
	
	bool downOut = d <= down;
	if (downOut || u >= up) {
		bool isWrongDirection = downOut ? c.vel.y < 0 : c.vel.y > 0;
		c.vel.y *= isWrongDirection ? -1 : 1;
		c.pos.y = downOut ? down + c.size.y / 2 : up - c.size.y / 2;
	}
	
	bool backOut = b <= back;
	if (backOut || f >= front) {
		bool isWrongDirection = backOut ? c.vel.z < 0 : c.vel.z > 0;
		c.vel.z *= isWrongDirection ? -1 : 1;
		c.pos.z = backOut ? back + c.size.z / 2 : front - c.size.z / 2;
	}
}