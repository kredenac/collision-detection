#include "Octree.h"

Octree::Octree(const Vector3 &pos, const Vector3 &size) : Box(pos, size)
{
	for (int i = 0; i < c_octants; i++) {
		octants[i] = nullptr;
	}
}

Octree::~Octree()
{
	if (octants[0] == nullptr) {
		return;
	}
	for (int i = 0; i < c_octants; i++) {
		delete octants[i];
	}
}

void Octree::getIntersectingOctants(const Cuboid &c, std::vector<bool> &whichOctants) const
{
	for (int i = 0; i < c_octants; i++) {
		whichOctants[i] = octants[i]->isCollidingWith(c);
	}
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

	octants[0] = new Octree(lowPos, newSize); //smallest x,y,z
	Vector3 newPos = lowPos; newPos.z += newSize.z;
	octants[1] = new Octree(newPos, newSize); // bigger z
	newPos = lowPos; newPos.z += newSize.z; newPos.y += newSize.y;
	octants[2] = new Octree(newPos, newSize); // bigger z y
	newPos = lowPos; newPos.y += newSize.y;
	octants[3] = new Octree(newPos, newSize); // bigger y
	lowPos.x += newSize.x;
	newPos = lowPos;
	octants[4] = new Octree(newPos, newSize); // bigger x
	newPos.z += newSize.z;
	octants[5] = new Octree(newPos, newSize); // bigger x, z
	newPos = lowPos; newPos.y += newSize.y;
	octants[6] = new Octree(newPos, newSize); // bigger x, y
	newPos.z += newSize.z;
	octants[7] = new Octree(newPos, newSize); // bigger x, y,z
}

// TODO FIXME: it can infinitely try to insert when two elements
// are overlapping
void Octree::insert(Cuboid *c, int d, bool markColl)
{
	if (markColl) {
		for (auto e : elements) {
			if (e->isCollidingWith(*c)) {
				e->setColliding(true);
				c->setColliding(true);
			}
		}
	}
	elements.push_back(c);

	// if there's space, just put it and return 
	if (elements.size() <= c_maxElem) {
		return;
	}
	// if max elem reached, subdivide
	subdivide();

	// then insert elements into octants
	std::vector<bool> whichOctants(c_octants);
	for (auto e : elements) {
		
		getIntersectingOctants(*e, whichOctants);
		for (int i = 0; i < c_octants; i++) {
			if (whichOctants[i]) {
				octants[i]->insert(e, d + 1);
			}
		}
	}
	elements.clear();
}


bool Octree::isLeaf() const
{
	return octants[0] == nullptr;
}