#pragma once

#include "iRobot.h"
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
	iRobot* getRobotFromIndex(int index);
	//iRobot* makeRobot(std::string robotType);

	int getRobotNumber();

	// Commands that the robots can call to ask about things, 
	//  like where my robots at? 
	// Returns NULL (or 0 or nullptr) if there isn't any
	//iRobot* findClosestRobot(iRobot* pMeWhosAsking);
	iDamage* findClosestRobot(iRobot* pMeWhosAsking);

	// Return true if I actually hit something... maybe
	bool ShootTheClosestRobot(iRobot* pMeWhosAsking, float amount);

	// Type of mediator method
	sMessage DoSomething(iRobot* pWhosCalling, sMessage theMessage);



	// or you could do this, where returning false means there isn't one
//	bool findClosestRobot(iRobot* pMeWhosAsking, iRobot* &pClosestRobot);

	// Calls update on all the robots
	void Update(double deltaTime);

private:
	std::vector<iRobot*> m_vecTheRobots;

};