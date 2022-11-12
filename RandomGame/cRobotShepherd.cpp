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

// From here: https://stackoverflow.com/questions/5289613/generate-random-float-between-two-floats/5289624
float RandomFloatSheppard(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

void cRobotShepherd::TeleportMe(iRobot* pMeWhoIsAsking) {
	cRobot* currentRobot = (cRobot*)pMeWhoIsAsking;

	// Set a new X and Z
		currentRobot->position = glm::vec3(RandomFloatSheppard(-128, 128), -46.5f, RandomFloatSheppard(-128, 128));

	float minDistance = 1000.f;
	int triangleMinDistance = 0;
	for (int j = 0; j < terrain->trianglesCenter.size(); j++) {
		float currentDistance = glm::distance(terrain->trianglesCenter[j], currentRobot->position);
		if (minDistance > currentDistance) {
			minDistance = currentDistance;
			triangleMinDistance = j;
		}
	}

	currentRobot->position = terrain->trianglesCenter[triangleMinDistance];

	currentRobot->currentWeapon->position = currentRobot->position;
	currentRobot->currentWeapon->position.y += 10;

	// Particle position = Object Position
	currentRobot->currentWeapon->pPosition.x = currentRobot->currentWeapon->position.x;
	currentRobot->currentWeapon->pPosition.y = currentRobot->currentWeapon->position.y;
	currentRobot->currentWeapon->pPosition.z = currentRobot->currentWeapon->position.z;

	SetClosestRobot(currentRobot);
	//testedIndexes.push_back(currentRobot->getClosestRobotID());
}

bool cRobotShepherd::checkLOS(iRobot* robotA, iRobot* robotB) {
	bool LOScheck = true;

	float dt = 2.0f;
	cRobot* A = (cRobot*)robotA;
	cRobot* B = (cRobot*)robotB;
	glm::vec3 weaponInicialPosition = A->currentWeapon->position;

	glm::vec3 TargetToRobotVector = B->position - A->currentWeapon->position;
	glm::vec3 DirectionToTarget = glm::normalize(TargetToRobotVector);
	glm::vec3 MovementPerStep = DirectionToTarget * dt;
	//float distanceTerrain = 0;

	float distanceWeaponTarget = 0;

	do {
		distanceWeaponTarget = glm::distance(A->currentWeapon->position, B->position);
		int breakpoint = 5;
		/*std::cout << "Distance between Robot " << robotA->getID() << " and Robot " << robotB->getID() << " is " <<
			distanceWeaponTarget << std::endl;*/

		float minDistance = 1000.f;
		int triangleMinDistance = 0;
		for (int j = 0; j < terrain->trianglesCenter.size(); j++) {
			float currentDistance = glm::distance(terrain->trianglesCenter[j], A->currentWeapon->position);
			if (minDistance > currentDistance) {
				minDistance = currentDistance;
				triangleMinDistance = j;
			}
		}

		if (A->currentWeapon->position.y < terrain->trianglesCenter[triangleMinDistance].y) {
			/*std::cout << "Weapon Y: " << A->currentWeapon->position.y << std::endl;
			std::cout << "Triangle Y: " << terrain->trianglesCenter[triangleMinDistance].y << std::endl;*/
			LOScheck = false;
			break;
		}

		A->currentWeapon->position += MovementPerStep;
	} while (distanceWeaponTarget > 2.0f);

	A->currentWeapon->position = weaponInicialPosition;

	return LOScheck;
}

void cRobotShepherd::SetClosestRobot(cRobot* pMeWhosAsking) {
	// Code to find closest robot
	// I'm lazy so I'm going to just pick a random robot
	float distanceBetween = 0;
	float minDistance = 1000;

	std::vector<int> outIndexes = pMeWhosAsking->testedIndexes;
	
	for (int i = 0; i < m_vecTheRobots.size(); i++) {
		bool indexTested = false;
		int currentIndex = 0;
		cRobot* currentRobotChecking = m_vecTheRobots[i];
		for (int j = 0; j < outIndexes.size(); j++) {
			currentIndex = m_vecTheRobots[i]->getID();
			if (currentIndex == outIndexes[j])
				indexTested = true;
		}
		
		if (!indexTested) {
			distanceBetween = glm::distance(pMeWhosAsking->position, currentRobotChecking->position);
			if (minDistance > distanceBetween && distanceBetween > 0.f) {
				minDistance = distanceBetween;
				pMeWhosAsking->setClosestRobotID(i);
			}
		}
	}

	//cRobot* whosAskingClosestRobot = (cRobot*)getRobotFromIndex(pMeWhosAsking->getClosestRobotID());
	//pMeWhosAsking->addToTestedIndexes(whosAskingClosestRobot->getID());
	//pMeWhosAsking.pClosestRobot = (cRobot*)this->m_vecTheRobots[rand() % 8];
}

void cRobotShepherd::Update(double deltaTime) {
	for (unsigned int index = 0; index != this->m_vecTheRobots.size();
		index++)
	{
		if (!this->m_vecTheRobots[index]->amIDead()) {
			this->m_vecTheRobots[index]->Update(deltaTime);
		}
		else {
			std::cout << "Robot " << this->m_vecTheRobots[index]->getID() << " is DEAD! ;(" << std::endl;
			this->m_vecTheRobots[index]->position.y = -1000;
			this->m_vecTheRobots[index]->currentWeapon->position.y = -1000;
			//this->m_vecTheRobots.erase(this->m_vecTheRobots.begin() + index);
		}
	}

	return;
}

// Return true if I actually hit something... maybe
bool cRobotShepherd::ShootTheClosestRobot(iRobot* pMeWhosAsking, float amount) {
	cRobot* robotShooting = (cRobot*)pMeWhosAsking;
	cRobot* robotToShoot = (cRobot*)getRobotFromIndex(pMeWhosAsking->getClosestRobotID());
	char lastIndexName = robotShooting->currentWeapon->friendlyName[robotShooting->currentWeapon->friendlyName.length() - 1];

	if (robotToShoot->amIDead()) {
		return false;
	}

	if (lastIndexName == '1') {
		// Check Weapon CD
		if (robotShooting->currentWeapon->GetWeaponCurrCD() > 0) {
			// LOS Shot! - LASER
			// Check first if the Robot is "Hittable"
			if (robotShooting->haveLOS) {
				glm::vec3 TargetToRobotVector = robotToShoot->position - robotShooting->currentWeapon->position;
				glm::vec3 DirectionToTarget = glm::normalize(TargetToRobotVector);

				Vector3 direction =
					Vector3(DirectionToTarget.x * 2, DirectionToTarget.y * 2, DirectionToTarget.z * 2);

				// ApplyForce as a Laser
				// No Gravity
				//robotShooting->currentWeapon->ApplyForce(direction);

				// Just taking damage for this project
				robotToShoot->TakeDamage(amount);
				robotShooting->currentWeapon->SetWaponCD(-1 * robotShooting->currentWeapon->GetWeaponCurrCD());
			}
			else {
				SetClosestRobot(robotShooting);
				return false;
			}
		}
		else {
			std::cout << "Robot " << robotShooting->getID() << " recharding . . . " << std::endl;
		}
	}
	else if (lastIndexName == '2') {
		if (robotShooting->currentWeapon->GetWeaponCurrCD() > 0) {
			//BALLISTIC Shot! - BOMB
			glm::vec3 TargetToRobotVector = robotToShoot->position - robotShooting->currentWeapon->position;
			glm::vec3 DirectionToTarget = glm::normalize(TargetToRobotVector);

			Vector3 direction =
				Vector3(DirectionToTarget.x, DirectionToTarget.y * 3, DirectionToTarget.z);

			Vector3 gravity = Vector3(0.0f, 0.98f, 0.0f);
			// ApplyForce as a Ballistic Bomb
			//robotShooting->currentWeapon->ApplyForce(direction);
			//robotShooting->currentWeapon->ApplyForce(gravity);

			std::vector<iRobot*> robotsInExplosion;
			robotsInExplosion = CalculateRadius(robotToShoot->position, robotShooting->currentWeapon->radius);

			// Just taking damage for this project
			// Still need to calculate the radius of explosion
			for (int i = 0; i < robotsInExplosion.size(); i++) {
				cRobot* currRobot = (cRobot*)robotsInExplosion[i];
				currRobot->TakeDamage(amount);
			}

			robotShooting->currentWeapon->SetWaponCD(-1 * robotShooting->currentWeapon->GetWeaponCurrCD());
		}
		else {
			std::cout << "Robot " << robotShooting->getID() << " recharding . . . " << std::endl;
		}
	}
	else if (lastIndexName == '3') {
		//LOS Shot! - BULLET
		// Check first if the Robot is "Hittable"
		if (robotShooting->haveLOS) {
			glm::vec3 TargetToRobotVector = robotToShoot->position - robotShooting->currentWeapon->position;
			glm::vec3 DirectionToTarget = glm::normalize(TargetToRobotVector);

			Vector3 direction =
				Vector3(DirectionToTarget.x, DirectionToTarget.y, DirectionToTarget.z);

			// ApplyForce as a BULLET
			// No Gravity
			//robotShooting->currentWeapon->ApplyForce(direction);

			// Just taking damage for this project
			robotToShoot->TakeDamage(amount);
		}
		else {
			SetClosestRobot(robotShooting);
			return false;
		}		
	}

	return true;

	// Code to find closest robot
	// I'm lazy so I'm going to just pick a random robot
	
	//cRobot* pClosestRobot = (cRobot*)this->m_vecTheRobots[rand() % 8];
	//cRobot* pClosestRobot = (cRobot*)getRobotFromIndex(pMeWhosAsking->getClosestRobotID());

	//pClosestRobot->TakeDamage(amount);

	//return true;
}

std::vector<iRobot*> cRobotShepherd::CalculateRadius(glm::vec3 center, float explosionRadius) {
	std::vector<iRobot*> robotsInRange;

	for (int i = 0; i < m_vecTheRobots.size(); i++) {
		cRobot* currRobot = (cRobot*)m_vecTheRobots[i];
		if (glm::distance(currRobot->position, center) < explosionRadius) {
			robotsInRange.push_back(currRobot);
		}
	}

	return robotsInRange;
}

void cRobotShepherd::RemoveMe(iRobot* pMeWhoIsAsking) {
	for (int i = 0; i < m_vecTheRobots.size(); i++) {
		if (m_vecTheRobots[i]->getID() == pMeWhoIsAsking->getID())
			m_vecTheRobots.erase(m_vecTheRobots.begin() + i);
	}
}
