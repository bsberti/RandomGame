#include "cLightManager.h"

#include "globalOpenGL.h"
#include <sstream>		// Light a string builder in other languages

cLightManager::cLightManager()
{
	lightsCreated = 0;
	// Create the initial lights
	for (unsigned int count = 0;
		count != cLightManager::NUMBER_OF_LIGHTS_IM_GONNA_USE;
		count++)
	{

		cLight newLight;
		this->vecTheLights.push_back(newLight);
	}
}

void cLightManager::DaylightScene() {
	// Sun or Moon light
	//cLight light0 = vecTheLights[0];
	//cLight light1 = vecTheLights[1];

	//Constructions light
	//cLight light2 = vecTheLights[2];
	//cLight light3 = vecTheLights[3];
	//cLight light4 = vecTheLights[4];
	//cLight light5 = vecTheLights[5];

	vecTheLights[0].direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
	vecTheLights[0].atten = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

	vecTheLights[1].direction = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	vecTheLights[1].position = glm::vec4(103.0f, 7.0f, 67.0f, 0.0f);
	vecTheLights[1].atten = glm::vec4(1.0f, 0.121f, 0.0f, 0.0f);

	vecTheLights[2].TurnOff();
	vecTheLights[3].TurnOff();
	vecTheLights[4].TurnOff();
	vecTheLights[5].TurnOff();
}

void cLightManager::NightlightScene() {
	// Sun or Moon light
	//cLight light0 = vecTheLights[0];
	//cLight light1 = vecTheLights[1];

	//Constructions light
	//cLight light2 = vecTheLights[2];
	//cLight light3 = vecTheLights[3];
	//cLight light4 = vecTheLights[4];
	//cLight light5 = vecTheLights[5];

	vecTheLights[0].direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
	vecTheLights[0].diffuse = glm::vec4(0.01f, 0.01f, 0.01f, 1.0f);
	vecTheLights[0].atten = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

	vecTheLights[1].direction = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	vecTheLights[1].atten = glm::vec4(1.0f, 1.0f, 0.0f, 0.0f);

	vecTheLights[2].direction = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	vecTheLights[2].atten = glm::vec4(-1.0f, 0.956f, 0.627f, 0.0f);
	vecTheLights[2].position = glm::vec4(16.484f, 109.889f, 0.0f, 0.0f);
	vecTheLights[2].TurnOn();

	vecTheLights[3].direction = glm::vec4(0.011f, 0.005f, 0.0f, 1.0f);
	vecTheLights[3].atten = glm::vec4(-1.0f, 0.660f, 0.198f, 0.0f);
	vecTheLights[3].position = glm::vec4(0.0f, 87.912f, -38.462f, 0.0f);
	vecTheLights[3].TurnOn();

	vecTheLights[4].direction = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	vecTheLights[4].atten = glm::vec4(1.0f, 1.0f, 0.095f, 0.0f);
	vecTheLights[4].position = glm::vec4(-68.345f, 7.194f, 93.525f, 0.0f);
	vecTheLights[4].TurnOn();

	vecTheLights[5].direction = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	vecTheLights[5].atten = glm::vec4(1.0f, 1.0f, 0.095f, 0.0f);
	vecTheLights[5].position = glm::vec4(-21.583f, 50.360f, 14.388f, 0.0f);
	vecTheLights[5].TurnOn();

	vecTheLights[6].direction = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	vecTheLights[6].atten = glm::vec4(1.0f, 1.0f, 0.095f, 0.0f);
	//vecTheLights[6].position = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	vecTheLights[6].TurnOn();

	vecTheLights[7].direction = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	vecTheLights[7].atten = glm::vec4(1.0f, 1.0f, 0.095f, 0.0f);
	//vecTheLights[7].position = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	vecTheLights[7].TurnOn();
}

void cLightManager::CreateBasicPointLight(unsigned int shaderID, glm::vec4 position) {
	// Set up the uniform variable from the shader
	LoadLightUniformLocations(shaderID);
	vecTheLights[lightsCreated].name = "Light" + std::to_string(lightsCreated);
	vecTheLights[lightsCreated].position = position;
	vecTheLights[lightsCreated].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	vecTheLights[lightsCreated].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	vecTheLights[lightsCreated].atten = glm::vec4(0.1f, 0.807497f, 0.0000001f, 1.0f);

	// In the shader Feeney gave you, the direciton is relative
	vecTheLights[lightsCreated].direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);

	// Make this a spot light
	//    vec4 param1;	   x = lightType, y = inner angle, z = outer angle, w = TBD
	//                     0 = pointlight
	//                     1 = spot light
	//                     2 = directional light
	vecTheLights[lightsCreated].param1 = glm::vec4(0.0f, 45.0f, 90.0f, 1.0f);     // Degrees
	vecTheLights[lightsCreated].param2 = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f); 		
	vecTheLights[lightsCreated].TurnOn();

	lightsCreated++;
}

