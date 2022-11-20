#include "PhysicsSystem.h"
#include "PhysicsUtils.h"

PhysicsSystem::PhysicsSystem() {
}

PhysicsSystem::~PhysicsSystem() {
	for (int i = 0; i < m_PhysicsObjects.size(); i++) {
		delete m_PhysicsObjects[i];
	}
	m_PhysicsObjects.clear();

	// delete[] m_Particles[0];		Can check if this works (Not sure)
	//  -> edit: Nope, use this for array of pointers
}

PhysicsObject* PhysicsSystem::CreatePhysicsObject(const Vector3& position, iShape* shape) {
	PhysicsObject* physicsObject = new PhysicsObject(position);
	physicsObject->pShape = shape;
	m_PhysicsObjects.push_back(physicsObject);
	return physicsObject;
}

void PhysicsSystem::AddSpring(Spring* spring) {
	m_Springs.push_back(spring);
}

int CalculateHashValuePhysics(float x, float y, float z)
{
	int hashValue = 0;

	assert(x + 128 > 0);
	assert(y + 300 > 0);
	assert(z + 128 > 0);

	hashValue += floor(x + 128) / 100 * 10000;
	hashValue += floor(y + 300) / 100 * 100;
	hashValue += floor(z + 128) / 100;
	return hashValue;
}

int CalculateHashValuePhysics(const Vector3& v)
{
	return CalculateHashValuePhysics(v.x, v.y, v.z);
}

void PhysicsSystem::UpdateStep(float duration) {
	size_t numPhysicsObjects = m_PhysicsObjects.size();

	if (numPhysicsObjects == 0)
		return;

	for (int i = 0; i < numPhysicsObjects; i++) {
		/* Gravity */
		m_PhysicsObjects[i]->ApplyForce(Vector3(0, -0.981f, 0));
		//m_PhysicsObjects[i]->ApplyForce(Vector3(-0.981f, -0.981f, -0.981f));
	}

	for (int i = 0; i < m_Springs.size(); i++) {
		m_Springs[i]->ApplyForces(duration);
	}

	for (int i = 0; i < numPhysicsObjects; i++) {
		m_PhysicsObjects[i]->Integrate(duration);
	}

	// Detect collisions
	PhysicsObject *physObjA, *physObjB;

	iShape *shapeA, *shapeB;

	bool collision;

	for (int i = 0; i < numPhysicsObjects - 1; i++) {
		physObjA = m_PhysicsObjects[i];
		shapeA = physObjA->pShape;

		for (int j = i + 1; j < numPhysicsObjects; j++) {
			physObjB = m_PhysicsObjects[j];
			shapeB = physObjB->pShape;

			collision = CollisionTest(physObjA->position, shapeA, physObjB->position, shapeB);

			if (collision) {
				//collision = false;
				//if ((shapeA->GetType() == SHAPE_TYPE_SPHERE && shapeB->GetType() == SHAPE_TYPE_AABB) ||
				//	(shapeB->GetType() == SHAPE_TYPE_SPHERE && shapeA->GetType() == SHAPE_TYPE_AABB)) {
				//
				//	int ShepreHash;
				//	// Shpere >< AABB get Hash
				//	if (shapeA->GetType() == SHAPE_TYPE_SPHERE && shapeB->GetType() == SHAPE_TYPE_AABB) {
				//		ShepreHash = CalculateHashValuePhysics(physObjA->position);
				//	} else  if (shapeB->GetType() == SHAPE_TYPE_SPHERE && shapeA->GetType() == SHAPE_TYPE_AABB) {
				//		ShepreHash = CalculateHashValuePhysics(physObjB->position);
				//	}

				//	std::map<int, std::vector<Triangle*>>::iterator itAABBmap = m_AABBStructure.find(ShepreHash);
				//	if (itAABBmap != m_AABBStructure.end()) {
				//		std::vector<Triangle*> trianglesVec = itAABBmap->second;
				//		for (int i = 0; i < trianglesVec.size(); i++) {
				//			Vector3 currTriangleCenter;

				//			currTriangleCenter.x = (trianglesVec[i]->A.x + trianglesVec[i]->B.x + trianglesVec[i]->C.x) / 3;
				//			currTriangleCenter.y = (trianglesVec[i]->A.y + trianglesVec[i]->B.y + trianglesVec[i]->C.y) / 3;
				//			currTriangleCenter.z = (trianglesVec[i]->A.z + trianglesVec[i]->B.z + trianglesVec[i]->C.z) / 3;

				//			if (shapeA->GetType() == SHAPE_TYPE_SPHERE) {
				//				collision = CollisionTest(physObjA->position, shapeA, Vector3(0.0f), trianglesVec[i]);
				//				if (collision) break; 
				//				collision = CollisionTest(physObjA->position, shapeA, Vector3(0.0f), trianglesVec[i]);
				//				if (collision) break;
				//				collision = CollisionTest(physObjA->position, shapeA, Vector3(0.0f), trianglesVec[i]);
				//				if (collision) break;
				//			}
				//			else {
				//				collision = CollisionTest(physObjB->position, shapeB, Vector3(0.0f), trianglesVec[i]);
				//				if (collision) break;
				//				collision = CollisionTest(physObjB->position, shapeB, Vector3(0.0f), trianglesVec[i]);
				//				if (collision) break;
				//				collision = CollisionTest(physObjB->position, shapeB, Vector3(0.0f), trianglesVec[i]);
				//				if (collision) break;
				//			}

				//			if (collision)
				//				break;
				//		}

				//	}
				//}

				if (collision) {
					if (physObjA->m_IsStatic == false)
					{
						physObjA->position.y = physObjA->prevPosition.y;
						////physObjA->KillAllForces();
						physObjA->velocity.y = 0.0f;

						// Bounce:
						//physObjA->velocity = Vector3(0.0f) - physObjA->velocity * 0.8f;
					}

					if (physObjB->m_IsStatic == false)
					{
						physObjB->position.y = physObjB->prevPosition.y;
						//physObjB->KillAllForces();
						physObjB->velocity.y = 0.0f;

						//physObjB->velocity = Vector3(0.0f) - physObjB->velocity * 0.8f;
					}
				}
			}
		}
	}

	for (int i = 0; i < numPhysicsObjects; i++) {
		m_PhysicsObjects[i]->KillAllForces();
	}
}

