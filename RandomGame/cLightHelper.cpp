#include "cLightHelper.h"


//static 
const float cLightHelper::DEFAULT_ATTEN_CONST = 0.1f;

//static 
const float cLightHelper::DEFAULT_ATTEN_LINEAR = 0.1f;

//static 
const float cLightHelper::DEFAULT_ATTEN_QUADRATIC = 0.1f;

//static
const float cLightHelper::DEFAULT_AMBIENT_TO_DIFFUSE_RATIO = 0.8f;

//static 
const float cLightHelper::DEFAULTINFINITEDISTANCE = 10000.0f;	

//static 
const float cLightHelper::DEFAULDIFFUSEACCURACYTHRESHOLD = 0.001f;

float cLightHelper::calcApproxDistFromAtten( float targetLightLevel )	// Uses the defaults
{
	return this->calcApproxDistFromAtten( targetLightLevel, cLightHelper::DEFAULDIFFUSEACCURACYTHRESHOLD );
}

float cLightHelper::calcApproxDistFromAtten(float targetLightLevel, float accuracy )	// Uses the defaults
{
	return this->calcApproxDistFromAtten( targetLightLevel, accuracy, 
										  cLightHelper::DEFAULT_ATTEN_CONST,
										  cLightHelper::DEFAULT_ATTEN_LINEAR, 
										  cLightHelper::DEFAULT_ATTEN_QUADRATIC, 
										  cLightHelper::DEFAULTINFINITEDISTANCE, cLightHelper::DEFAULTMAXITERATIONS );
}
                                                      
float cLightHelper::calcApproxDistFromAtten( float targetLightLevel, float accuracy, 
                                             float infiniteDistance, 
											 float constAttenuation, 
											 float linearAttenuation, 
											 float quadraticAttenuation, 
											 unsigned int maxIterations /*= DEFAULTMAXITERATIONS = 50*/ )
{
	// See if the accuracy being set it too big for the targetLightLevel, unless targetLightLevel is actually zero (0.0f)
	// If it's actually zero, then adjusting the accuracy to a tenth of zero would give... wait for it...
	//	zero, and we would max out the iterations
	if ( targetLightLevel != 0.0f )	
	{	
		if ( ( accuracy * 10.0f) >= targetLightLevel * 10.0f )
		{	// Adjust the accuracy by a hundreth
			accuracy = targetLightLevel / 10.0f;
		}
	}//if ( targetLightLevel != 0.0f )	

	float targetLightLevelLow = targetLightLevel - accuracy;
	//if ( targetLightLevelLow <= 0.0f )	{ targetLightLevel = 0.0f; }
	float targetLightLevelHigh = targetLightLevel + accuracy;

	// See if we're getting a value at infinite. i.e. at 'infinite distance', is the light level too high already
	if ( this->calcDiffuseFromAttenByDistance( cLightHelper::DEFAULTINFINITEDISTANCE, 
		                                       constAttenuation, linearAttenuation, quadraticAttenuation,
	                                           accuracy ) > targetLightLevelHigh )
	{	// Yes, so we can never get down to this light level
		return cLightHelper::DEFAULTINFINITEDISTANCE;
	}

	// There is a light level somewhere between a distance of 0.0 to DEFAULTINFINITEDISTANCE
	// Set initial guesses
	float distanceGuessLow = 0.0f;
	float distanceGuessHigh = cLightHelper::DEFAULTINFINITEDISTANCE;

	unsigned int iterationCount = 0;
	while ( iterationCount < maxIterations )
	{
		// Pick a distance between the high and low
		float curDistanceGuess = ( (distanceGuessHigh - distanceGuessLow) / 2.0f ) + distanceGuessLow;
		// 
		float curDiffuseAtGuessDistance 
			= this->calcDiffuseFromAttenByDistance( curDistanceGuess, constAttenuation, 
													linearAttenuation, quadraticAttenuation, DEFAULTZEROTHRESHOLD );
		// Could be three possibilities: too low, too high, or in between
		if ( curDiffuseAtGuessDistance < targetLightLevelLow )
		{	// Light is too dark, so distance is to HIGH. Reduce and guess again.
			distanceGuessHigh = curDistanceGuess;		// Lower the high limit for the guesses
		}
		else if ( curDiffuseAtGuessDistance > targetLightLevelHigh )
		{	// Light is too bright, so distance is to LOW. Increase and guess again
			distanceGuessLow = curDistanceGuess;
		}
		else 
		{	// Nailed it! Light level is within range, so return this distance
			return curDistanceGuess;
		}

		iterationCount++;

	}// while ( iterationCount < maxIterations )
	// If we are here, then we ran out of iterations.
	// Pick a distance between the low and high
	float distance = (distanceGuessHigh - distanceGuessLow) / 2.0f;

	return distance;
}

//static 
const float cLightHelper::DEFAULTZEROTHRESHOLD = 0.0001f;

float cLightHelper::calcDiffuseFromAttenByDistance(
		float distance, 
		float constAttenuation, 
		float linearAttenuation, 
		float quadraticAttenuation,
		float zeroThreshold /*= DEFAULTZEROTHRESHOLD*/)
{
	float diffuse = 1.0f;		// Assume full brightness

	float denominator = constAttenuation + 
	                    linearAttenuation * distance +
						quadraticAttenuation * distance * distance;
	// Is this zero (we don't want a divide by zero, do we)?
	if ( denominator <= zeroThreshold )
	{	// Let's just say it's zero, shall we?
		diffuse = 1.0f;
	}
	else
	{
		float atten = 1.0f / denominator;
		diffuse *= atten;
		if ( diffuse > 1.0f )
		{
			diffuse = 1.0f;
		}
	}//if ( denominator <= zeroThreshold )
	return diffuse;
}