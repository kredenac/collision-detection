#pragma once
#include <vector>
#include "Cuboid.h"

class BasicCollision
{
public:
	BasicCollision();
	virtual ~BasicCollision();

	virtual void markCollisions(std::vector<Cuboid>& items);

private:
	bool areColliding(Cuboid &a, Cuboid &b);
	bool hasIntervalIntersection(float posA, float sizeA, float posB, float sizeB);
};

