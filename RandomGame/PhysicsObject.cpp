#include "PhysicsObject.h"
#include <iostream>

//#define PRINT_DEBUG_INFO

PhysicsObject::PhysicsObject()
	: PhysicsObject(Vector3())
{
}

PhysicsObject::PhysicsObject(const Vector3& position)
	: position(position)
	, velocity(0.0f)
	, acceleration(0.0f)
	, damping(0.98f)
	, invMass(1.0f)
	, m_IsStatic(false)
{
#ifdef PRINT_DEBUG_INFO
	printf("Particle::Particle();\n");
#endif
}

PhysicsObject::~PhysicsObject() {
#ifdef PRINT_DEBUG_INFO
	printf("Particle::~Particle();\n");
#endif
}

PhysicsObject::PhysicsObject(const PhysicsObject& particle) {
#ifdef PRINT_DEBUG_INFO
	printf("Particle::Particle(const Particle& particle);\n");
#endif
}

PhysicsObject PhysicsObject::operator=(const PhysicsObject& particle) {
#ifdef PRINT_DEBUG_INFO
	printf("Particle::operator=();\n");
#endif
	return PhysicsObject();
}


// NewPosition = OldPosition + Velocity * ChangeInTime
// 
// p1 = p0 + v*dt
// 
// p0 = origin position
// p1 = new position
// v  = velocity
// dt = deltatime
// 
// position = position + velocity * dt; 

// NewVelocity = OldVelocity + Acceleration * ChangeTime
//
// v1 = v0 + a * dt
// 
// a = acceleration
//
// velocity = velocity + acceleration * dt;
void PhysicsObject::PrintInfo() {
#ifdef PRINT_DEBUG_INFO
	std::cout << "-----------------\n";
	std::cout << "Velocity(" << velocity.x << ", " << velocity.y << ", " << velocity.z << ")\n";
	std::cout << "Position(" << position.x << ", " << position.y << ", " << position.z << ")\n";
#endif
}

void PhysicsObject::Integrate(float dt) {
	//velocity = velocity + acceleration * dt;
	//velocity *= damping;
	//position = position + velocity * dt;

	// f = ma  force = mass * acceleration
	// a = f/m
	// a = f*invMass;

	// 0 or negative mass object will be a "static" object.
	if (invMass <= 0 || m_IsStatic)
		return;


	acceleration = force * invMass;
	velocity.addScaledVector(acceleration, dt);

	prevPosition = position;
	position.addScaledVector(velocity, dt);

	//velocity *= damping;
}

void PhysicsObject::ApplyForce(const Vector3& direction) {
	if (invMass <= 0 || m_IsStatic)
		return;

	force += direction;
}

void PhysicsObject::KillAllForces() {
	force.Set(0.f, 0.f, 0.f);
}
