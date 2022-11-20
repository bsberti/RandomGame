#include "SimulationView.h"

#include <limits>
#include <math.h>
#include <map>
#include <vector>
#include <glm/gtx/projection.hpp>

unsigned int g_SphereModelId;
unsigned int g_SphereMaterialId;
unsigned int g_TreeMaterialId;
unsigned int g_FirTreeModelId;
unsigned int g_SphereTextureId;

const float SHIP_SCALE = 0.5f;

Ball* g_Ball;

std::map<int, cMeshObject*> g_PartialMeshObjects;
std::map<int, cMeshObject*>::iterator g_PartialMeshObjectsCursor;

SimulationView::SimulationView(){

}

SimulationView::~SimulationView(){
	delete m_PhysicsDebugRenderer;
}

void SimulationView::Initialize(){
	
}

void SimulationView::Destroy() {

}

int CalculateHashValue(float x, float y, float z)
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

int CalculateHashValue(const Vector3& v)
{
	return CalculateHashValue(v.x, v.y, v.z);
}

void SimulationView::LoadStaticModelToOurAABBEnvironment(sModelDrawInfo model, glm::vec3 position) {
	std::vector<glm::vec3> vertices;
	std::vector<int> triangles;

	// Load Models
	//float min[3] = { 0.f, 2.3528f, 0.f };
	//float max[3] = { 256.f, 51.4f, 256.f };
	float min[3] = { 0.f, 0.f, 0.f };
	float max[3] = { 256.f, 1.f, 256.f };
	AABB* groundAABB = new AABB(min, max);

	PhysicsObject* physicsGround = m_PhysicsSystem.CreatePhysicsObject(Vector3(0, 0, 0), groundAABB);
	physicsGround->pShape = groundAABB;
	physicsGround->SetMass(-1.f);
	
	vertices.resize(model.numberOfIndices);
	for (int i = 0; i < model.numberOfIndices; i++) {
		vertices[i].x = model.pVertices[model.pIndices[i]].x;
		vertices[i].y = model.pVertices[model.pIndices[i]].y;
		vertices[i].z = model.pVertices[model.pIndices[i]].z;
	}

	for (int i = 0; i < model.numberOfTriangles; i+= 3) {
		Point a = Point(vertices[i] + position);
		Point b = Point(vertices[i + 1] + position);
		Point c = Point(vertices[i + 2] + position);

		int hashA = CalculateHashValue(a);
		int hashB = CalculateHashValue(b);
		int hashC = CalculateHashValue(c);

		//printf("(%.2f, %.2f, %.2f) -> %d\n", a.x, a.y, a.z, hashA);
		//printf("(%.2f, %.2f, %.2f) -> %d\n", b.x, b.y, b.z, hashB);
		//printf("(%.2f, %.2f, %.2f) -> %d\n", c.x, c.y, c.z, hashC);

		Triangle* newTriangle = new Triangle(a, b, c);

		m_PhysicsSystem.AddTriangleToAABBCollisionCheck(hashA, newTriangle);

		if (hashA != hashB)
			m_PhysicsSystem.AddTriangleToAABBCollisionCheck(hashB, newTriangle);

		if (hashC != hashB && hashC != hashA)
			m_PhysicsSystem.AddTriangleToAABBCollisionCheck(hashC, newTriangle);
	}
}

