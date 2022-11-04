#include "GraphicScene.h"

GraphicScene::GraphicScene() {
}

GraphicScene::~GraphicScene() {
}

void GraphicScene::Initialize() {

}

void GraphicScene::CreateGameObjectByType(const std::string& type, glm::vec3 position) {
	if (type.compare("Bunny") == 0) {
		cMeshObject* go = new cMeshObject();
		go->meshName = type;
		go->position = position;
		go->scale = 20.0f;
		vec_pMeshObjects.push_back(go);
	}
	else if (type.compare("Cabin") == 0) {
		cMeshObject* go = new cMeshObject();
		go->meshName = type;
		go->friendlyName = type;
		go->position = position;
		go->bUse_RGBA_colour = false;
		go->scale = 0.05f;
		go->isWireframe = false;
		vec_pMeshObjects.push_back(go);
	}	
	else {
		cMeshObject* go = new cMeshObject();
		go->meshName = type;
		go->friendlyName = type;
		go->position = position;
		go->bUse_RGBA_colour = false;
		go->isWireframe = false;
		vec_pMeshObjects.push_back(go);
	}
}
