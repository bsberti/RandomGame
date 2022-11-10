#include "cRobot.h"
#include <iostream>
#include "Vector3.h"

cRobot::cRobot()
{
	this->m_ID = cRobot::m_nextID;
	// Not increment
	cRobot::m_nextID += ID_VALUE_INCREMENT;

	pClosestRobotId = NULL;
	haveLOS = false;

	// Full health
	this->health = 100.0f;
}

// Note the very strange 
//static 
unsigned int cRobot::m_nextID = cRobot::STARTING_ID;

// Or something like this:
//unsigned int cRobot::m_nextID = 5000;

void cRobot::ShootRobot(cRobot* robotToShoot) {
	//robotToShoot = (cRobot*)m_pTheShepherd.getRobotFromIndex(this);
	char lastIndexName = this->currentWeapon->friendlyName[this->currentWeapon->friendlyName.length() - 1];
	
	if (lastIndexName == '1') {
		// Check Weapon CD
		if (this->currentWeapon->GetWeaponCurrCD() > 0) {
			// LOS Shot! - LASER
			// Check first if the Robot is "Hittable"
			if (haveLOS) {
				glm::vec3 TargetToRobotVector = robotToShoot->position - currentWeapon->position;
				glm::vec3 DirectionToTarget = glm::normalize(TargetToRobotVector);

				Vector3 direction =
					Vector3(DirectionToTarget.x, DirectionToTarget.y, DirectionToTarget.z);

				// ApplyForce as a Laser
				this->currentWeapon->ApplyForce(direction);
				// No Gravity
			}
		}
		
	}
	else if (lastIndexName == '2') {
		//BALLISTIC Shot! - BOMB

	}
	else if (lastIndexName == '3') {
		//LOS Shot! - BULLET

	}
	return;
}

// The Shepherd calls this
void cRobot::SetRobotShepherd(iRobotInformant* pTheShepherd)
{
	this->m_pTheShepherd = pTheShepherd;
	return;
}

void cRobot::SetRobotWeapon(cWeapon* currWeapon) {
	this->currentWeapon = currWeapon;
}


void cRobot::Update(double deltaTime)
{
	// Assume we are about to find a robot
	cRobot* closestRobot = (cRobot*)m_pTheShepherd->getRobotFromIndex(this->getClosestRobotID());

	std::vector<int> testedIndexes;
    testedIndexes.resize(0);
    bool getOut = true;
    do {		
        this->haveLOS = m_pTheShepherd->checkLOS(this, closestRobot);
		testedIndexes.push_back(this->getClosestRobotID());
        getOut = (testedIndexes.size() < 9);

        //std::cout << currentRobot. << std::endl;
        std::cout << "Robot: " << this->getID() << " - " << (getOut ? (this->haveLOS ? "LOS TRUE!" : "LOS FALSE for ") : "LOS FALSE for ALL") << std::endl;

    } while (!this->haveLOS && getOut);

	//	iDamage* pTheClosestRobot = this->m_pTheShepherd->findClosestRobot(this);
	//	// Hit the robot
	//	pTheClosestRobot->TakeDamage(10.0f);
	
	// Damage depends on weapon
	if (!this->m_pTheShepherd->ShootTheClosestRobot(this, currentWeapon->damage)) {
		// TELEPORT
	}

	return;
}

void cRobot::setClosestRobotID(unsigned int id) {
	this->pClosestRobotId = id;
}

void cRobot::TakeDamage(float amount)
{
	std::cout << "Robot " << this->getID() << " took "
		<< amount << " damage!!" << std::endl;
	return;
}