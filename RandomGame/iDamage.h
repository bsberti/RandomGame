//#pragma once
#ifndef _iDamage_2022_11_07_
#define _iDamage_2022_11_07_

// This is an interface class
// So only methods and all "pure virtual"

class iDamage
{
public:
	virtual ~iDamage() {}
	virtual void TakeDamage(float amount) = 0;
};
#endif