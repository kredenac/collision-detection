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
	Mover(float left, float right, float up, float down, float front, float back, float speedScale = 1.f);
	Mover(Vector3 &min, Vector3 &max, float speedScale = 1.f);
	Mover();
	~Mover();

	void moveItems(std::vector<Cuboid> &items, float dt) const;

	// if cuboid c would exit Mover bounds in next step, with given speed,
	// then reflect its velocity to prevent it from going out
	void reflectVelocity(Cuboid &c, float withSpeed) const;
	
	void ensureWithinBounds(std::vector<Cuboid> &items, float delta) const;

	Box getBounds() const;

private:
	float left;
	float right;
	float up;
	float down;
	float front;
	float back;
	float speedScale;
};

