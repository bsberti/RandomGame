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
	, currentTimer(0.0)
	, m_IsStatic(false)
	, movingXPos(false)
	, movingYPos(false)
	, movingZPos(false)
	, movingXNeg(false)
	, movingYNeg(false)
	, movingZNeg(false)
{
#ifdef PRINT_DEBUG_INFO
	printf("Particle::Particle();\n");
#endif
	repositioningTimer = (rand() % 6) + 3.0;
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

void PhysicsObject::PrintInfo() {
#ifdef PRINT_DEBUG_INFO
	std::cout << "-----------------\n";
	std::cout << "Velocity(" << velocity.x << ", " << velocity.y << ", " << velocity.z << ")\n";
	std::cout << "Position(" << position.x << ", " << position.y << ", " << position.z << ")\n";
#endif
}

void PhysicsObject::Repositioning() {
	if (invMass <= 0 || m_IsStatic)
		return;

	position = Vector3(rand() % 20, (rand() % 10) + 3, rand() % 20);
	movingXPos = false;
	movingYPos = false;
	movingZPos = false;
	movingXNeg = false;
	movingYNeg = false;
	movingZNeg = false;

	repositioningTimer = (rand() % 6) + 3.0;
	currentTimer = 0.0;

	int randomNumber = rand() % 7;
	switch (randomNumber)
	{
	case 0:
		movingXPos = true;
		break;
	case 1:
		movingYPos = true;
		break;
	case 2:
		movingZPos = true;
		break;
	case 3:
		movingXNeg = true;
		break;
	case 4:
		movingYNeg = true;
		break;
	case 5:
		movingZNeg = true;
		break;
	default:
		break;
	}
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

	if (movingXPos) {
		force = Vector3(0.5f, 0.0f, 0.0f);
	}

	if (movingYPos) {
		force = Vector3(0.0f, 0.5f, 0.0f);
	}

	if (movingZPos) {
		force = Vector3(0.0f, 0.0f, 0.5f);
	}

	if (movingXNeg) {
		force = Vector3(-0.5f, 0.0f, 0.0f);
	}

	if (movingYNeg) {
		force = Vector3(0.0f, -0.5f, 0.0f);
	}

	if (movingZNeg) {
		force = Vector3(0.0f, 0.0f, -0.5f);
	}

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
