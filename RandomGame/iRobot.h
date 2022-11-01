//#pragma once
#ifndef _iRobot_HG_2022_10_13_
#define _iRobot_HG_2022_10_13_

#include <string>

struct sVec3
{
	float x, y, z;
};

class iRobot
{
public:
	// No constructor needed because we never call "new" on this
//	iAnimal();				
	virtual ~iRobot() {};

	// Called every frame
	virtual void Update(double deltaTime) = 0;

	// 
	// FindTarget();
	// ShootTarget();

	virtual unsigned int getID(void) = 0;

};



#endif 