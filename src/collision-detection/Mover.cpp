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

Mover::~Mover()
{
}

Cuboid Mover::getBounds() const
{
	float x = (left + right) / 2;
	float y = (up + down) / 2;
	float z = (back + front) / 2;

	float sizex = fabsf(left - right);
	float sizey = fabsf(up - down);
	float sizez = fabsf(front - back);

	Vector3 pos = Vector3(x, y, z);
	return Cuboid(pos, Vector3(sizex, sizey, sizez));
}


void Mover::moveItems(std::vector<Cuboid> &items, float delta) const
{
	for (auto& c : items) {
		c.pos = c.pos + c.vel * delta * speedScale;
		reflectVelocity(c);
	}
}

void Mover::reflectVelocity(Cuboid &c) const
{
	float l, r, u, d, f, b;
	c.getNextLRUDFB(l, r, u, d, f, b);

	if (l <= left || r >= right) {
		c.vel.x *= -1;
		c.pos.x = l <= left ? left + c.size.x / 2 : right - c.size.x / 2;
	}
		
	if (d <= down || u >= up) {
		c.vel.y *= -1;
		c.pos.y = d <= down ? down + c.size.y / 2 : up - c.size.y / 2;
	}
		
	if (b <= back || f >= front) {
		c.vel.z *= -1;
		c.pos.z = b <= back ? back + c.size.z / 2 : front - c.size.z / 2;
	}
}