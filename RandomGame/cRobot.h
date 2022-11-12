#pragma once
#include <string>
#include <vector>

#include "iRobot.h"
#include "iDamage.h"
//#include "cRobotShepherd.h"
#include "iRobotInformant.h"
#include "cMeshObject.h"
#include "cWeapon.h"

class cRobot :
	public iRobot,		// This is "multiple inheritance"
	public iDamage,
	public cMeshObject
{
public:
	cRobot();

	std::string name;
	float health;
	sVec3 location;

	// from the iDamage interface
	virtual void TakeDamage(float amount);

	bool haveLOS;
	unsigned int pClosestRobotId;
	std::vector<int> testedIndexes;
	bool needRepositioning = false;

	void addToTestedIndexes(int index);

	// The Shepherd calls this
	void SetRobotShepherd(iRobotInformant* pTheShepherd);
	void ShootRobot(cRobot* robotToShoot);
	void SetRobotWeapon(cWeapon* currWeapon);

	cWeapon* currentWeapon;

	// From the iRobot interface
	virtual void Update(double deltaTime);

	virtual bool amIDead() {
		if (health < 0.0f)
			return true;
		else
			return false;
	}

	virtual unsigned int getID(void) {
		return this->m_ID;
	}

	virtual unsigned int getClosestRobotID(void) {
		return pClosestRobotId;
	}

	virtual void setClosestRobotID(unsigned int id);

private:
	unsigned int m_ID;

	iRobotInformant* m_pTheShepherd;

	// Static = There's only one for ALL instances
	// Const just means I can't change it
	static const unsigned int STARTING_ID = 5000;
	static const unsigned int ID_VALUE_INCREMENT = 5000;
	// This is JUST static, not const 
	static unsigned int m_nextID;// = 5000;
};
