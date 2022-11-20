#pragma once

#include "PhysicsObject.h"

#include <map>
#include <vector>
#include "Spring.h"
#include "Shapes.h"

class PhysicsSystem {
public:
	PhysicsSystem();
	~PhysicsSystem();

	PhysicsObject* CreatePhysicsObject(const Vector3& position, iShape* shape);
	void AddSpring(Spring* spring);
	void UpdateStep(float duration);
	void AddTriangleToAABBCollisionCheck(int hash, Triangle* triangle);

	bool RayCast(Ray ray, PhysicsObject** hitObject);

	const std::map<int, std::vector<Triangle*>>& GetAABBStructure() {
		return m_AABBStructure;
	}

	std::vector<PhysicsObject*> m_PhysicsObjects;
private:
	bool CollisionTest(const Vector3& posA, iShape* shapeA, const Vector3& posB, iShape* shapeB);
	bool CollisionTest(const Vector3& posA, Sphere* a, const Vector3& posB, Sphere* b);
	bool CollisionTest(const Vector3& posA, Sphere* a, const Vector3& posB, AABB* b);
	bool CollisionTest(const Vector3& posA, Sphere* a, const Vector3& posB, Triangle* b);

	//bool CollisionTest(AABB* a, Triangle* b);		// Not implementing
	//bool CollisionTest(AABB* a, AABB* b);			// Not implementing
	//bool CollisionTest(Triangle* a, Triangle* b);	// Not implementing

	std::vector<Spring*> m_Springs;
	std::map<int, std::vector<Triangle*>> m_AABBStructure;
};