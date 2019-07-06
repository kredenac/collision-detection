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

	// get stats about algorithm to be displayed
	virtual std::string getInfo() const;

	// check all items for collisions and construct their pairs
	virtual void markCollisions(std::vector<Cuboid>& items, Collisions &pairs);

	// get algorithm name
	virtual std::string name() const;

	// get data specific to this algorithm - return empty if there is none
	virtual std::string additionalLogData() const;

	// currently not in use
	virtual void drawSelf(void (*draw)(const Box &c, float r, float g, float b, float a)) const;

private:
	bool areColliding(Cuboid &a, Cuboid &b);
};

class Idle : public BasicCollision
{
	void markCollisions(std::vector<Cuboid>& items, Collisions & pairs);
	virtual std::string getInfo() const;
	virtual std::string name() const;
};

