#pragma once

#include <vector>
#include "cMeshObject.h"


class GraphicScene {
public:
	GraphicScene();
	~GraphicScene();

	void Initialize();

	void CreateGameObjectByType(const std::string& type);

	std::vector< cMeshObject* > vec_pMeshObjects;

private:
	
};