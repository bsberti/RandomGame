#pragma once

// This is an interface class
// So only methods and all "pure virtual"

class iDamage
{
public:
	virtual ~iDamage() {}
	virtual void TakeDamage(float amount) = 0;
};