#pragma once
#include "BasicCollision.h"
#include <stdexcept>

class Octree : public BasicCollision, public Box
{
public:
	Octree(const Vector3 &pos, const Vector3 &size, int depth = 1);
	~Octree();

	virtual std::string getInfo() const;

	void markCollisions(std::vector<Cuboid>& items, std::vector<std::pair<Cuboid*, Cuboid*>> &pairs) override;

	// draws itself recursively using given draw function
	void drawSelf(void(*draw)(const Box &c, float r, float g, float b, float a)) const override;

	// counts number of all elements in the tree
	int countStoredElements() const;

	int countElementsInInnerNodes() const;
	static bool innerNodesHoldChildren;

	static unsigned maxElem;
	static unsigned maxDepth;

private:
	static std::vector<std::pair<Cuboid*, Cuboid*>> *pairs;
	// it's a member so it doesn't always get instantiated
	std::vector<bool> whichOctants;
	int depth;

	// helper function for insert
	void insertDownward(Cuboid *c);

	// inserts into node or lower
	void insert(Cuboid *c, bool markColl);

	// splits itself in octants[]
	void subdivide();
	bool isLeaf() const;

	// finds intersection downward with c
	void findIntersectionDownward(Cuboid *c);

	// returns true if there are collisions with multiple octants
	// puts true in whichOctants[i] when c intersects with i-th octant
	bool getIntersectingOctants(const Cuboid &c);
	const float c_extendFactor;
	static const unsigned c_octants = 8;


	std::vector<Cuboid*> elements;
	std::vector<Cuboid*> innerElements;
	// children are first split on x axis, then y, then z
	Octree *octants[8]; 
	// left x half: [0,1,2,3]
	// lower y half: [0,1,4,5]
	// lower z half: [0,2,4,6]
};

