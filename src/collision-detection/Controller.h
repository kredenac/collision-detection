#pragma once
#include "Mover.h"
#include "BasicCollision.h"
#include "Octree.h"
#include "Sap.h"
#include <sstream>

class Controller
{
public:
	std::vector<Cuboid> cuboids;
	std::vector<std::pair<Cuboid*, Cuboid*>> pairs;
	BasicCollision* collisionChecker;
	Mover mover;
	bool doResolution;
	float delta;
	int numPairs;
	int numInCollision;

	void joltTowards(float x, float y, float z)
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

	void moreElements(int n = 1, float cubSize = -1.f)
	{
		m_cuboidSize = cubSize != -1.f ? cubSize : m_cuboidSize;
		Vector3 minSpawnAt = m_min + m_cuboidSize;
		Vector3 maxSpawnAt = m_max + (-m_cuboidSize);
		for (int i = 0; i < n; i++) {
			cuboids.push_back(Cuboid(Vector3::randVec(minSpawnAt, maxSpawnAt), m_cuboidSize));
		}
	}

	void lessElements(size_t n = 1)
	{
		if (n > cuboids.size()) {
			cuboids.clear();
			return;
		}
		cuboids.erase(cuboids.end() - n, cuboids.end());
	}

	void nextAlgorithm()
	{
		m_algorithmIndex = (m_algorithmIndex + 1) % c_numAlgorithms;
		resetAlgorithm();
	}

	void resetAlgorithm()
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

	void prevAlgorithm()
	{
		m_algorithmIndex = (m_algorithmIndex + c_numAlgorithms - 1) % c_numAlgorithms;
		resetAlgorithm();
	}

	static Controller& get()
	{
		static Controller instance;
		return instance;
	}

	Controller(Controller const&) = delete;
	void operator=(Controller const&) = delete;

	void setMinMax(Vector3 &minv, Vector3 &maxv)
	{
		m_min = minv;
		m_max = maxv;
		mover = Mover(m_min, m_max, m_speed);
		resetAlgorithm();
	}

	void setSpeed(float speed)
	{
		m_speed = speed;
		mover = Mover(m_min, m_max, m_speed);
	}

	void increaseSpeed()
	{
		setSpeed(m_speed * c_speedMultiplier);
	}

	void decreaseSpeed()
	{
		setSpeed(m_speed / c_speedMultiplier);
	}

	void setCuboidSize(float size)
	{
		if (size < 1e-7) {
			return;
		}
		m_cuboidSize = size;
		auto oldSize = cuboids.size();
		cuboids.clear();
		moreElements(oldSize);
	}

	float cuboidSize() const
	{
		return m_cuboidSize;
	}

	std::string getInfo() const
	{
		std::stringstream str;

		return "Number of elements: " + std::to_string(cuboids.size()) +
			", size: " + std::to_string(m_cuboidSize) + ", colliding: " + 
			std::to_string(numInCollision) + ", pairs : " + std::to_string(numPairs);
	}

	void changeOcteeMaxDepth(int diff)
	{
		auto depth = Octree::maxDepth + diff;
		if (depth <= 0) {
			return;
		}
		Octree::maxDepth = depth;
	}

	void changeOctreeMaxElements(int diff)
	{
		auto max = Octree::maxElem + diff;
		if (max <= 0) {
			return;
		}
		Octree::maxElem = max;
	}

private:
	static const int c_numAlgorithms = 4;
	static const float c_speedMultiplier;
	Vector3 m_min;
	Vector3 m_max;
	int m_algorithmIndex;
	float m_cuboidSize;

	// TODO implement reinserting when changing speed
	float m_speed;

	Controller() : m_algorithmIndex(1), m_min(-2.f, -0.99f, -2.f), m_max(2.f, 2.f, 2.f),
		m_cuboidSize(0.02f), collisionChecker(nullptr), mover(m_min, m_max, 0.3f),
		delta(0.f), m_speed(0.01f), doResolution(false)
	{
		setMinMax(m_min, m_max);
	}
};