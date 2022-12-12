#include "GraphicScene.h"
#include <iostream>

GraphicScene::GraphicScene() {

}

GraphicScene::~GraphicScene() {
}

void GraphicScene::Initialize() {

}

// From here: https://stackoverflow.com/questions/5289613/generate-random-float-between-two-floats/5289624
float RandomFloatGraphic(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

void GraphicScene::PositioningMe(cMeshObject* pMeWhoIsAsking) {
	cMeshObject* currObj = pMeWhoIsAsking;

	// Set a new X and Z
	//currObj->position = glm::vec3(RandomFloatGraphic(-128, 128), -46.5f, RandomFloatGraphic(-128, 128));

	float minDistance = 1000.f;
	int triangleMinDistance = 0;
	for (int j = 0; j < trianglesCenter.size(); j++) {
		float currentDistance = glm::distance(trianglesCenter[j], currObj->position);
		if (minDistance > currentDistance) {
			minDistance = currentDistance;
			triangleMinDistance = j;
		}
	}

	currObj->position = trianglesCenter[triangleMinDistance];
}

void GraphicScene::CreateGameObjectByType(const std::string& type, glm::vec3 position, sModelDrawInfo& drawInfo) {
	cMeshObject* go = new cMeshObject();
	go->meshName = type;
	go->friendlyName = type;
	go->position = position;
	go->bUse_RGBA_colour = true;
	go->RGBA_colour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f); 
	//go->specular_colour_and_power = glm::vec4(1.0f, 1.0f, 1.0f, 1000.0f);
	go->isWireframe = false;
	go->soundPlayed = false;
	go->numberOfTriangles = drawInfo.numberOfTriangles;
	go->meshTriangles = drawInfo.modelTriangles;

	vec_pMeshObjects.push_back(go);
}
