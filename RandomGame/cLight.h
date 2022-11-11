#ifndef _cLight_HG_
#define _cLight_HG_

#include <glm/glm.hpp>
#include <glm/vec4.hpp>
#include <string>

class cLight
{
public:
	cLight();
	std::string name;
	glm::vec4 position;
	glm::vec4 diffuse;
	glm::vec4 specular;	// rgb = highlight colour, w = power
	glm::vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	glm::vec4 direction;	// Spot, directional lights
	glm::vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
					// 0 = pointlight
					// 1 = spot light
					// 2 = directional light
	glm::vec4 param2;	// x = 0 for off, 1 for on

	// Also store the uniform locations from the shader here
	int position_UniformLocation = -1;		// This only works with C++ 11
	int diffuse_UniformLocation = -1;
	int specular_UniformLocation = -1;
	int atten_UniformLocation = -1;
	int direction_UniformLocation = -1;
	int param1_UniformLocation = -1;
	int param2_UniformLocation = -1;


	void setConstantAttenuation(float newConstAtten);
	void setLinearAttenuation(float newLinearAtten);
	void setQuadraticAttenuation(float newQuadAtten);
	// Maybe these, too
	void TurnOn(void);
	void TurnOff(void);
	void SetToWhite(void);
	void SetToBlack(void);
	enum eLightType
	{
		POINT_LIGHT,
		DIRECTIONAL_LIGHT,
		SPOT_LIGHT
	};
	void SetLightType(eLightType newLightType);
	void MakeItAPointLight(void);
	void MakeItASpotLight(glm::vec3 direction, float innerAngle, float outerAngle);
};

#endif
