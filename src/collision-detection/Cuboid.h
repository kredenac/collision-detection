#pragma once
#include "Vector3.h"

class Cuboid
{
public:
	Cuboid(Vector3&& pos) : Cuboid(pos, Vector3(1, 1, 1))
	{

	}

	Cuboid(Vector3& pos, Vector3&& size) : pos(pos), size(size)
	{

	}

	~Cuboid();

	bool hasCollision() const {
		return m_hasCollision;
	}

	void setColliding(bool isColliding) {
		m_hasCollision = isColliding;
	}

	Vector3 pos;
	Vector3 size;

private:
	bool m_hasCollision;
};

