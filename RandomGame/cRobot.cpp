#include "cRobot.h"
#include <iostream>

cRobot::cRobot()
{
	this->m_ID = cRobot::m_nextID;
	// Not increment
	cRobot::m_nextID += ID_VALUE_INCREMENT;

	// Full health
	this->health = 100.0f;
}

// Note the very strange 
//static 
unsigned int cRobot::m_nextID = cRobot::STARTING_ID;

// Or something like this:
//unsigned int cRobot::m_nextID = 5000;

void cRobot::ShootRobot(void)
{
	// Find closest robot to me
	// Shoot it!

	return;
}

// The Shepherd calls this
void cRobot::SetRobotShepherd(iRobotInformant* pTheShepherd)
{
	this->m_pTheShepherd = pTheShepherd;
	return;
}



void cRobot::Update(double deltaTime)
{
	// Assume we are about to find a robot
	// iRobot* findClosestRobot(iRobot* pMeWhosAsking);

//	iDamage* pTheClosestRobot = this->m_pTheShepherd->findClosestRobot(this);
//	// Hit the robot
//	pTheClosestRobot->TakeDamage(10.0f);

	this->m_pTheShepherd->ShootTheClosestRobot(this, 10.0f);



	return;
}

void cRobot::TakeDamage(float amount)
{
	std::cout << "Robot " << this->getID() << " took "
		<< amount << " damage!!" << std::endl;
	return;
}