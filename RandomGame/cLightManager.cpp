#include "cLightManager.h"

#include "globalOpenGL.h"
#include <sstream>		// Light a string builder in other languages

cLightManager::cLightManager()
{
	// Create the initial lights
	for ( unsigned int count = 0;
		  count != cLightManager::NUMBER_OF_LIGHTS_IM_GONNA_USE;
		  count++ )
	{

		cLight newLight;
		this->vecTheLights.push_back(newLight);
	}

}

void cLightManager::LoadLightUniformLocations(unsigned int shaderID)
{
//	unsigned int lightNumber = 0;
//	for (std::vector<cLight>::iterator itLight = this->vecTheLights.begin();
//		 itLight != this->vecTheLights.end();
//		 itLight++ )
//	{
////		std::string lightName = "theLights[" + lightNumber + "].position";
//		std::stringstream ssLightName;
//		ssLightName << "theLights[" << lightNumber << "].";
//
//		// Set the light information for lights (from the shader)
//		std::string lightPosition = ssLightName.str() + "position";
//		itLight->position_UniformLocation = glGetUniformLocation(shaderID, lightPosition.c_str() );
//
//		// Left for the student to complete... 
//		itLight->diffuse_UniformLocation = glGetUniformLocation(shaderID, "theLights[0].diffuse");
//		itLight->specular_UniformLocation = glGetUniformLocation(shaderID, "theLights[0].specular");
//		itLight->atten_UniformLocation = glGetUniformLocation(shaderID, "theLights[0].atten");
//		itLight->direction_UniformLocation = glGetUniformLocation(shaderID, "theLights[0].direction");
//		itLight->param1_UniformLocation = glGetUniformLocation(shaderID, "theLights[0].param1");
//		itLight->param2_UniformLocation = glGetUniformLocation(shaderID, "theLights[0].param2");
//
//		lightNumber++;
//	}

	// Or you could do this:
	this->vecTheLights[0].position_UniformLocation = glGetUniformLocation(shaderID, "theLights[0].position");
	this->vecTheLights[0].diffuse_UniformLocation = glGetUniformLocation(shaderID, "theLights[0].diffuse");
	this->vecTheLights[0].specular_UniformLocation = glGetUniformLocation(shaderID, "theLights[0].specular");
	this->vecTheLights[0].atten_UniformLocation = glGetUniformLocation(shaderID, "theLights[0].atten");
	this->vecTheLights[0].direction_UniformLocation = glGetUniformLocation(shaderID, "theLights[0].direction");
	this->vecTheLights[0].param1_UniformLocation = glGetUniformLocation(shaderID, "theLights[0].param1");
	this->vecTheLights[0].param2_UniformLocation = glGetUniformLocation(shaderID, "theLights[0].param2");

	this->vecTheLights[1].position_UniformLocation = glGetUniformLocation(shaderID, "theLights[1].position");
	this->vecTheLights[1].diffuse_UniformLocation = glGetUniformLocation(shaderID, "theLights[1].diffuse");
	this->vecTheLights[1].specular_UniformLocation = glGetUniformLocation(shaderID, "theLights[1].specular");
	this->vecTheLights[1].atten_UniformLocation = glGetUniformLocation(shaderID, "theLights[1].atten");
	this->vecTheLights[1].direction_UniformLocation = glGetUniformLocation(shaderID, "theLights[1].direction");
	this->vecTheLights[1].param1_UniformLocation = glGetUniformLocation(shaderID, "theLights[1].param1");
	this->vecTheLights[1].param2_UniformLocation = glGetUniformLocation(shaderID, "theLights[1].param2");

	this->vecTheLights[2].position_UniformLocation = glGetUniformLocation(shaderID, "theLights[2].position");
	this->vecTheLights[2].diffuse_UniformLocation = glGetUniformLocation(shaderID, "theLights[2].diffuse");
	this->vecTheLights[2].specular_UniformLocation = glGetUniformLocation(shaderID, "theLights[2].specular");
	this->vecTheLights[2].atten_UniformLocation = glGetUniformLocation(shaderID, "theLights[2].atten");
	this->vecTheLights[2].direction_UniformLocation = glGetUniformLocation(shaderID, "theLights[2].direction");
	this->vecTheLights[2].param1_UniformLocation = glGetUniformLocation(shaderID, "theLights[2].param1");
	this->vecTheLights[2].param2_UniformLocation = glGetUniformLocation(shaderID, "theLights[2].param2");

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
					itLight->position.w );

		glUniform4f(itLight->diffuse_UniformLocation,
					itLight->diffuse.x,
					itLight->diffuse.y,
					itLight->diffuse.z,
					itLight->diffuse.w );

		glUniform4f(itLight->specular_UniformLocation,
					itLight->specular.x,
					itLight->specular.y,
					itLight->specular.z,
					itLight->specular.w );

		glUniform4f(itLight->atten_UniformLocation,
					itLight->atten.x,
					itLight->atten.y,
					itLight->atten.z,
					itLight->atten.w );

		glUniform4f(itLight->direction_UniformLocation,
					itLight->direction.x,
					itLight->direction.y,
					itLight->direction.z,
					itLight->direction.w );

		glUniform4f(itLight->param1_UniformLocation,
					itLight->param1.x,
					itLight->param1.y,
					itLight->param1.z,
					itLight->param1.w );

		glUniform4f(itLight->param2_UniformLocation,
					itLight->param2.x,
					itLight->param2.y,
					itLight->param2.z,
					itLight->param2.w );


	}// for (std::vector<cLight>::iterator itLight

	return;
}

