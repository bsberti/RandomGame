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

	testedIndexes.resize(0);
}

// Note the very strange 
//static 
unsigned int cRobot::m_nextID = cRobot::STARTING_ID;

// Or something like this:
//unsigned int cRobot::m_nextID = 5000;

void cRobot::ShootRobot(cRobot* robotToShoot) {
	
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

	this->haveLOS = m_pTheShepherd->checkLOS(this, closestRobot);
	this->testedIndexes.push_back(closestRobot->getID());
	needRepositioning = (testedIndexes.size() >= 9);

	/*std::cout << "Robot: " << this->getID() << " - " << (!needRepositioning ?
		(this->haveLOS ? "LOS TRUE!" : "LOS FALSE for " + std::to_string(closestRobot->getID())) :
		"LOS FALSE for ALL") << std::endl;*/

	// Damage depends on weapon
	if (this->m_pTheShepherd->ShootTheClosestRobot(this, currentWeapon->damage)) {
		std::cout << "Robot " << this->getID() << " just Shoot Robot " << closestRobot->getID() << std::endl;
	}
	else {
		if (needRepositioning) {
			// TELEPORT
			this->testedIndexes.resize(0);
			m_pTheShepherd->TeleportMe(this);
		}
	}

	currentWeapon->UpdateCD(deltaTime);
}

void cRobot::addToTestedIndexes(int index) {
	testedIndexes.push_back(index);
}

void cRobot::setClosestRobotID(unsigned int id) {
	this->pClosestRobotId = id;
}

void cRobot::TakeDamage(float amount)
{
	std::cout << "Robot " << this->getID() << " took "
		<< amount << " damage!!" << std::endl;
	health -= 100 * amount;
	if (health <= 0) {
		std::cout << "Robot " << m_ID << " just Died! </3" << std::endl;
		//m_pTheShepherd->RemoveMe(this);
		return;
	}

	std::cout << "Robot " << this->getID() << " health is " << health << std::endl;
}