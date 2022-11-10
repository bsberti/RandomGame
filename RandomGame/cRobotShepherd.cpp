#include "cRobotShepherd.h"
#include <glm/glm.hpp>
#include "cRobot.h"	// This is the ACTUAL robot, not interface
#include <iostream>

cRobotShepherd::cRobotShepherd() {
	
}

iRobot* cRobotShepherd::makeRobot(void) {
	cRobot* pNewRobot = new cRobot();

	// Tell the robot about the shepherd
	static_cast<cRobot*>(pNewRobot)->SetRobotShepherd(this);

	// Add the robot to the list of robots
	this->m_vecTheRobots.push_back(pNewRobot);

	return pNewRobot;
}

iRobot* cRobotShepherd::getRobotFromIndex(int index) {
	return (cRobot*)m_vecTheRobots[index];
}

int cRobotShepherd::getRobotNumber() {
	return this->m_vecTheRobots.size();
}

bool cRobotShepherd::checkLOS(cRobot* robotA, cRobot* robotB) {
	bool LOScheck = true;

	float dt = 2.0f;

	glm::vec3 weaponInicialPosition = robotA->currentWeapon->position;

	glm::vec3 TargetToRobotVector = robotB->position - robotA->currentWeapon->position;
	glm::vec3 DirectionToTarget = glm::normalize(TargetToRobotVector);
	glm::vec3 MovementPerStep = DirectionToTarget * dt;
	//float distanceTerrain = 0;

	float distanceWeaponTarget = 0;

	do {
		distanceWeaponTarget = glm::distance(robotA->currentWeapon->position, robotB->position);
		int breakpoint = 5;
		std::cout << "Distance between Robot " << robotA->getID() << " and Robot " << robotB->getID() << " is " <<
			distanceWeaponTarget << std::endl;

		float minDistance = 1000.f;
		int triangleMinDistance = 0;
		for (int j = 0; j < terrain->trianglesCenter.size(); j++) {
			float currentDistance = glm::distance(terrain->trianglesCenter[j], robotA->currentWeapon->position);
			if (minDistance > currentDistance) {
				minDistance = currentDistance;
				triangleMinDistance = j;
			}
		}

		if (robotA->currentWeapon->position.y < terrain->trianglesCenter[triangleMinDistance].y) {
			std::cout << "Weapon Y: " << robotA->currentWeapon->position.y << std::endl;
			std::cout << "Triangle Y: " << terrain->trianglesCenter[triangleMinDistance].y << std::endl;
			LOScheck = false;
			break;
		}

		robotA->currentWeapon->position += MovementPerStep;
	} while (distanceWeaponTarget > 2.0f);

	robotA->currentWeapon->position = weaponInicialPosition;

	return LOScheck;
}

void cRobotShepherd::SetClosestRobot(cRobot* pMeWhosAsking, std::vector<int> outIndexes) {
	// Code to find closest robot
	// I'm lazy so I'm going to just pick a random robot
	float distanceBetween = 0;
	float minDistance = 1000;
	
	for (int i = 0; i < m_vecTheRobots.size(); i++) {
		bool indexTested = false;
		for (int j = 0; j < outIndexes.size(); j++) {
			if (i == outIndexes[j])
				indexTested = true;
		}
		
		if (!indexTested) {
			distanceBetween = glm::distance(pMeWhosAsking->position, m_vecTheRobots[i]->position);
			if (minDistance > distanceBetween && distanceBetween > 0.f) {
				minDistance = distanceBetween;
				pMeWhosAsking->setClosestRobotID(i);
			}
		}
	}
	//pMeWhosAsking.pClosestRobot = (cRobot*)this->m_vecTheRobots[rand() % 8];

}

//iDamage* cRobotShepherd::findClosestRobot(cRobot* pMeWhosAsking) {
//	
//}

void cRobotShepherd::Update(double deltaTime) {
	for (unsigned int index = 0; index != this->m_vecTheRobots.size();
		index++)
	{
		this->m_vecTheRobots[index]->Update(deltaTime);
	}

	return;
}

// Return true if I actually hit something... maybe
bool cRobotShepherd::ShootTheClosestRobot(iRobot* pMeWhosAsking, float amount) {
	// Code to find closest robot
	// I'm lazy so I'm going to just pick a random robot
	
	//cRobot* pClosestRobot = (cRobot*)this->m_vecTheRobots[rand() % 8];
	cRobot* pClosestRobot = (cRobot*)getRobotFromIndex(pMeWhosAsking->getClosestRobotID());

	pClosestRobot->TakeDamage(amount);

	return true;
}
