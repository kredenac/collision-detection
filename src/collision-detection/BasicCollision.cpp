#include "BasicCollision.h"

BasicCollision::BasicCollision(std::vector<Cuboid>& items) : items(items)
{
	printf("original = %d, new = %d\n", &items[0], &this->items[0]);
}


BasicCollision::~BasicCollision()
{
}

void BasicCollision::markCollisions()
{
	unsigned len = items.size();
	for (unsigned i = 0; i < len - 1; i++) {

		for (unsigned j = i; j < len; j++) {
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
	bool interx = hasIntervalIntersection(a.pos.x, a.size.x, b.pos.x, b.size.x);
	bool intery = hasIntervalIntersection(a.pos.y, a.size.y, b.pos.y, b.size.y);
	bool interz = hasIntervalIntersection(a.pos.z, a.size.z, b.pos.z, b.size.z);
	return interx && intery && interz;
}

bool BasicCollision::hasIntervalIntersection(float posA, float sizeA, float posB, float sizeB)
{
	float mina, maxa, minb, maxb;
	mina = posA - sizeA / 2;
	maxa = posA + sizeA / 2;
	minb = posB - sizeB / 2;
	maxb = posB + sizeB / 2;
	return maxa > minb && mina < maxb;
}