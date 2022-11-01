#include "GraphicScene.h"

GraphicScene::GraphicScene() {
}

GraphicScene::~GraphicScene() {
}

void GraphicScene::Initialize() {

}

void GraphicScene::CreateGameObjectByType(const std::string& type) {
	if (type.compare("Bunny") == 0) {
		cMeshObject* go = new cMeshObject();
		go->meshName = type;
		go->position = glm::vec3(2.0f, -6.0f, 0.0f);
		go->scale = 20.0f;
		vec_pMeshObjects.push_back(go);
	}
	else {
		cMeshObject* go = new cMeshObject();
		go->meshName = type;
		go->friendlyName = type;
		go->bUse_RGBA_colour = true;
		go->isWireframe = false;
		vec_pMeshObjects.push_back(go);
	}
}
