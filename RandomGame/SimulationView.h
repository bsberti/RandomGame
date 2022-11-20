#pragma once

#include <map>
#include <vector>
#include <string>

#include "PhysicsDebugRenderer.h"
#include "PhysicsSystem.h"
#include "BoundingBox.h"
#include "Ball.h"
#include "cVAOManager/sModelDrawInfo.h"

typedef glm::vec3 color;

//struct GameObjectData {
//	unsigned int ModelId;
//	unsigned int MaterialId;
//};

class SimulationView {
public:
	SimulationView();
	~SimulationView();

	void Initialize();
	void Destroy();

	void Update(double dt);
	void PhysicsUpdate(double dt);
	void Render();
	void LoadStaticModelToOurAABBEnvironment(sModelDrawInfo model, glm::vec3 position);
	Ball* CreateBall(glm::vec3 position, float scale);

	PhysicsSystem m_PhysicsSystem;
private:
	void PrepareDemo();

	//void CreateTree(const Vector3& position, float scale);
	//void AddGameDataToMap(GameObjectData& data);
	//GameObject* CreateObjectByType(const std::string& type);

	//std::map<std::string, GameObjectData> m_GameObjectDataMap;

	Ball m_ControlledBall;

	BoundingBox m_BallBoundingBox;

	std::vector<Ball> m_Balls;

	unsigned int m_ShipModelId;
	PhysicsDebugRenderer* m_PhysicsDebugRenderer;
};
