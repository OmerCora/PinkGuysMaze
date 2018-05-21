#include "Utilities.h"

#include <stdlib.h>     // srand, rand 

float calcApproxDistFromAtten(float targetLightLevel, 
							  float attenConst, 
							  float attenLinear,
							  float attenQuad,
							  float accuracy, 
                              float infiniteDistance, 
							  unsigned int maxIterations /*= DEFAULTMAXITERATIONS = 50*/ )
{
	const float DEFAULTINFINITEDISTANCE = 10000.0f;	
//static 
	const float DEFAULDIFFUSEACCURACYTHRESHOLD = 0.001f;

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
	//if ( calcDiffuseFromAttenByDistance( DEFAULTINFINITEDISTANCE, accuracy ) > targetLightLevelHigh )
	float diffuseCalculated = calcDiffuseFromAttenByDistance( DEFAULTINFINITEDISTANCE, 
															  attenConst, attenLinear, attenQuad );
															  
	if ( diffuseCalculated > targetLightLevelHigh )
	{	// Yes, so we can never get down to this light level
		return DEFAULTINFINITEDISTANCE;
	}

	// There is a light level somewhere between a distance of 0.0 to DEFAULTINFINITEDISTANCE
	// Set initial guesses
	float distanceGuessLow = 0.0f;
	float distanceGuessHigh = DEFAULTINFINITEDISTANCE;

	unsigned int iterationCount = 0;
	while ( iterationCount < maxIterations )
	{
		// Pick a distance between the high and low
		float curDistanceGuess = ( (distanceGuessHigh - distanceGuessLow) / 2.0f ) + distanceGuessLow;
		// 
		float curDiffuseAtGuessDistance = calcDiffuseFromAttenByDistance( curDistanceGuess, 
																		  attenConst, 
																		  attenLinear, 
																		  attenQuad );
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

//const float CLightDescription::DEFAULTZEROTHRESHOLD = 0.0001f;

float calcDiffuseFromAttenByDistance(float distance, 
									 float attenConst, 
									 float attenLinear, 
									 float attenQuad, 
									 float zeroThreshold /*=0.0001f  DEFAULTZEROTHRESHOLD*/)
{
	const float DEFAULTZEROTHRESHOLD = 0.0001f;

	float diffuse = 1.0f;		// Assume full brightness

	float denominator = attenConst + 
	                    attenLinear * distance +
						attenQuad * distance * distance;
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



/*static*/ 
const unsigned int cAverageSample::DEFAULTNUMBERSAMPLES = 20;

cAverageSample::cAverageSample()
{
	this->SetResetSamples( cAverageSample::DEFAULTNUMBERSAMPLES, false );
	return;
}

cAverageSample::cAverageSample(unsigned int numberSamples)
{
	if ( numberSamples == 0 )	
	{	// Zero samples? Seriously? What are you doing, man...?
		numberSamples = cAverageSample::DEFAULTNUMBERSAMPLES; 
	}

	this->SetResetSamples( numberSamples, false );
	return;
}

void cAverageSample::SetResetSamples(unsigned int numberSamples, bool bKeepOld /*=true*/)
{
	// Keeping the old samples
	if ( bKeepOld )
	{	// Copy old samples
		std::vector< double > oldSamples;
		oldSamples = this->m_vecSamples;
		// Copy them back, setting value with average
		double average = this->getAverage();

		this->m_vecSamples.clear();
		this->m_vecSamples.reserve( numberSamples );
		// Copy back
		for ( int index = 0; index != numberSamples; index++ )
		{
			if ( index < static_cast<int>( oldSamples.size() ) )
			{	// Copy actual old value back
				this->m_vecSamples[index] = oldSamples[index];
			}
			else
			{	// Use the old average (as there is no "old" sample at this index)
				this->m_vecSamples[index] = average;
			}
		}//for ( int index = 0; index != numberSamples; index++ )
		this->m_nextIndex = 0;
	}//if ( bKeepOld )
	else
	{	// Brand new, or we don't care about the old samples
		this->m_vecSamples.clear();
		this->m_vecSamples.reserve( numberSamples );
		for ( int index = 0; index != numberSamples; index++ )
		{
			this->m_vecSamples.push_back( 0.0f );
		}
		// Indicate we haven't taken a sample, yet
		this->m_bIsFirstSample = true;
		this->m_bIsFirstFullPass = true;
		this->m_nextIndex = 0;
	}//if ( bKeepOld )

	return;
}

void cAverageSample::AddSample(double sample)
{
	if ( ! this->m_bIsFirstFullPass )
	{	// This is the "normal" state, were we simply place the value in vector
		this->m_vecSamples[ this->m_nextIndex ] = sample;
		this->m_nextIndex++;
		if ( this->m_nextIndex >= this->m_vecSamples.size() )
		{	// Index too far, so reset to start
			this->m_nextIndex = 0;
		}
	}
	else
	{	// One of a couple special cases, designed to limit the 
		//	"ramp up" of the filter.

		// If this is the 1st sample, then fill entire vector with 1st sample.
		if ( this->m_bIsFirstSample )
		{
			for ( int index = 0; index != this->m_vecSamples.size(); index++ )
			{
				this->m_vecSamples[index] = sample;
			}
			this->m_bIsFirstSample = false;
			this->m_nextIndex++;	// Don't need to check as this is the 1st sample
		}
		else
		{	// NOT first sample, but are we still loading initial values?
			// Must be YES, as we tested this earlier (m_bIsFirstFullPass)
			//
			// So, it's NOT the 1st sample, but we haven't actually filled the 
			//	entire filter one time, yet.
			// 
			// Add this sample, but fill rest of entries with the average 
			this->m_vecSamples[ this->m_nextIndex ] = sample;
			this->m_nextIndex++;
			// Get average up to this sample
			double averageUpToIndex = this->m_getAverage( this->m_nextIndex );
			// Load rest of buffer with this average value
			for ( int index = this->m_nextIndex; index != this->m_vecSamples.size(); index++ )
			{
				this->m_vecSamples[index] = averageUpToIndex;
			}
			// Index too high?
			if ( this->m_nextIndex >= this->m_vecSamples.size() )
			{	// Yup, so reset and indicate we have completed an entire 1st pass
				this->m_nextIndex = 0;
				this->m_bIsFirstFullPass = false;
			}

		}//if ( this->m_bIsFirstSample )


	}//if ( ! this->m_bIsFirstFullPass )


	return;
}

// Gets average but only up to certain index
double cAverageSample::m_getAverage(unsigned int maxIndex)
{
	if ( maxIndex == 0 ) 
	{	// Seriously? What are you doing? 
		return 0.0; // Pointless average of zero (0) samples
	}

	double total = 0.0;
	for ( int index = 0; index != maxIndex; index++ )
	{
		total += this->m_vecSamples[index];
	}
	return total / static_cast<double>(maxIndex - 1);
}


double cAverageSample::getAverage(void)
{
	return this->m_getAverage( this->m_vecSamples.size() );
}


