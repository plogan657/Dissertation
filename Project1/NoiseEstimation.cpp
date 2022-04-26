#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG



#include"NoiseEstimation.h"
#include"STFT.h"
#include "doctest.h"
#include <iostream>
using namespace std;

NoiseEstimation::NoiseEstimation(STFT &stft)
{
	specMag = stft.getSpecMag();
	nFFT2 = stft.getNFFT2();
}

NoiseEstimation::~NoiseEstimation()
{

}


float* NoiseEstimation::calcEstimation(int startFrame, int endFrame)
{
	float sum = 0, average = 0;
	float* averages = new float[nFFT2 + 1];
	float* noiseEstFrames = new float[endFrame - startFrame];
	float* noiseEst = new float[nFFT2 + 1];

	for (int i = 0; i < endFrame; i++)
	{
		sum = 0;
		for (int j = 0; j <= nFFT2; j++)
		{
			sum += specMag[i][j];
		}
		noiseEstFrames[i] = sum / (endFrame - startFrame);
	}
	sum = 0;
	for(int i = 0; i < endFrame; i++)
	{
		sum += noiseEstFrames[i];
	}
	sum = sum / (endFrame - startFrame);
	for (int i = 0; i <= nFFT2; i++)
	{
		noiseEst[i] = sum;
	}
	delete[] averages;
	delete[] noiseEstFrames;

	return noiseEst;
}

// TESTS
TEST_CASE("Noise Estimation")
{
	Wav wav(1);
	short* x =  new short[10];
	for (int i = 0; i < 10; i++)
	{
		x[i] = 50;
	}
	wav.sample = x;
	wav.nSamples = 10;
	STFT stft2(wav, 2);
	NoiseEstimation noiseEstimation(stft2);
	float* noise = noiseEstimation.calcEstimation(0, 2);

	CHECK(noise[0] == doctest::Approx(4.0f));

	delete[] noise;
}
