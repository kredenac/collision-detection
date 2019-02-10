#pragma once
#include "BasicCollision.h"
#include "Octree.h"
#include <unordered_set>
#include <algorithm>

// TODO: maybe create a class with overloaded equality
// to ignore pair ordering
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

	void remove(int a, int b) 
	{
		if (a == b) {
			throw std::runtime_error("indices can't be the same");
		}
		if (a > b) {
			std::swap(a, b);
		}
		auto p = std::make_pair(a, b);
		m_set.erase(p);
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
// assumes working with same objects
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
		for (auto &axis : m_axes) {
			axis.reserve(items.size()*2);
		}
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
			m_axes[zAxis].emplace_back(i, b, true);
			//zEnd
			m_axes[zAxis].emplace_back(i, f, false);
		}
		for (auto &axis : m_axes) {
			std::sort(axis.begin(), axis.end());
		}
	}

	// updates values of all points
	void updateAxesPoints(std::vector<Cuboid>& items) {
		
		for (auto &point : m_axes[xAxis]) {
			auto &elem = items[point.index];
			point.value = point.isBegin ? elem.left() : elem.right();
		}
		for (auto &point : m_axes[yAxis]) {
			auto &elem = items[point.index];
			point.value = point.isBegin ? elem.down() : elem.up();
		}
		for (auto &point : m_axes[zAxis]) {
			auto &elem = items[point.index];
			point.value = point.isBegin ? elem.back() : elem.front();
		}
	}

	std::string getInfo() const
	{
		return "Sweep and Prune, number of swaps: " + std::to_string(m_numberOfSwaps);
	}

	void initPhase(std::vector<Cuboid>& items, Collisions &pairs)
	{
		isInit = false;
		Octree oct(octPos, octSize);
		oct.markCollisions(items, pairs);

		for (auto &pair : pairs)
		{
			int first = pair.first - &items[0];
			int second = pair.second - &items[0];
			m_pairs.add(first, second);
		}
	}

	void updateCollisions(std::vector<Cuboid>& items)
	{
		int numMoved = 0;
		updateAxesPoints(items);
		for (auto &axis : m_axes) {
			numMoved += sortAxis(axis, items);
		}
		m_numberOfSwaps = numMoved;
	}

	// returns the number of moved elements
	int sortAxis(std::vector<Point> &axis, std::vector<Cuboid> &items)
	{
		int numMoved = 0;
		for (size_t j = 1; j < axis.size(); j++) {
			auto currPoint = axis[j];
			float value = currPoint.value;

			// currPoint is moved to the left .
			// update position of elements using insertion sort
			// add/remove collisions accordingly. 
			int i;
			for (i = j - 1; i >= 0 && axis[i].value > value; i--) {
				auto &toMove = axis[i];
				
				// toMove.begin skips over currPoint.end, which means 
				// they are no longer colliding
				if (toMove.isBegin && !currPoint.isBegin) {
					m_pairs.remove(toMove.index, currPoint.index);
				}

				// tomove.end skips oevr currPoint.begin, which means
				// that they now they might have an intersection
				if (!toMove.isBegin && currPoint.isBegin) {
					auto &first = items[toMove.index];
					auto &second = items[currPoint.index];
					if (first.isCollidingWith(second)) {
						m_pairs.add(toMove.index, currPoint.index);
					}
				}
				axis[i + 1] = toMove;
			}
			numMoved += j - i + 1;
			axis[i + 1] = currPoint;
		}
		return numMoved;
	}

	void markCollisions(std::vector<Cuboid>& items, Collisions &pairs) override
	{
		if (isInit) {
			initPhase(items, pairs);
			return;
		}

		updateCollisions(items);

		m_pairs.setCollisions(items, pairs);
	}


private:
	// used for reporting number of swamps per update
	int m_numberOfSwaps;
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



