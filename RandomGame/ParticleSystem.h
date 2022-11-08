#pragma once

#include "Particle.h"

#include <vector>

class ParticleSystem {
public:
	ParticleSystem();
	ParticleSystem(const Vector3& position);
	~ParticleSystem();

	void GenerateParticles(unsigned int count);
	Particle* CreateParticle(float age);
	//void CreateParticle(const Vector3& velocity);
	//void AddParticle(Particle* particle);
	void Integrate(float duration);
	void PrintParticleInfo();

	Vector3 MinVelocity;
	Vector3 MaxVelocity;
	float MinAgeMilliseconds;
	float MaxAgeMilliseconds;

private:
	//unsigned int m_NumParticles;
	std::vector<Particle*> m_Particles;
	Vector3 m_Position;
};