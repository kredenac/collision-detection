#pragma once
#include <vector>
#include <string>
#include "Cuboid.h"

using Collisions = std::vector<std::pair<Cuboid*, Cuboid*>>;

class BasicCollision
{
public:
	BasicCollision();
	virtual ~BasicCollision();
	virtual std::string getInfo() const;
	virtual void markCollisions(std::vector<Cuboid>& items, Collisions &pairs);
	virtual void drawSelf(void (*draw)(const Box &c, float r, float g, float b, float a)) const;
private:
	bool areColliding(Cuboid &a, Cuboid &b);
};

