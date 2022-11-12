#include "GraphicScene.h"
#include "cRobot.h"
#include <iostream>

GraphicScene::GraphicScene() {

}

GraphicScene::~GraphicScene() {
}

void GraphicScene::Initialize() {

}

bool GraphicScene::checkLOS(cRobot* pMeWhosAsking) {
	bool LOScheck = true;

	cRobot* closestRobot;
	closestRobot = (cRobot*)robotShepard.getRobotFromIndex(pMeWhosAsking->getClosestRobotID());
	
	float dt = 2.0f;

	glm::vec3 weaponInicialPosition = pMeWhosAsking->currentWeapon->position;

	glm::vec3 TargetToRobotVector = closestRobot->position - pMeWhosAsking->currentWeapon->position;
	glm::vec3 DirectionToTarget = glm::normalize(TargetToRobotVector);
	glm::vec3 MovementPerStep = DirectionToTarget * dt;

	//float distanceTerrain = 0;

	float distanceWeaponTarget = 0;

	do {
		distanceWeaponTarget = glm::distance(pMeWhosAsking->currentWeapon->position, closestRobot->position);
		int breakpoint = 5;
		std::cout << "Distance between Robot " << pMeWhosAsking->getID() << " and Robot " << closestRobot->getID() << " is " <<
			distanceWeaponTarget << std::endl;

		float minDistance = 1000.f;
		int triangleMinDistance = 0;
		for (int j = 0; j < trianglesCenter.size(); j++) {
			float currentDistance = glm::distance(trianglesCenter[j], pMeWhosAsking->currentWeapon->position);
			if (minDistance > currentDistance) {
				minDistance = currentDistance;
				triangleMinDistance = j;
			}
		}

		if (pMeWhosAsking->currentWeapon->position.y < trianglesCenter[triangleMinDistance].y) {
			std::cout << "Weapon Y: " << pMeWhosAsking->currentWeapon->position.y << std::endl;
			std::cout << "Triangle Y: " << trianglesCenter[triangleMinDistance].y << std::endl;
			LOScheck = false;
			break;
		}

		//for (int j = 0; j < trianglesCenter.size(); j++) {
		//	//distanceTerrain = glm::distance(pMeWhosAsking->currentWeapon->position, trianglesCenter[j]);
		//	//if (distanceTerrain < 0.01f) {
		//	if (pMeWhosAsking->currentWeapon->position.y < trianglesCenter[j].y) {

		//		std::cout << "Weapon Y: " << pMeWhosAsking->currentWeapon->position.y << std::endl;
		//		std::cout << "Triangle Y: " << trianglesCenter[j].y << std::endl;
		//		LOScheck = false;
		//		break;
		//	}
		//}

		pMeWhosAsking->currentWeapon->position += MovementPerStep;
	} while (distanceWeaponTarget > 2.0f);

	pMeWhosAsking->currentWeapon->position = weaponInicialPosition;

	return LOScheck;
}

void GraphicScene::CreateGameObjectByType(const std::string& type, glm::vec3 position, sModelDrawInfo& drawInfo) {
	if (type.compare("Bunny") == 0) {
		cRobot* go = (cRobot*)robotShepard.makeRobot();;
		go->meshName = type;
		go->position = position;
		go->scale = 40.0f;
		go->numberOfTriangles = drawInfo.numberOfTriangles;
		go->meshTriangles = drawInfo.modelTriangles;
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
		go->numberOfTriangles = drawInfo.numberOfTriangles;
		go->meshTriangles = drawInfo.modelTriangles;
		vec_pMeshObjects.push_back(go);
	}	
	else {
		cMeshObject* go = new cMeshObject();
		go->meshName = type;
		go->friendlyName = type;
		go->position = position;
		go->bUse_RGBA_colour = false;
		go->isWireframe = false;
		go->numberOfTriangles = drawInfo.numberOfTriangles;
		go->meshTriangles = drawInfo.modelTriangles;
		vec_pMeshObjects.push_back(go);
	}
}
