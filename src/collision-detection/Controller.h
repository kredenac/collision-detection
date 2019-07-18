#pragma once
#include "Mover.h"
#include "BasicCollision.h"
#include "Octree.h"
#include "Sap.h"
#include "Logger.h"
#include <sstream>
#include <iomanip>

class Controller
{
public:
	// all objects to be considered for collision
	std::vector<Cuboid> cuboids;

	// pairs of colliding objects
	std::vector<std::pair<Cuboid*, Cuboid*>> pairs;

	// a collision detection algorithm
	BasicCollision* collisionChecker;

	// saves the delta time measurements in a file
	Logger logger;

	// updates positions of objects
	Mover mover;

	// should collision resolution be applied
	bool doResolution;

	// number of collision pairs
	int numPairs;

	// number of objects that are colliding with something
	int numInCollision;

	// all moving objects launch towards observer location
	void joltTowards(float x, float y, float z);

	// add n elements with given size. if no argument given for size
	// then current size is taken
	void moreElements(int n = 1, float cubSize = -1.f);

	// numElements -= n
	void lessElements(size_t n = 1);

	// switch to next algorithm
	void nextAlgorithm();

	// resets the current algorithm
	void resetAlgorithm();

	// switch to previous algorithm
	void prevAlgorithm();

	// gets singleton instance
	static Controller& get();

	// deleted because it is a singleton
	Controller(Controller const&) = delete;
	void operator=(Controller const&) = delete;

	// bounds within which all objects move
	void setMoverBounds(Vector3 &minv, Vector3 &maxv);

	// sets speed of all objects
	void setSpeed(float speed);

	// increases speed of all objects
	void increaseSpeed();

	// decraeses speed of all objects
	void decreaseSpeed();

	void setCuboidSize(float size);

	float cuboidSize() const;

	// gets description of the current state
	std::string getInfo() const;

	// newMaxDepth += diff
	void changeOcteeMaxDepth(int diff);

	// maxElem += diff
	void changeOctreeMaxElements(int diff);

	// changes the size of container holding all objects
	void changeContainerSize(bool toBigger);

	// resets all parameters to default values
	void resetToDefault();

	// starts the measurement of time deltas to be written to a file
	void startMeasurement();

	// ends the measurement
	void endMeasurement();

	// relative delta time. 1 when its exactly at expected speed,
	// >1 when slower, <1 when faster
	void setDelta(float dt);

	// gets the delta time
	float delta() const;

	// whether or not measurement is in progress
	bool isMeasuring() const;

private:
	// delta time
	float m_delta;

	// indicator if measuring time deltas is in progress
	bool isMeasurementInProgress;

	// number of different algorithms to cycle through
	static const int c_numAlgorithms = 5;

	// speed multiplier for moving objects
	static const float c_speedMultiplier;

	// lower bound of the container of objects
	static const float c_containerLowerBound;

	// initial speed of objects
	static const float c_defaultSpeed;

	// which algorithm is currently used
	int m_algorithmIndex;

	float m_speed;
	float m_cuboidSize;
	float m_containerSize;

	Vector3 m_min;
	Vector3 m_max;

	Controller();
};