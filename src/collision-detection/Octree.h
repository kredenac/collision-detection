#pragma once
#include "BasicCollision.h"

class Octree : public BasicCollision, public Box
{
public:
	Octree(const Vector3 &pos, const Vector3 &size);
	~Octree();

	void markCollisions(std::vector<Cuboid>& items) override;

private:
	void insert(Cuboid *c, int d, bool markColl = false);
	void subdivide();
	bool isLeaf() const;
	void getIntersectingOctants(const Cuboid &c, std::vector<bool> &whichOctants) const;
	const unsigned c_maxElem = 4;
	const unsigned c_octants = 8;

	std::vector<Cuboid*> elements;
	// children are first split on x axis, then y, then z
	Octree *octants[8]; 
	// left x half: [0,1,2,3]
	// lower y half: [0,1,4,5]
	// lower z half: [0,2,4,6]
};

