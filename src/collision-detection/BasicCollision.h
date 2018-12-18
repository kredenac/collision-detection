#pragma once
#include <vector>
#include "Cuboid.h"

class BasicCollision
{
public:
	BasicCollision();
	virtual ~BasicCollision();

	virtual void markCollisions(std::vector<Cuboid>& items);
	virtual void drawSelf(void (*draw)(const Box &c, float r, float g, float b, float a)) const;
private:
	bool areColliding(Cuboid &a, Cuboid &b);
};

