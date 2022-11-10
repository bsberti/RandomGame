#pragma once

#include <vector>
#include "cMeshObject.h"
#include "cRobotShepherd.h"
#include "cVAOManager/sModelDrawInfo.h"
#include "cRobot.h"

class GraphicScene {
public:
	friend class cRandomUI;
	GraphicScene();
	~GraphicScene();

	void Initialize();

	void CreateGameObjectByType(const std::string& type, glm::vec3 position, sModelDrawInfo& drawInfo);

	bool checkLOS(cRobot* pMeWhosAsking);

	cMeshObject selectedObject;
	std::vector< cMeshObject* > vec_pMeshObjects;
	cRobotShepherd robotShepard;

	std::vector<glm::vec3> trianglesCenter;
private:
	
};