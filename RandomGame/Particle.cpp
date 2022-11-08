#include "Particle.h"
#include <iostream>

//#define PRINT_DEBUG_INFO

Particle::Particle()
	: position(0.0f)
	, velocity(0.0f)
	, acceleration(0.0f)
	, damping(1.0f)
	, invMass(1.0f)
	, age(-1.0f)
	, force(0.0f)
{
#ifdef PRINT_DEBUG_INFO
	printf("Particle::Particle();\n");
#endif
}

Particle::~Particle() {
#ifdef PRINT_DEBUG_INFO
	printf("Particle::~Particle();\n");
#endif
}

Particle::Particle(const Particle& particle) {
#ifdef PRINT_DEBUG_INFO
	printf("Particle::Particle(const Particle& particle);\n");
#endif
	damping = particle.damping;
	invMass = particle.invMass;
	age = particle.age;
}

Particle Particle::operator=(const Particle& particle) {
#ifdef PRINT_DEBUG_INFO
	printf("Particle::operator=();\n");
#endif
	return Particle();
}

void Particle::PrintInfo() {
#ifdef PRINT_DEBUG_INFO
	std::cout << "-----------------\n";
	std::cout << "Velocity(" << velocity.x << ", " << velocity.y << ", " << velocity.z << ")\n";
	std::cout << "Position(" << position.x << ", " << position.y << ", " << position.z << ")\n";
#endif
}

void Particle::Integrate(float dt) {
	if (age <= 0.f)
		return;

	if (invMass <= 0)
		return;

	acceleration = force * invMass;

	velocity.addScaledVector(acceleration, dt);
	velocity *= damping;
	position.addScaledVector(velocity, dt);
	if (position.y < 0) {
		position.y = 0;
		velocity *= 0;
	}

	age -= dt;
	PrintInfo();
}

void Particle::ApplyForce(const Vector3& direction) {
	force += direction;
}

void Particle::KillAllForces() {
	force = 0;
}