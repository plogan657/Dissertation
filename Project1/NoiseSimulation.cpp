#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

#include "NoiseSimulation.h"
#include <iostream>
#include <math.h>
#include"Wav.h"
#include "doctest.h"
using namespace std;

NoiseSimulation::NoiseSimulation()
{

}

NoiseSimulation::~NoiseSimulation()
{

}

void NoiseSimulation::createWhiteNoise(float* x, int nSamples)
{
	// Generating White Noise
	for (int i = 0; i < nSamples; i++)
	{
		x[i] = rand() % 10001 - 5000;
	}
}

float* NoiseSimulation::createNoiseSim(short* noiseSample, short* speechSample, int nSamples, float snr)
{
	// Calculate Power of Samples
	float speechSum = 0;
	float noiseSum = 0;

	// Calculate Power of Speech
	for (int i = 0; i < nSamples; i++)
	{
		speechSum += pow(speechSample[i], 2);
	}
	float pwrSpeech = speechSum / nSamples;

	// Calculate Actual Power of Noise
	for (int i = 0; i < nSamples; i++)
	{
		noiseSum += pow(noiseSample[i], 2);
	}

	float actualPwrNoise = noiseSum / nSamples;

	// Calculate desired Power of noise
	float desiredPwrNoise = (pwrSpeech / pow(10, (snr / 10)));

	// Calculate a (varible)
	float a = sqrt(desiredPwrNoise / actualPwrNoise);

	// Calculate Noisy sample
	float* noisySample = new float[nSamples];
	for (int n = 0; n < nSamples; n++)
	{
		noisySample[n] = speechSample[n] + a * noiseSample[n];
	}

	return noisySample;
}

// TESTS
TEST_CASE("Noise Simulation")
{
	NoiseSimulation noiseSim;
	SUBCASE("createWhiteNoise")
	{
		float* x = new float[10];
		noiseSim.createWhiteNoise(x, 10);
		CHECK(x[0] < 5000);
		delete[] x;
	}

	SUBCASE("createNoiseSim")
	{
		short* noiseSample = new short[10];
		short* speechSample = new short[10];
		for (int i = 0; i < 10; i++)
		{
			noiseSample[i] = 5;
			speechSample[i] = 10;
		}
	
		float* noisy = noiseSim.createNoiseSim(noiseSample, speechSample, 10, 5);
		CHECK(noisy[0] == doctest::Approx(15.62341f));
		CHECK(noisy[9] == doctest::Approx(15.62341f));
	}
}