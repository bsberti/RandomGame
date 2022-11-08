#pragma once

#include <vector>
#include "cMeshObject.h"
#include "cRobotShepherd.h"


#include "cVAOManager/sModelDrawInfo.h"

class GraphicScene {
public:
	GraphicScene();
	~GraphicScene();

	void Initialize();

	void CreateGameObjectByType(const std::string& type, glm::vec3 position, sModelDrawInfo& drawInfo);

	cMeshObject selectedObject;
	std::vector< cMeshObject* > vec_pMeshObjects;
	cRobotShepherd robotShepard;

	std::vector<glm::vec3> trianglesCenter;
private:
	
};