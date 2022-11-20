#pragma once

#include "Vector3.h"



struct BoundingBox {
	Vector3 centerPoint;
	Vector3 minPoints;
	Vector3 maxPoints;
	Vector3 halfExtents;
};

struct BoundingSphere {
	Vector3 centerPoint;
	float radius;
};