#pragma once
#include "Cuboid.h"
#include <vector>
#include <cmath>
#include <exception>
#include <stdexcept>

// moves the given items
// holding them within given bounds

class Mover
{
public:
	Mover(float left, float right, float up, float down, float front, float back);
	Mover(Vector3 &min, Vector3 &max);
	~Mover();

	void moveItems(std::vector<Cuboid> &items, float dt) const;

	//bool willBeOutsideBounds(const Cuboid &c) const;
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

