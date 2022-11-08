#include "ParticleSystem.h";
#include <random>

ParticleSystem::ParticleSystem() : ParticleSystem(Vector3(0.f)) {
}

ParticleSystem::ParticleSystem(const Vector3& position)
	: m_Position(position)
	, MinAgeMilliseconds(1.f)
	, MaxAgeMilliseconds(2.f)
	, MinVelocity(Vector3(0.f))
	, MaxVelocity(Vector3(0.f, 1.f, 0.f)) {
}

ParticleSystem::~ParticleSystem() {
	for (int i = 0; i < m_Particles.size(); i++) {
		delete m_Particles[i];
	}
}

void ParticleSystem::GenerateParticles(unsigned int count) {
	m_Particles.resize(count);
}

Particle* ParticleSystem::CreateParticle(float age) {
	Particle* newParticle = new Particle();
	newParticle->age = age;
	m_Particles.push_back(newParticle);
	return newParticle;
}

float MyRandFloat(float min, float max) {
	if (max == min)
		return 0.f;

	int diff = (max - min) * 1000;
	return min + (rand() % diff) / 1000.f;
}

/*void ParticleSystem::CreateParticle(const Vector3& velocity) {
	bool particleCreated = false;
	for (int i = 0; i < m_Particles.size(); i++) {
		Particle* p = m_Particles[i];

		if (p->age <= 0.f) {
			// Reset the position to the default position
			p->position = m_Position;
			
			//Example of random velocity particles
			//p->velocity.x = MyRandFloat(MaxVelocity.x, MinVelocity.x);
			//p->velocity.y = MyRandFloat(MaxVelocity.y, MinVelocity.y);
			//p->velocity.z = MyRandFloat(MaxVelocity.z, MinVelocity.z);

			// ageDifference = (MaxAge - MinAge) * 1000
			// MinAge + Rand() % ageDiferrence;
			int ageDiff = (MaxAgeMilliseconds - MinAgeMilliseconds) * 1000;
			p->age = (float)(MinAgeMilliseconds + rand() % ageDiff) / 1000.f;

			//printf("Created a particle:\n");
			//printf("Age: %.2f\n", p->age);
			//printf("Velocity: (%.2f, %.2f, %.2f)\n", p->velocity.x, p->velocity.y, p->velocity.z);

			particleCreated = true;
			break;
		}
	}

	if (!particleCreated) {
		printf("ParticleSystem::CreateParticle Maximum number of particles already created!\n");
		return;
	}
}*/

void ParticleSystem::Integrate(float duration) {
	//for (int i = 0; i < m_Particles.size(); i++) {
	//	m_Particles[i]->ApplyForce(Vector3(0.f, -0.981f, 0.f));
	//}

	for (int i = 0; i < m_Particles.size(); i++) {
		Particle* p = m_Particles[i];
		if (p->age > 0.f) {
			p->Integrate(duration);
		}
	}

	for (int i = 0; i < m_Particles.size(); i++) {
		m_Particles[i]->KillAllForces();
	}
}

void ParticleSystem::PrintParticleInfo() {
	for (int i = 0; i < m_Particles.size(); i++) {
		Particle* p = m_Particles[i];
		if (p->age > 0.f) {
			p->PrintInfo();
		}
	}
}