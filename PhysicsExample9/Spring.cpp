#include "Spring.h"

Spring::Spring(PhysicsObject* A, PhysicsObject* B, float constant, float restingLength)
	: m_ParticleA(A)
	, m_ParticleB(B)
	, m_SpringConstant(constant)
	, m_RestingLength(restingLength)
{
}

void Spring::ApplyForces(double duration)
{
	// Get the direction of the force
	// This direction will be pointing towards ParticleA
	Vector3 direction = m_ParticleA->GetPosition() - m_ParticleB->GetPosition();

	// If the magnitude is 0, we can not call Normalize, or else we will have a
	// divide by zero error and crash
	float length = direction.Magnitude();
	if (length == 0)
		return;

	// We need to normalize the direction to ensure we are not 
	// multiplying the force by an extra amount.
	direction.Normalize();
	float distanceFromRestingLength = m_RestingLength - length;

	// Hooke's Law  Fspring = -kx
	float springForce = -m_SpringConstant * distanceFromRestingLength;

	// Apply forces to both particles
	m_ParticleA->ApplyForce(direction * -springForce);
	m_ParticleB->ApplyForce(direction * springForce);
}