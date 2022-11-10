#pragma once

#include "Particle.h"
#include "cMeshObject.h"

class cWeapon : 
	public Particle,
	public cMeshObject{
public:
	cWeapon() {
		weaponCooldown = 0.0f;
		damage = 0.0f;
	};

	void SetWaponCD(float cd) {
		weaponCooldown = cd;
	};

	float GetWeaponCurrCD() {
		return weaponCooldown;
	};

	void UpdateCD(float dt) {
		weaponCooldown -= dt;
	};

	float damage;

private:
	// Weapon cooldown in seconds
	float weaponCooldown;
};