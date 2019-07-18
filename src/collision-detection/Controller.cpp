#include "Controller.h"

const float Controller::c_speedMultiplier = 2.f;
const float Controller::c_containerLowerBound = -0.99f;
const float Controller::c_defaultSpeed = 0.01f;

Controller::Controller()
{
	resetToDefault();
}

void Controller::resetToDefault()
{
	collisionChecker = nullptr;
	doResolution = true;
	m_delta = 1.f;
	numPairs = 0;
	numInCollision = 0;
	m_algorithmIndex = 1;
	m_speed = c_defaultSpeed;
	m_cuboidSize = 0.02f;
	m_containerSize = 2.f;
	Vector3 min(-m_containerSize, c_containerLowerBound, -m_containerSize);
	Vector3 max(m_containerSize, m_containerSize, m_containerSize);
	setMoverBounds(min, max);
	lessElements(cuboids.size());
	moreElements(1000);
	isMeasurementInProgress = false;
}

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
	case 4: {
		collisionChecker = new Idle();
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
	if (size < 1e-6) {
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

void Controller::changeContainerSize(bool toBigger)
{
	float diff = m_containerSize * 0.01 * m_delta;
	if (!toBigger) {
		diff = -diff;
	}
	if (m_containerSize + diff < 0.1f) {
		return;
	}
	m_containerSize += diff;
	Vector3 min(-m_containerSize, c_containerLowerBound, -m_containerSize);
	Vector3 max(m_containerSize, m_containerSize, m_containerSize);
	setMoverBounds(min, max);
}

std::string Controller::getInfo() const
{
	std::stringstream str;
	std::string response = doResolution ? "on" : "off";
	str << std::setprecision(2) << std::setfill(' ') << std::right << std::setw(5);
	str << "Number of elements: " << cuboids.size() << ", size: " << m_cuboidSize;
	if (m_speed != c_defaultSpeed) {
		str << ", speed: " << m_speed;
	}

	str << ", response: " << response << ", colliding: " << std::setw(4) <<
		numInCollision << ", pairs : " << std::setw(4) << numPairs;

	return str.str();
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

void Controller::startMeasurement()
{
	isMeasurementInProgress = true;
	std::stringstream str;
	std::string response = doResolution ? "on" : "off";
	str << collisionChecker->name() + " _response_" << response;
	
	logger.startNewMeasurement(str.str());
}

void Controller::endMeasurement()
{
	isMeasurementInProgress = false;
	logger.finishMeasurement();
}

void Controller::setDelta(float dt)
{
	m_delta = dt;

	if (isMeasurementInProgress) {
		logger.addInstance(dt, m_cuboidSize, cuboids.size(), collisionChecker->additionalLogData());
	}
}

float Controller::delta() const 
{
	return m_delta;
}

bool Controller::isMeasuring() const
{
	return isMeasurementInProgress;
}