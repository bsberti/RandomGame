#pragma once

#include "cLight.h"
#include <vector>
#include <string>

class cLightManager
{
public:
	friend class cRandomUI;

	cLightManager();

	void DaylightScene();
	void NightlightScene();

	static const unsigned int NUMBER_OF_LIGHTS_IM_GONNA_USE = 8;
	std::vector<cLight> vecTheLights;
	unsigned int lightsCreated;

	void LoadLightUniformLocations(unsigned int shaderID);
	void CopyLightInformationToShader(unsigned int shaderID);
	void CreateBasicPointLight(unsigned int shaderID, glm::vec4 position);
	void CreateBasicSpotLight(unsigned int shaderID, glm::vec4 position);
	void CreateBasicDirecLight(unsigned int shaderID, glm::vec4 position);

	// Or we could use a simple array
	//	cLight theLights[NUMBER_OF_LIGHTS_IM_GONNA_USE];

};
