#pragma once
#include "Mover.h"
#include "BasicCollision.h"
#include "Octree.h"

class Controller
{
public:
	std::vector<Cuboid> cuboids;
	BasicCollision* collisionChecker;
	Mover mover;
	bool doResolution;

	void joltTowards(float x, float y, float z)
	{
		//auto center = mover.getBounds();
		auto center = Vector3(x, y, z);
		for (auto &c : cuboids) {
			c.vel = center - c.pos;
			// speed grows with distance
			//c.vel = c.vel * 0.2;
			c.vel.normalize();
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
			delete collisionChecker;
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
		}
			break;
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

	void setCuboidSize(float size)
	{
		m_cuboidSize = size;
		auto oldSize = cuboids.size();
		cuboids.clear();
		moreElements(oldSize);
	}

	float cuboidSize()
	{
		return m_cuboidSize;
	}

private:
	static const int c_numAlgorithms = 3;
	Vector3 m_min;
	Vector3 m_max;
	int m_algorithmIndex;
	float m_cuboidSize;

	// TODO implement reinserting when changing speed
	float m_speed;

	Controller() : m_algorithmIndex(1), m_min(-2.f, -0.99f, -2.f), m_max(2.f, 2.f, 2.f), 
		m_cuboidSize(0.02f), collisionChecker(nullptr), mover(m_min, m_max, 0.3f), 
		m_speed(0.01f), doResolution(false)
	{
		setMinMax(m_min, m_max);
	}
};