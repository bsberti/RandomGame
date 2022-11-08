#pragma once
#include <string>
#include <vector>

#include "iRobot.h"
#include "iDamage.h"
//#include "cRobotShepherd.h"
#include "iRobotInformant.h"
#include "cMeshObject.h"

class cRobot :
	public iRobot,		// This is "multiple inheritance"
	public iDamage,
	public cMeshObject
{
public:

	//	friend cRobotShepherd;
	cRobot();

	std::string name;
	float health;
	sVec3 location;

	// from the iDamage interface
	virtual void TakeDamage(float amount);



	// The Shepherd calls this
	void SetRobotShepherd(iRobotInformant* pTheShepherd);

	void ShootRobot(void);


	// From the iRobot interface
	virtual void Update(double deltaTime);
	virtual unsigned int getID(void)
	{
		return this->m_ID;
	}
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
