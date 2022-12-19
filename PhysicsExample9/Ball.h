#pragma once

#include "PhysicsObject.h"
#include "gdp/GameObject.h"

class Ball {
public:
	PhysicsObject* physicsObject;
	gdp::GameObject* gameObject;
};