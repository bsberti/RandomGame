#pragma once

#include <vector>
#include "cMeshObject.h"


class GraphicScene {
public:
	GraphicScene();
	~GraphicScene();

	void Initialize();

	void CreateGameObjectByType(const std::string& type, glm::vec3 position);

	cMeshObject selectedObject;
	std::vector< cMeshObject* > vec_pMeshObjects;

private:
	
};