Ball* SimulationView::CreateBall(glm::vec3 position, float scale) {
	Sphere* otherSphere = new Sphere(Point(0.0f, 0.0f, 0.0f), scale);

	Ball* newBall = new Ball();
	newBall->physicsObject = m_PhysicsSystem.CreatePhysicsObject(position, otherSphere);
	newBall->gameObject = new cMeshObject();
	newBall->gameObject->meshName = "ISO_Sphere_1";
	newBall->gameObject->friendlyName = "Ball";
	newBall->gameObject->position = position;
	//newBall->gameObject->Renderer.ShaderId = 1;
	//newBall->gameObject->Renderer.MaterialId = g_SphereMaterialId;
	//newBall->gameObject->Renderer.MeshId = g_SphereModelId;
	newBall->gameObject->scale = scale;

	// Create a bounding box around our ball.
	//Vector3 halfExtents = m_BallBoundingBox.halfExtents;
	//newBall->physicsObject->pBoundingBox = &m_BallBoundingBox;
	//newBall->physicsObject->pBoundingBox->halfExtents = halfExtents * scale;

	//m_PhysicsDebugRenderer->AddPhysicsObject(newBall.physicsObject);

	return newBall;
}

void SimulationView::PrepareDemo() {

	//std::vector<glm::vec3> vertices;
	//std::vector<int> triangles;

	//unsigned int unused1, unused2;

	//GDP_GetModelData(g_SphereModelId, vertices, triangles, unused1, unused2);
	//Vector3 minPoints = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	//Vector3 maxPoints = Vector3(FLT_MIN, FLT_MIN, FLT_MIN);
	//for (int i = 0; i < vertices.size(); i++) {
	//	glm::vec3& vertex = vertices[i];

	//	if (minPoints.x > vertex.x)
	//		minPoints.x = vertex.x;
	//	if (minPoints.y > vertex.y)
	//		minPoints.y = vertex.y;
	//	if (minPoints.z > vertex.z)
	//		minPoints.z = vertex.z;

	//	if (maxPoints.x < vertex.x)
	//		maxPoints.x = vertex.x;
	//	if (maxPoints.y < vertex.y)
	//		maxPoints.y = vertex.y;
	//	if (maxPoints.z < vertex.z)
	//		maxPoints.z = vertex.z;
	//}

	//// Calculate the point halfway between the minPoints, and maxPoints
	//Vector3 halfExtents = (maxPoints - minPoints) / 2.f;
	//Vector3 centerPoint = minPoints + halfExtents;

	//printf("Minimum Points: (%.2f, %.2f, %.2f)\n",
	//	minPoints.x,
	//	minPoints.y,
	//	minPoints.z);
	//printf("Maximum Points: (%.2f, %.2f, %.2f)\n",
	//	maxPoints.x,
	//	maxPoints.y,
	//	maxPoints.z);
	//printf("Half Extents: (%.2f, %.2f, %.2f)\n",
	//	halfExtents.x,
	//	halfExtents.y,
	//	halfExtents.z);
	//printf("Center Point: (%.2f, %.2f, %.2f)\n",
	//	centerPoint.x,
	//	centerPoint.y,
	//	centerPoint.z);

	//m_BallBoundingBox.centerPoint = centerPoint;
	//m_BallBoundingBox.halfExtents = halfExtents;
	//m_BallBoundingBox.maxPoints = maxPoints;
	//m_BallBoundingBox.minPoints = minPoints;


	Sphere* controlledSphere = new Sphere(Point(0.0f, 0.0f, 0.0f), 1.0f);

	//// Create our controlled ball
	m_ControlledBall.physicsObject = m_PhysicsSystem.CreatePhysicsObject(Vector3(1, 25, 0), controlledSphere);
	m_ControlledBall.gameObject = new cMeshObject();
	//m_ControlledBall.gameObject->Renderer.ShaderId = 1;
	//m_ControlledBall.gameObject->Renderer.MaterialId = g_SphereMaterialId;
	//m_ControlledBall.gameObject->Renderer.MeshId = g_SphereModelId;
	//m_ControlledBall.gameObject->Scale = glm::vec3(1, 1, 1);

	//// Create a bounding box around our ball.
	//m_ControlledBall.physicsObject->pBoundingBox = &m_BallBoundingBox;

	//m_PhysicsDebugRenderer->AddPhysicsObject(m_ControlledBall.physicsObject);

	//CreateTree(Vector3(0.0f), 0.025f);
	//CreateTree(Vector3(-10.0f, 0.0f, -10.0f), 0.025f);
	//CreateTree(Vector3(-10.0f, 0.0f,  10.0f), 0.025f);
	//CreateTree(Vector3( 10.0f, 0.0f, -10.0f), 0.025f);
	//CreateTree(Vector3( 10.0f, 0.0f,  10.0f), 0.025f);

	//for (int i = -20; i < 20; i+=3) {
	//	for (int j = -20; j < 20; j+=3) {
	//		CreateBall(Vector3(i, 20, j), 0.1f);
	//	}
	//}	
}

