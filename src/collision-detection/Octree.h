#pragma once
#include "BasicCollision.h"

class Octree : public BasicCollision
{
public:
	Octree(const Vector3 &pos, const Vector3 &halfSize);
	~Octree();

	void markCollisions(std::vector<Cuboid>& items) override;

private:
	bool intersectsWith(const Cuboid &c) const;
	bool isLeaf() const;
	void getIntersectingOctants(const Cuboid &c, std::vector<bool> &whichOctants) const;
	const unsigned c_maxElem = 1;
	const unsigned c_octants = 8;
	const Vector3 pos;
	const Vector3 halfSize;

	Cuboid *elements;
	// children are first split on x axis, then y, then z
	Octree *children[8]; 
};

