#include "BasicCollision.h"

BasicCollision::BasicCollision()
{
}


BasicCollision::~BasicCollision()
{
}

void BasicCollision::markCollisions(std::vector<Cuboid>& items, Collisions &pairs)
{
	unsigned len = items.size();
	for (unsigned i = 0; i < len - 1; i++) {

		for (unsigned j = i + 1; j < len; j++) {
			auto &a = items[i];
			auto &b = items[j];
			if (areColliding(a, b)) {
				items[i].setColliding(true);
				items[j].setColliding(true);
				pairs.push_back(std::make_pair(&items[i], &items[j]));
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

std::string BasicCollision::name() const
{
	return "BasicCollision";
}

std::string BasicCollision::additionalLogData() const
{
	return std::string();
}


void Idle::markCollisions(std::vector<Cuboid>& items, Collisions &pairs)
{
	return;
}

std::string Idle::getInfo() const
{
	return "Not handling collisions";
}

std::string Idle::name() const
{
	return "Idle";
}
