#pragma once
#include "BasicCollision.h"
#include "Octree.h"
#include <unordered_set>
#include <algorithm>

using CollisionPair = std::pair<int, int>;

struct pairhash {
public:
	std::size_t operator()(const CollisionPair &x) const;
};

class Point
{
public:
	Point(int index, float value, bool isBegin);

	// compares order with another point
	bool operator < (const Point &rhs) const;

	// index of this point within sorted vector
	int index;

	// position along axis
	float value;

	// is the start of a projection along axis
	bool isBegin;
};

// pair manager, stores pairs of object indices
class PairStorage
{
public:
	bool has(CollisionPair &p) const;

	void add(CollisionPair &p);

	void add(int a, int b);

	void remove(int a, int b);

	void setCollisions(std::vector<Cuboid>& items, Collisions &pairs);

private:
	std::unordered_set<CollisionPair, pairhash> m_set;
};


// Sweep and Prune
// assumes working with same objects
class Sap : public BasicCollision
{
enum Axis { xAxis = 0, yAxis = 1, zAxis = 2 };

public:
	// gets the singleton. if parameters are unchanged, then it returns the same instance
	// otherwise, makes a new one
	static Sap* get(const Vector3 &pos, const Vector3 &size, std::vector<Cuboid>& items);

	void markCollisions(std::vector<Cuboid>& items, Collisions &pairs) override;

	std::string getInfo() const override;

	std::string name() const;

	std::string additionalLogData() const;
private:
	Sap(const Vector3 &pos, const Vector3 &size, std::vector<Cuboid>& items);

	~Sap();

	// fills three vectors with 2n points
	void initAxes(std::vector<Cuboid>& items);

	// updates values of all points
	void updateAxesPoints(std::vector<Cuboid>& items);

	// in the first phase it's needed to initialize collision pairs
	// with help of some other algorithm
	void initPhase(std::vector<Cuboid>& items, Collisions &pairs);

	// core of the algorithm, moves items in arrays
	// until they are sorted whilst updating collisions
	void updateCollisions(std::vector<Cuboid>& items);

	// returns the number of moved elements
	int sortAxis(std::vector<Point> &axis, std::vector<Cuboid> &items);

	// information about cuboids from last frame
	Vector3 lastCubeSize;
	size_t m_lastNumElements;

	// used for reporting number of swamps per update
	int m_numberOfSwaps;

	// needed to construct helper octree in first iteration
	Vector3 octPos;
	Vector3 octSize;

	// when an instance is called the first time, it uses another algorithm
	// to produce the first batch of collision pairs
	bool isInit;

	// saved collision pairs from last frame
	PairStorage m_pairs;
	static const unsigned c_axes = 3;

	// axes that contain projections
	std::vector<Point> m_axes[c_axes];
};
