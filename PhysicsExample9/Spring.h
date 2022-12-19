#pragma once

#include "PhysicsObject.h"

class Spring
{
public:
	Spring(PhysicsObject* A, PhysicsObject* B, float constant, float restingLength);

	void ApplyForces(double duration);

private:
	PhysicsObject* m_ParticleA;
	PhysicsObject* m_ParticleB;
	float m_SpringConstant;
	float m_RestingLength;
};