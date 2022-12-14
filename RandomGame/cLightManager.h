#pragma once

#include "cLight.h"
#include <vector>
#include <string>

class cLightManager
{
public:
	cLightManager();



	static const unsigned int NUMBER_OF_LIGHTS_IM_GONNA_USE = 30;
	std::vector<cLight> vecTheLights;
	unsigned int lightsCreated;

	void LoadLightUniformLocations(unsigned int shaderID);
	void CopyLightInformationToShader(unsigned int shaderID);
	void CreateBasicLight(unsigned int shaderID, glm::vec4 position);
	void CreateBasicPointLight(unsigned int shaderID, glm::vec4 position);
	void CreateBasicSpotLight(unsigned int shaderID, glm::vec4 position);
	void CreateBasicDirecLight(unsigned int shaderID, glm::vec4 position);

	// Or we could use a simple array
	//	cLight theLights[NUMBER_OF_LIGHTS_IM_GONNA_USE];

};
