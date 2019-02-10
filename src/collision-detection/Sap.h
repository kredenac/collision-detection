#pragma once
#include "BasicCollision.h"
#include "Octree.h"
#include <unordered_set>
#include <algorithm>

using CollisionPair = std::pair<int, int>;

struct pairhash {
public:
	std::size_t operator()(const CollisionPair &x) const
	{
		return std::hash<int>()(x.first << 16 + x.second);
	}
};

class Point
{
public:
	Point(int index, float value, bool isBegin) :
		index(index), value(value), isBegin(isBegin)
	{

	}

	int index;
	float value;
	bool isBegin;

	bool operator < (const Point &rhs) const
	{
		return value < rhs.value;
	}

};

// pair manager, stores pairs of object indices
class PM
{
public:
	bool has(CollisionPair &p) const
	{
		if (p.first == p.second) {
			throw std::runtime_error("indices can't be the same");
		}
		if (p.first > p.second) {
			std::swap(p.first, p.second);
		}
		return m_set.find(p) != m_set.cend();
	}

	void add(CollisionPair &p)
	{
		if (p.first == p.second) {
			throw std::runtime_error("indices can't be the same");
		}
		if (p.first > p.second) {
			std::swap(p.first, p.second);
		}
		m_set.insert(p);
	}

	void add(int a, int b)
	{
		auto p = std::make_pair(a, b);
		add(p);
	}


	void setCollisions(std::vector<Cuboid>& items, Collisions &pairs)
	{
		for (auto &pair : m_set) {
			auto &first = items[pair.first];
			auto &second = items[pair.second];
			first.setColliding(true);
			second.setColliding(true);
			pairs.emplace_back(&first, &second);
		}
	}

private:
	std::unordered_set<CollisionPair, pairhash> m_set;
};

enum Axis { xAxis = 0, yAxis = 1, zAxis = 2 };

// Sweep and Prune
class Sap : public BasicCollision
{
public:
	Sap(const Vector3 &pos, const Vector3 &size, std::vector<Cuboid>& items)
		: isInit(true), octPos(pos), octSize(size)
	{
		initAxes(items);
	}

	~Sap()
	{

	}

	void initAxes(std::vector<Cuboid>& items)
	{
		m_axes[xAxis].reserve(items.size());
		m_axes[yAxis].reserve(items.size());
		m_axes[zAxis].reserve(items.size());
		// fill all three axes with 2n points each
		for (size_t i = 0; i < items.size(); i++) {

			float l, r, u, d, f, b;
			items[i].getLRUDFB(l, r, u, d, f, b);

			// xBeg
			m_axes[xAxis].emplace_back(i, // index
				l, // value
				true // isBegin
			);
			// xEnd
			m_axes[xAxis].emplace_back(i, r, false);

			//yBeg
			m_axes[yAxis].emplace_back(i, d, true);
			//yEnd
			m_axes[yAxis].emplace_back(i, u, false);

			//zBeg
			m_axes[zAxis].emplace_back(i, f, true);
			//zEnd
			m_axes[zAxis].emplace_back(i, b, false);
		}
		std::sort(m_axes[xAxis].begin(), m_axes[xAxis].end());
		std::sort(m_axes[yAxis].begin(), m_axes[yAxis].end());
		std::sort(m_axes[zAxis].begin(), m_axes[zAxis].end());
	}

	std::string getInfo() const
	{
		return "Sweep and Prune - TODO";
	}


	void markCollisions(std::vector<Cuboid>& items, Collisions &pairs) override
	{
		if (isInit) {
			isInit = false;
			Octree oct(octPos, octSize);
			oct.markCollisions(items, pairs);

			for (auto &pair : pairs)
			{
				int first = pair.first - &items[0];
				int second = pair.second - &items[0];
				m_pairs.add(first, second);
			}
			return;
		}

		m_pairs.setCollisions(items, pairs);
	}


private:
	// needed to construct helper octree in first iteration
	Vector3 octPos;
	Vector3 octSize;
	// when an instance is called first time, it uses other algorithm
	// to produce the first batch of collision pairs
	bool isInit;
	PM m_pairs;
	static const unsigned c_axes = 3;
	std::vector<Point> m_axes[c_axes];
};



