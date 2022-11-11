#include "GraphicScene.h"
#include <iostream>

GraphicScene::GraphicScene() {
	pirateShow = false;
	pirateShow2 = false;
}

GraphicScene::~GraphicScene() {
}

void GraphicScene::Initialize() {

}

void GraphicScene::CreateGameObjectByType(const std::string& type, glm::vec3 position, sModelDrawInfo& drawInfo) {
	cMeshObject* go = new cMeshObject();
	go->meshName = type;
	go->friendlyName = type;
	go->position = position;
	go->bUse_RGBA_colour = false;
	go->isWireframe = false;
	go->numberOfTriangles = drawInfo.numberOfTriangles;
	go->meshTriangles = drawInfo.modelTriangles;

	if (type == "Lighthouse") {
		go->scale = 16.0f;
		go->rotation.y = 4;
	}

	if (type == "Old_House")
		go->scale = 18.964f;

	if (type == "Palm")
		go->scale = 0.014f;

	if (type == "Sky_Pirate") {
		go->scale = 3.529f;
		go->rotation.y = 0.356f;
	}

	if (type == "Factory") {
		go->scale = 25.0f;
		go->rotation.y = -2.331;
	}

	if (type == "Tree1")
		go->scale = 0.232f;

	if (type == "Tree2")
		go->scale = 10.282f;

	if (type == "Trees")
		go->scale = 1.793f;

	if (type == "Wood_House")
		go->scale = 5.727f;
	
	vec_pMeshObjects.push_back(go);
}
