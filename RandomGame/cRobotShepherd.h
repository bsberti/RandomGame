#pragma once

#include "iRobot.h"
#include "cRobot.h"
#include "iRobotInformant.h"
#include <vector>

// Command pattern 
struct sMessage
{
	std::string theCommand;
	std::string someData;
	float fData[4];
};

//sMessage shotThisGuy;
//shotThisGuy.theCommand = "shoot closest robot"
//shotThisGuy.theCommand = "SCR"
//shotThisGuy.fData[0] = 10.0f;

// This is:
// * the factory object to create robots
// * keeps track of robots for other robots
class cRobotShepherd :
	public iRobotInformant			// Interface
{
public:
	cRobotShepherd();

	// Factory method
	iRobot* makeRobot(void);

	virtual iRobot* getRobotFromIndex(int index);

	int getRobotNumber();

	void setTerrain(cMeshObject* terrain) {
		this->terrain = terrain;
	};

	// Commands that the robots can call to ask about things, 
	//  like where my robots at? 
	void SetClosestRobot(cRobot* pMeWhosAsking);

	// Teleport a robot when LOS is false
	virtual void TeleportMe(iRobot* pMeWhoIsAsking);

	// Return true if I actually hit something... maybe
	bool ShootTheClosestRobot(iRobot* pMeWhosAsking, float amount);

	// Return true if A can shot laser or bullets at B
	virtual bool checkLOS(iRobot* robotA, iRobot* robotB);

	// Remove Robot from m_vecTheRobots;
	virtual void RemoveMe(iRobot* pMeWhoIsAsking);

	// Calls update on all the robots
	void Update(double deltaTime);

	// Return a vector of Robots in a explosion Radius
	std::vector<iRobot*> CalculateRadius(glm::vec3 center, float explosionRadius);

private:
	std::vector<cRobot*> m_vecTheRobots;
	cMeshObject* terrain;
};