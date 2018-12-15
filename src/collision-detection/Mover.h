#pragma once
#include "Cuboid.h"
#include <vector>
#include <cmath>

// moves the given items
// holding them within given bounds

class Mover
{
public:
	Mover(float left, float right, float up, float down, float front, float back);
	~Mover();

	void moveItems(std::vector<Cuboid> &items) const;

	bool willBeOutsideBounds(const Cuboid &c) const;
	void reflectVelocity(Cuboid &c) const;
	
	Cuboid getBounds() const;
private:
	float left;
	float right;
	float up;
	float down;
	float front;
	float back;
};

