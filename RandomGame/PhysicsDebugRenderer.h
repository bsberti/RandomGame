#pragma once

#include "PhysicsObject.h"

#include <vector>

class PhysicsDebugRenderer
{
public:
	PhysicsDebugRenderer();
	~PhysicsDebugRenderer();

	void Render();

	void AddPhysicsObject(PhysicsObject* object);

private:
	std::vector<PhysicsObject*> m_PhysicsObjects;
	unsigned int m_BoundingBoxModelId;
	unsigned int m_BoundingSphereModelId;
	unsigned int m_DebugShaderProgramId;
};