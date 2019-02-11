#include "Controller.h"

const float Controller::c_speedMultiplier = 2.f;

void Controller::joltTowards(float x, float y, float z)
{
	//auto center = mover.getBounds();
	auto center = Vector3(x, y, z);
	for (auto &c : cuboids) {
		c.vel = center - c.pos;
		// speed grows with distance
		// c.vel = c.vel * 0.2;
		// c.vel.normalize();
	}
}

void Controller::moreElements(int n, float cubSize)
{
	m_cuboidSize = cubSize != -1.f ? cubSize : m_cuboidSize;
	Vector3 minSpawnAt = m_min + m_cuboidSize;
	Vector3 maxSpawnAt = m_max + (-m_cuboidSize);
	for (int i = 0; i < n; i++) {
		cuboids.push_back(Cuboid(Vector3::randVec(minSpawnAt, maxSpawnAt), m_cuboidSize));
	}
}

void Controller::lessElements(size_t n)
{
	if (n > cuboids.size()) {
		cuboids.clear();
		return;
	}
	cuboids.erase(cuboids.end() - n, cuboids.end());
}

void Controller::nextAlgorithm()
{
	m_algorithmIndex = (m_algorithmIndex + 1) % c_numAlgorithms;
	resetAlgorithm();
}

void Controller::resetAlgorithm()
{
	//printf("now %d\n", m_algorithmIndex);
	if (collisionChecker != nullptr) {
		if (collisionChecker->getInfo().find("Sweep") == std::string::npos) {
			delete collisionChecker;
		}
	}
	switch (m_algorithmIndex) {
	case 0:
		collisionChecker = new BasicCollision();
		break;
	case 1:
	case 2: {
		auto bounds = mover.getBounds();
		collisionChecker = new Octree(bounds.pos, bounds.size);
		Octree::innerNodesHoldChildren = m_algorithmIndex != 1;
		break;
	}
	case 3: {
		auto bounds = mover.getBounds();
		collisionChecker = Sap::get(bounds.pos, bounds.size, cuboids);
		break;
	}
	default:
		break;
	}
}

void Controller::prevAlgorithm()
{
	m_algorithmIndex = (m_algorithmIndex + c_numAlgorithms - 1) % c_numAlgorithms;
	resetAlgorithm();
}

Controller& Controller::get()
{
	static Controller instance;
	return instance;
}

void Controller::setMoverBounds(Vector3 &minv, Vector3 &maxv)
{
	m_min = minv;
	m_max = maxv;
	mover = Mover(m_min, m_max, m_speed);
	resetAlgorithm();
}

void Controller::setSpeed(float speed)
{
	m_speed = speed;
	mover = Mover(m_min, m_max, m_speed);
}

void Controller::increaseSpeed()
{
	setSpeed(m_speed * c_speedMultiplier);
}

void Controller::decreaseSpeed()
{
	setSpeed(m_speed / c_speedMultiplier);
}

void Controller::setCuboidSize(float size)
{
	if (size < 1e-7) {
		return;
	}
	m_cuboidSize = size;
	auto oldSize = cuboids.size();
	cuboids.clear();
	moreElements(oldSize);
}

float Controller::cuboidSize() const
{
	return m_cuboidSize;
}

std::string Controller::getInfo() const
{
	std::stringstream str;

	return "Number of elements: " + std::to_string(cuboids.size()) +
		", size: " + std::to_string(m_cuboidSize) + ", colliding: " +
		std::to_string(numInCollision) + ", pairs : " + std::to_string(numPairs);
}

void Controller::changeOcteeMaxDepth(int diff)
{
	auto depth = Octree::maxDepth + diff;
	if (depth <= 0) {
		return;
	}
	Octree::maxDepth = depth;
}

void Controller::changeOctreeMaxElements(int diff)
{
	auto max = Octree::maxElem + diff;
	if (max <= 0) {
		return;
	}
	Octree::maxElem = max;
}