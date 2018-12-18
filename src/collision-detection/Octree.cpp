#include "Octree.h"

Octree::Octree(const Vector3 &pos, const Vector3 &halfSize) : pos(pos), halfSize(halfSize)
{
	for (int i = 0; i < c_octants; i++) {
		children[i] = nullptr;
	}
}


Octree::~Octree()
{
	if (children[0] == nullptr) {
		throw std::runtime_error("maybe this shouldn't happen");
		return;
	}
	for (int i = 0; i < c_octants; i++) {
		delete children[i];
	}
}

bool Octree::intersectsWith(const Cuboid &c) const
{
	return false;
}

void Octree::getIntersectingOctants(const Cuboid &c, std::vector<bool> &whichOctants) const
{

}

void Octree::markCollisions(std::vector<Cuboid>& items) 
{

}

bool Octree::isLeaf() const
{
	return children[0] == nullptr;
}