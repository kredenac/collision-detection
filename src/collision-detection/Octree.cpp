#include "Octree.h"

bool Octree::innerNodesHoldChildren;

Octree::Octree(const Vector3 &pos, const Vector3 &size, int depth) :
	Box(pos, size), depth(depth), whichOctants(c_octants)
{
	whichOctants.resize(c_octants);
	for (int i = 0; i < c_octants; i++) {
		octants[i] = nullptr;
	}
}

Octree::~Octree()
{
	if (isLeaf()) {
		return;
	}
	for (int i = 0; i < c_octants; i++) {
		delete octants[i];
	}
}

bool Octree::getIntersectingOctants(const Cuboid &c)
{
	bool moreThanOneOctant = false;
	bool foundOctant = false;
	for (int i = 0; i < c_octants; i++) {
		whichOctants[i] = octants[i]->isCollidingWith(c);
		if (whichOctants[i]) {
			moreThanOneOctant = foundOctant;
			foundOctant = true;
		}
	}
	return moreThanOneOctant;
}

void Octree::markCollisions(std::vector<Cuboid>& items) 
{
	for (Cuboid &c : items) {
		insert(&c, 0, true);
	}
}

void Octree::subdivide()
{
	Vector3 newSize = size * 0.5f;
	Vector3 lowPos(pos.x - newSize.x/2, pos.y - newSize.y/2, pos.z - newSize.z/2);
	int newDepth = depth + 1;
	octants[0] = new Octree(lowPos, newSize, newDepth); //smallest x,y,z
	Vector3 newPos = lowPos; newPos.z += newSize.z;
	octants[1] = new Octree(newPos, newSize, newDepth); // bigger z
	newPos = lowPos; newPos.z += newSize.z; newPos.y += newSize.y;
	octants[2] = new Octree(newPos, newSize, newDepth); // bigger z y
	newPos = lowPos; newPos.y += newSize.y;
	octants[3] = new Octree(newPos, newSize, newDepth); // bigger y
	lowPos.x += newSize.x;
	newPos = lowPos;
	octants[4] = new Octree(newPos, newSize, newDepth); // bigger x
	newPos.z += newSize.z;
	octants[5] = new Octree(newPos, newSize, newDepth); // bigger x, z
	newPos = lowPos; newPos.y += newSize.y;
	octants[6] = new Octree(newPos, newSize, newDepth); // bigger x, y
	newPos.z += newSize.z;
	octants[7] = new Octree(newPos, newSize, newDepth); // bigger x, y,z
}

void Octree::insert(Cuboid *c, int testing_depth, bool markColl)
{
	if (!isLeaf()) {
		insertDownward(c);
		return;
	}

	if (markColl) {
		for (auto e : elements) {
			if (e->isCollidingWith(*c)) {
				e->setColliding(true);
				c->setColliding(true);
			}
		}
	}
	elements.push_back(c);

	// if there's space, or max depth reached, just put it and return 
	if (elements.size() <= c_maxElem || depth == c_maxDepth) {
		return;
	}
	// if max elem reached, subdivide
	subdivide();

	// then insert elements into octants
	// iterating over index, vector may get realloced
	for (int i = 0; i < elements.size(); i++) {
		insertDownward(elements[i]);
	}
	elements.clear();
}

void Octree::findIntersectionDownward(Cuboid *c)
{
	if (innerNodesHoldChildren == false) {
		std::runtime_error("only use this method when inner nodes hold children");
	}
	for (auto e : innerElements) {
		if (e->isCollidingWith(*c)) {
			e->setColliding(true);
			c->setColliding(true);
		}
	}
	if (isLeaf()) {
		for (auto e : elements) {
			if (e->isCollidingWith(*c)) {
				e->setColliding(true);
				c->setColliding(true);
			}
		}
		return;
	}
	for (int i = 0; i < c_octants; i++) {
		if (whichOctants[i]) {
			octants[i]->findIntersectionDownward(c);
		}
	}
}

void Octree::insertDownward(Cuboid *c)
{
	bool atleastOneOctant = false;
	bool multipleOctants = getIntersectingOctants(*c);
	if (multipleOctants && innerNodesHoldChildren) {
		findIntersectionDownward(c);
		innerElements.push_back(c);
		return;
	}

	for (int i = 0; i < c_octants; i++) {
		if (whichOctants[i]) {
			octants[i]->insert(c, depth + 1, true);
			atleastOneOctant = true;
		}
	}

	if (atleastOneOctant == false) {
		std::runtime_error("element doesn't belong to any octant");
	}
}

int Octree::countStoredElements() const
{
	int sum = elements.size();
	sum += innerElements.size();
	if (!isLeaf()) {
		for (int i = 0; i < c_octants; i++) {
			sum += octants[i]->countStoredElements();
		}
	}
	return sum;
}

bool Octree::isLeaf() const
{
	return octants[0] == nullptr;
}

void Octree::drawSelf(void(*draw)(const Box &c, float r, float g, float b, float a)) const 
{
	(*draw)(*this, 0, 0, 1, 0.1);
	if (!isLeaf()) {
		for (int i = 0; i < c_octants; i++) {
			octants[i]->drawSelf(draw);
		}
	}
}
