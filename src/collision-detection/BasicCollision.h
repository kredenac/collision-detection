#pragma once
#include <vector>
#include "Cuboid.h"

class BasicCollision
{
public:
	BasicCollision(std::vector<Cuboid> &items);
	~BasicCollision();

	void markCollisions();

private:
	std::vector<Cuboid> &items;
	bool areColliding(Cuboid &a, Cuboid &b);
	bool hasIntervalIntersection(float posA, float sizeA, float posB, float sizeB);
};

