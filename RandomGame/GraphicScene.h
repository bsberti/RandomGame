#pragma once

#include <vector>
#include "cMeshObject.h"
#include "cVAOManager/sModelDrawInfo.h"

class GraphicScene {
public:
	friend class cRandomUI;
	GraphicScene();
	~GraphicScene();

	void Initialize();

	void CreateGameObjectByType(const std::string& type, glm::vec3 position, sModelDrawInfo& drawInfo);

	cMeshObject selectedObject;
	std::vector< cMeshObject* > vec_pMeshObjects;

	std::vector<glm::vec3> trianglesCenter;

	bool pirateShow;
	bool pirateShow2;

private:
	
};