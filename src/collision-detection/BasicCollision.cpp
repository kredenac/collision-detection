#include "BasicCollision.h"

BasicCollision::BasicCollision()
{
}


BasicCollision::~BasicCollision()
{
}

void BasicCollision::markCollisions(std::vector<Cuboid>& items, std::vector<std::pair<Cuboid*, Cuboid*>> &pairs)
{
	unsigned len = items.size();
	for (unsigned i = 0; i < len - 1; i++) {

		for (unsigned j = i + 1; j < len; j++) {
			auto &a = items[i];
			auto &b = items[j];
			if (areColliding(a, b)) {
				items[i].setColliding(true);
				items[j].setColliding(true);
			}
		}
	}
}

bool BasicCollision::areColliding(Cuboid &a, Cuboid &b)
{
	return a.isCollidingWith(b);
}

void BasicCollision::drawSelf(void(*draw)(const Box &c, float r, float g, float b, float a)) const
{
	(void)draw;
}

std::string BasicCollision::getInfo() const
{
	return "Basic Collision";
}