void cLightManager::CreateBasicSpotLight(unsigned int shaderID, glm::vec4 position) {
	// Set up the uniform variable from the shader
	LoadLightUniformLocations(shaderID);
	vecTheLights[lightsCreated].name = "Light" + std::to_string(lightsCreated);
	vecTheLights[lightsCreated].position = position;
	vecTheLights[lightsCreated].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	vecTheLights[lightsCreated].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	vecTheLights[lightsCreated].atten = glm::vec4(0.1f, 0.807497f, 0.0000001f, 1.0f);

	// In the shader Feeney gave you, the direciton is relative
	vecTheLights[lightsCreated].direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);

	// Make this a spot light
	//    vec4 param1;	   x = lightType, y = inner angle, z = outer angle, w = TBD
	//                     0 = pointlight
	//                     1 = spot light
	//                     2 = directional light
	vecTheLights[lightsCreated].param1 = glm::vec4(1.0f, 45.0f, 90.0f, 1.0f);     // Degrees
	vecTheLights[lightsCreated].param2 = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	vecTheLights[lightsCreated].TurnOn();

	lightsCreated++;
}

void cLightManager::CreateBasicDirecLight(unsigned int shaderID, glm::vec4 position) {
	// Set up the uniform variable from the shader
	LoadLightUniformLocations(shaderID);
	vecTheLights[lightsCreated].name = "Light" + std::to_string(lightsCreated);
	vecTheLights[lightsCreated].position = position;
	vecTheLights[lightsCreated].diffuse = glm::vec4(0.06f, 0.06f, 0.06f, 1.0f);
	vecTheLights[lightsCreated].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	vecTheLights[lightsCreated].atten = glm::vec4(0.1f, 0.807497f, 0.0000001f, 1.0f);

	// In the shader Feeney gave you, the direciton is relative
	vecTheLights[lightsCreated].direction = glm::vec4(0.0f, -0.5f, 0.0f, 1.0f);

	// Make this a spot light
	//    vec4 param1;	   x = lightType, y = inner angle, z = outer angle, w = TBD
	//                     0 = pointlight
	//                     1 = spot light
	//                     2 = directional light
	vecTheLights[lightsCreated].param1 = glm::vec4(2.0f, 45.0f, 90.0f, 1.0f);     // Degrees
	vecTheLights[lightsCreated].param2 = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	vecTheLights[lightsCreated].TurnOn();

	lightsCreated++;
}

void cLightManager::LoadLightUniformLocations(unsigned int shaderID)
{
	unsigned int lightNumber = 0;
	for (std::vector<cLight>::iterator itLight = this->vecTheLights.begin();
		itLight != this->vecTheLights.end();
		itLight++)
	{
		std::stringstream ssLightName;
		ssLightName << "theLights[" << lightNumber << "].";

		std::string lightPosition = ssLightName.str() + "position";
		itLight->position_UniformLocation = glGetUniformLocation(shaderID, lightPosition.c_str());

		std::string lightDiffuse = ssLightName.str() + "diffuse";
		itLight->diffuse_UniformLocation = glGetUniformLocation(shaderID, lightDiffuse.c_str());

		std::string lightSpecular = ssLightName.str() + "specular";
		itLight->specular_UniformLocation = glGetUniformLocation(shaderID, lightSpecular.c_str());

		std::string lightAtten = ssLightName.str() + "atten";
		itLight->atten_UniformLocation = glGetUniformLocation(shaderID, lightAtten.c_str());

		std::string lightDirection = ssLightName.str() + "direction";
		itLight->direction_UniformLocation = glGetUniformLocation(shaderID, lightDirection.c_str());

		std::string lightParam1 = ssLightName.str() + "param1";
		itLight->param1_UniformLocation = glGetUniformLocation(shaderID, lightParam1.c_str());

		std::string lightParam2 = ssLightName.str() + "param2";
		itLight->param2_UniformLocation = glGetUniformLocation(shaderID, lightParam2.c_str());

		lightNumber++;
	}

	return;
}

void cLightManager::CopyLightInformationToShader(unsigned int shaderID)
{
	for (std::vector<cLight>::iterator itLight = this->vecTheLights.begin();
		itLight != this->vecTheLights.end();
		itLight++)
	{
		glUniform4f(itLight->position_UniformLocation,
			itLight->position.x,
			itLight->position.y,
			itLight->position.z,
			itLight->position.w);

		glUniform4f(itLight->diffuse_UniformLocation,
			itLight->diffuse.x,
			itLight->diffuse.y,
			itLight->diffuse.z,
			itLight->diffuse.w);

		glUniform4f(itLight->specular_UniformLocation,
			itLight->specular.x,
			itLight->specular.y,
			itLight->specular.z,
			itLight->specular.w);

		glUniform4f(itLight->atten_UniformLocation,
			itLight->atten.x,
			itLight->atten.y,
			itLight->atten.z,
			itLight->atten.w);

		glUniform4f(itLight->direction_UniformLocation,
			itLight->direction.x,
			itLight->direction.y,
			itLight->direction.z,
			itLight->direction.w);

		glUniform4f(itLight->param1_UniformLocation,
			itLight->param1.x,
			itLight->param1.y,
			itLight->param1.z,
			itLight->param1.w);

		glUniform4f(itLight->param2_UniformLocation,
			itLight->param2.x,
			itLight->param2.y,
			itLight->param2.z,
			itLight->param2.w);


	}// for (std::vector<cLight>::iterator itLight

	return;
}

