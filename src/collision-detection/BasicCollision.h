#pragma once
#include <vector>
#include <string>
#include "Cuboid.h"


// TODO move into seperate files
class BasicCollision;
#include "Mover.h"
class Controller
{
public:
	static std::vector<Cuboid> cuboids;
	static BasicCollision* collisionChecker;
	
	static void moreElements(int n = 1);
	static void lessElements(int n = 1);

	static void nextAlgorithm();
	static void previousAlgorithm();

	static Mover mover;

	static Controller& getInstance()
	{
		static Controller instance;
		return instance;
	}
	Controller(Controller const&) = delete;
	void operator=(Controller const&)  = delete;

private:
	Controller() 
	{

	}
};

class BasicCollision
{
public:
	BasicCollision();
	virtual ~BasicCollision();
	virtual std::string getInfo() const;
	virtual void markCollisions(std::vector<Cuboid>& items);
	virtual void drawSelf(void (*draw)(const Box &c, float r, float g, float b, float a)) const;
private:
	bool areColliding(Cuboid &a, Cuboid &b);
};