bool isClicked = false;
void SimulationView::Update(double dt) {
	int state = 0;
	


	//// Typically moved to a UserInput Section
	//if (g_Ball)
	//{
	//	if (GDP_IsKeyHeldDown('a'))
	//		g_Ball->gameObject->Position += glm::vec3(25.0f, 0.0f, 0.0f) * (float)dt;
	//	if (GDP_IsKeyHeldDown('d'))
	//		g_Ball->gameObject->Position += glm::vec3(-25.0f, 0.0f, 0.0f) * (float)dt;
	//	if (GDP_IsKeyHeldDown('w'))
	//		g_Ball->gameObject->Position += glm::vec3(0.0f, 0.0f, 25.0f) * (float)dt;
	//	if (GDP_IsKeyHeldDown('s'))
	//		g_Ball->gameObject->Position += glm::vec3(0.0f, 0.0f, -25.0f) * (float)dt;
	//	if (GDP_IsKeyHeldDown('e'))
	//		g_Ball->gameObject->Position += glm::vec3(0.0f, 25.0f, 0.0f) * (float)dt;
	//	if (GDP_IsKeyHeldDown('q'))
	//		g_Ball->gameObject->Position += glm::vec3(0.0f, -25.0f, 0.0f) * (float)dt;

	//	for (auto meshObjectIt = g_PartialMeshObjects.begin();
	//		meshObjectIt != g_PartialMeshObjects.end();
	//		meshObjectIt++)
	//	{
	//		meshObjectIt->second->Enabled = false;
	//	}

	//	int hashValue = CalculateHashValue(g_Ball->gameObject->Position);
	//	printf("%d\n", hashValue);


	//	// THIS will insert a pair if it does not exist
	//	// Which is what we do not want.
	//	// g_PartialMeshObjects[hashValue]->Enabled = true;

	//	auto resultIt = g_PartialMeshObjects.find(hashValue);
	//	if (resultIt != g_PartialMeshObjects.end())
	//	{
	//		if (resultIt->second != nullptr)
	//			resultIt->second->Enabled = true;
	//	}
	//}


	//// Update the Visual object from the Physics object
	//if (m_ControlledBall.physicsObject != nullptr)
	//{
	//	Vector3 p = m_ControlledBall.physicsObject->GetPosition();
	//	m_ControlledBall.gameObject->Position = glm::vec3(p.x, p.y, p.z);
	//}

	//for (int i = 0; i < m_Balls.size(); i++) {
	//	Vector3 p = m_Balls[i].physicsObject->GetPosition();
	//	m_Balls[i].gameObject->Position = glm::vec3(p.x, p.y, p.z);
	//}

	//for (int i = 0; i < m_Balls.size(); i++) {
	//	Vector3 p = m_Balls[i].physicsObject->GetPosition();
	//	m_Balls[i].gameObject->Position = glm::vec3(p.x, p.y, p.z);
	//}

}

void SimulationView::PhysicsUpdate(double dt)
{
	// System/Engine update step
	m_PhysicsSystem.UpdateStep(dt);

}

void SimulationView::Render() {
	m_PhysicsDebugRenderer->Render();
}

//void SimulationView::AddGameDataToMap(GameObjectData& data) {
//}
//
//GameObject* SimulationView::CreateObjectByType(const std::string& type){
//	return nullptr;
//}