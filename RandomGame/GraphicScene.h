#pragma once

#include <vector>
#include <map>
#include "cMeshObject.h"
#include "cVAOManager/sModelDrawInfo.h"

class GraphicScene {
public:
	friend class cRandomUI;
	GraphicScene();
	~GraphicScene();

	void Initialize();

	void PositioningMe(cMeshObject* pMeWhoIsAsking);

	void CreateGameObjectByType(const std::string& type, glm::vec3 position, sModelDrawInfo& drawInfo);

	cMeshObject* GetObjectByName(std::string name, bool bSearchChildren);

	cMeshObject selectedObject;
	std::vector<cMeshObject*> vec_torchFlames;
	std::vector<cMeshObject*> vec_pMeshObjects;
	std::map<std::string, cMeshObject*>* map_beholds;

	std::vector<glm::vec3> trianglesCenter;
private:
	
};