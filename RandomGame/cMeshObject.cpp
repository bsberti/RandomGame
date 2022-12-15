#include "cMeshObject.h"

unsigned int cMeshObject::nextID = cMeshObject::STARTING_ID;

cMeshObject::cMeshObject() {
	this->position = glm::vec3(0.0f);

	//this->rotation = glm::vec3(0.0f);		// Euler angles
	// Set rotation to 0 degrees around each axis
	this->qRotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));

	// RGB and "alpha" (A) 0.0f = transparent
	this->SetUniformScale(1.0f);
	this->isWireframe = false;

	// This will mainly be used for non-lit debug objects, eventually
	this->RGBA_colour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	this->bUse_RGBA_colour = false;

	// Make this white unless it's plastic or metal
	// Specualar (w) of 1 is "not" shiny
	this->specular_colour_and_power = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	this->bDoNotLight = false;
	this->bIsVisible = true;

	this->textureRatios[0] = 0.0f;
	this->textureRatios[1] = 0.0f;
	this->textureRatios[2] = 0.0f;
	this->textureRatios[3] = 0.0f;
	this->textureRatios[4] = 0.0f;
	this->textureRatios[5] = 0.0f;
	this->textureRatios[6] = 0.0f;
	this->textureRatios[7] = 0.0f;

	this->m_myID = cMeshObject::nextID;
	cMeshObject::nextID += 1;

	this->currentI = 0;
	this->currentJ = 0;
	this->moving = 0;
}

cMeshObject* cMeshObject::findObjectByFriendlyName(std::string nameToFind, bool bSearchChildren /*=true*/) {
	for (std::vector< cMeshObject* >::iterator itCurrentMesh = this->vecChildMeshes.begin();
		itCurrentMesh != this->vecChildMeshes.end();
		itCurrentMesh++)
	{
		cMeshObject* pCurrentMesh = *itCurrentMesh;

		if (pCurrentMesh->friendlyName == nameToFind) {
			return pCurrentMesh;
		}

		// Search children too? 
		cMeshObject* pChildMesh = pCurrentMesh->findObjectByFriendlyName(nameToFind, bSearchChildren);

		if (pChildMesh)  { /* NULL = 0 = false) */
			return pChildMesh;
		}
	}

	return NULL;
}