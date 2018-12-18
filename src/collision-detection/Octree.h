#pragma once
#include "BasicCollision.h"

class Octree : public BasicCollision, public Box
{
public:
	Octree(const Vector3 &pos, const Vector3 &size, int depth = 1);
	~Octree();

	void markCollisions(std::vector<Cuboid>& items) override;
	void drawSelf(void(*draw)(const Box &c, float r, float g, float b, float a)) const override;
	int countStoredElements() const;
private:
	// it's a member so it doesn't always get instantiated
	std::vector<bool> whichOctants;
	int depth;
	void insertDownward(Cuboid *c);
	void insert(Cuboid *c, int d, bool markColl);
	void subdivide();
	bool isLeaf() const;
	void getIntersectingOctants(const Cuboid &c);
	const unsigned c_maxElem = 4;
	const unsigned c_octants = 8;
	const int c_maxDepth = 10;

	std::vector<Cuboid*> elements;
	// children are first split on x axis, then y, then z
	Octree *octants[8]; 
	// left x half: [0,1,2,3]
	// lower y half: [0,1,4,5]
	// lower z half: [0,2,4,6]
};