void PhysicsSystem::AddTriangleToAABBCollisionCheck(int hash, Triangle* triangle)
{
	m_AABBStructure[hash].push_back(triangle);
}

bool PhysicsSystem::RayCast(Ray ray, PhysicsObject** hitObject)
{
	for (int i = 0; i < m_PhysicsObjects.size(); i++) {
		PhysicsObject* physicsObject = m_PhysicsObjects[i];
		if (physicsObject->pShape->GetType() == SHAPE_TYPE_SPHERE)
		{
			Sphere* pSphere = dynamic_cast<Sphere*>(physicsObject->pShape);
			if (TestRaySphere(ray.origin, ray.direction, pSphere->Center + physicsObject->position, pSphere->Radius))
			{
				*hitObject = physicsObject;
				return true;
			}
		}
	}
	return false;
}

bool PhysicsSystem::CollisionTest(const Vector3& posA, iShape* shapeA, const Vector3& posB, iShape* shapeB)
{
	bool detectedCollision = false;

	// ShapeA AABB Collision test 
	if (shapeA->GetType() == SHAPE_TYPE_AABB)
	{
		if (shapeB->GetType() == SHAPE_TYPE_AABB) {}

		else if (shapeB->GetType() == SHAPE_TYPE_SPHERE) {
			detectedCollision = CollisionTest(posB, dynamic_cast<Sphere*>(shapeB), posA, dynamic_cast<AABB*>(shapeA));
		}

		else if (shapeB->GetType() == SHAPE_TYPE_TRIANGLE) {}
	}


	// ShapeA Sphere collision tests
	else if (shapeA->GetType() == SHAPE_TYPE_SPHERE)
	{
		if (shapeB->GetType() == SHAPE_TYPE_AABB) {
			detectedCollision = CollisionTest(posA, dynamic_cast<Sphere*>(shapeA), posB, dynamic_cast<AABB*>(shapeB));
		}

		else if (shapeB->GetType() == SHAPE_TYPE_SPHERE) {
			detectedCollision = CollisionTest(posA, dynamic_cast<Sphere*>(shapeA), posB, dynamic_cast<Sphere*>(shapeB));
		}

		else if (shapeB->GetType() == SHAPE_TYPE_TRIANGLE) {
			detectedCollision = CollisionTest(posA, dynamic_cast<Sphere*>(shapeA), posB, dynamic_cast<Triangle*>(shapeB));
		}
	}


	// ShapeA Triangle collision tests
	else if (shapeA->GetType() == SHAPE_TYPE_TRIANGLE)
	{
		if (shapeB->GetType() == SHAPE_TYPE_AABB) {}

		else if (shapeB->GetType() == SHAPE_TYPE_SPHERE) {
			detectedCollision = CollisionTest(posB, dynamic_cast<Sphere*>(shapeB), posA, dynamic_cast<Triangle*>(shapeA));
		}

		else if (shapeB->GetType() == SHAPE_TYPE_TRIANGLE) {}
	}


	// ShapeA is unknown shape...
	else
	{
		// what is this!?!?!
	}

	return detectedCollision;
}

bool PhysicsSystem::CollisionTest(const Vector3& posA, Sphere* a, const Vector3& posB, Sphere* b)
{
	return TestSphereSphere(posA + a->Center, a->Radius, posB + b->Center, b->Radius);
}

bool PhysicsSystem::CollisionTest(const Vector3 &posA, Sphere* a, const Vector3& posB, AABB* b)
{
	return TestSphereAABB(posA + a->Center, a->Radius, *b);
}

bool PhysicsSystem::CollisionTest(const Vector3 &posA, Sphere* a, const Vector3& posB, Triangle* b)
{
	Point unused;
	return TestSphereTriangle(posA + a->Center, a->Radius, posB + (*b).A, posB + (*b).B, posB + (*b).C, unused);
}