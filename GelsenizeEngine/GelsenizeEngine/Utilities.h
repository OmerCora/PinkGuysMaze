#ifndef _Utilities_HG_
#define _Utilities_HG_

#include <vector>

float getRandFloat( float LO, float HI );

float calcApproxDistFromAtten(float targetLightLevel, 
							  float attenConst, 
							  float attenLinear,
							  float attenQuad,
							  float accuracy, 
                              float infiniteDistance, 
							  unsigned int maxIterations = 50 /*DEFAULTMAXITERATIONS*/ );

float calcDiffuseFromAttenByDistance(float distance, 
									 float attenConst, 
									 float attenLinear, 
									 float attenQuad, 
									 float zeroThreshold =0.0001f /*DEFAULTZEROTHRESHOLD*/);

// Basic low pass filer
class cAverageSample
{
public:
	static const unsigned int DEFAULTNUMBERSAMPLES; // = 20;
	cAverageSample();
	cAverageSample(unsigned int numberSamples);
	void SetResetSamples(unsigned int numberSamples, bool bKeepOld = true);
	void AddSample(double sample);
	double getAverage(void);
private:
	std::vector< double > m_vecSamples;
	unsigned int m_nextIndex;
	// These are used to counter the "ramp up" of the filter, 
	//	where the initial values are loaded and the filter 
	//	slowly stabilizes. 
	bool m_bIsFirstSample;
	bool m_bIsFirstFullPass;
	// Gets average but only up to certain index
	double m_getAverage(unsigned int maxIndex);
};

#endif
