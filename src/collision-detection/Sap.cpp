#include "Sap.h"

std::size_t pairhash::operator()(const CollisionPair &x) const
{
	return std::hash<int>()((x.first << 16) + x.second);
}

Point::Point(int index, float value, bool isBegin) :
	index(index), value(value), isBegin(isBegin)
{

}

bool Point::operator < (const Point &rhs) const
{
	return value < rhs.value;
}

bool PairStorage::has(CollisionPair &p) const
{
	if (p.first == p.second) {
		throw std::runtime_error("indices can't be the same");
	}
	if (p.first > p.second) {
		std::swap(p.first, p.second);
	}
	return m_set.find(p) != m_set.cend();
}

void PairStorage::add(CollisionPair &p)
{
	if (p.first == p.second) {
		throw std::runtime_error("indices can't be the same");
	}
	if (p.first > p.second) {
		std::swap(p.first, p.second);
	}
	m_set.insert(p);
}

void PairStorage::add(int a, int b)
{
	auto p = std::make_pair(a, b);
	add(p);
}

void PairStorage::remove(int a, int b)
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

void PairStorage::setCollisions(std::vector<Cuboid>& items, Collisions &pairs)
{
	for (auto &pair : m_set) {
		auto &first = items[pair.first];
		auto &second = items[pair.second];
		first.setColliding(true);
		second.setColliding(true);
		pairs.emplace_back(&first, &second);
	}
}


Sap* Sap::get(const Vector3 &pos, const Vector3 &size, std::vector<Cuboid>& items)
{
	static Sap instance(pos, size, items);
	// if number of elements or their size has changed, needs to bee reinitialized
	if (instance.m_lastNumElements != items.size() ||
		instance.lastCubeSize != items[0].size) {

		instance = Sap(pos, size, items);
		instance.m_lastNumElements = items.size();
		instance.lastCubeSize = items[0].size;
	}
	return &instance;
}

void Sap::markCollisions(std::vector<Cuboid>& items, Collisions &pairs)
{
	if (isInit) {
		initPhase(items, pairs);
		return;
	}

	updateCollisions(items);

	m_pairs.setCollisions(items, pairs);
}

std::string Sap::getInfo() const
{
	return "Sweep and Prune, number of swaps: " + std::to_string(m_numberOfSwaps);
}

Sap::Sap(const Vector3 &pos, const Vector3 &size, std::vector<Cuboid>& items)
	: isInit(true), octPos(pos), octSize(size), m_numberOfSwaps(0)
{
	lastCubeSize = Vector3(0, 0, 0);
	m_lastNumElements = 0;
	initAxes(items);
}

Sap::~Sap()
{

}

void Sap::initAxes(std::vector<Cuboid>& items)
{
	for (auto &axis : m_axes) {
		axis.reserve(items.size() * 2);
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
void Sap::updateAxesPoints(std::vector<Cuboid>& items) {

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

void Sap::initPhase(std::vector<Cuboid>& items, Collisions &pairs)
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

void Sap::updateCollisions(std::vector<Cuboid>& items)
{
	int numMoved = 0;
	updateAxesPoints(items);
	for (auto &axis : m_axes) {
		numMoved += sortAxis(axis, items);
	}
	m_numberOfSwaps = numMoved;
}

// returns the number of moved elements
int Sap::sortAxis(std::vector<Point> &axis, std::vector<Cuboid> &items)
{
	int numMoved = 0;
	for (size_t i = 1; i < axis.size(); i++) {
		auto currPoint = axis[i];
		float value = currPoint.value;

		// currPoint is moved to the left .
		// update position of elements using insertion sort
		// add/remove collisions accordingly. 
		int j;
		for (j = i - 1; j >= 0 && axis[j].value > value; j--) {
			auto &toMove = axis[j];

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
			axis[j + 1] = toMove;
		}
		numMoved += i - j - 1;
		axis[j + 1] = currPoint;
	}
	return numMoved;
}