#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

#include"Evaluation.h"
#include"Wav.h"
#include "doctest.h"
#include <iostream>
#include <math.h>
using namespace std;

Evaluation::Evaluation()
{

}

Evaluation::~Evaluation()
{

}

void Evaluation::SSNR(Wav &original, Wav &noisy, Wav &enhanced, int fileSize, int nFFT, float &noisySNR, float &enhancedSNR)
{
	int nFFT2 = nFFT / 2;
	int nFrames = (fileSize - nFFT) / nFFT2 + 1;
	float** oSample = new float* [nFrames];
	float** nSample = new float* [nFrames];
	float** eSample = new float* [nFrames];

	for (int i = 0; i < nFrames; i++)
	{
		oSample[i] = new float[nFFT];
		nSample[i] = new float[nFFT];
		eSample[i] = new float[nFFT];
	}

	// Split Frames
	float* frame = new float[nFFT];
	int start = 0;
	int end = nFFT;
	int k = 0;
	int count = 0;
	int m = 0;

	// Calculate frames
	while (count < nFrames)
	{
		for (int i = start; i < end; i++)
		{
			oSample[count][k] = original.sample[i];
			nSample[count][k] = noisy.sample[i];
			eSample[count][k] = enhanced.sample[i];
			k++;
		}
		// Update values for next frame
		start = end - nFFT2;
		end = start + nFFT;
		k = 0;
		if (end > fileSize) {
			end = fileSize;
		}
		count++;
	}

	float cleanSum, powerN, powerN2, snrFrame;
	//float noisySNR = 0, enhancedSNR = 0;
	for (int i = 0; i < nFrames; i++)
	{
		powerN = 0;
		powerN2 = 0;
		cleanSum = 0;
		for (int j = 0; j < nFFT; j++)
		{
			// Calculate sum n^2
			float x = nSample[i][j] - oSample[i][j];
			powerN += x * x;

			//Calculate sum n2^2
			float z = eSample[i][j] - oSample[i][j];
			powerN2 += z * z;

			//Calculate power sum of clean
			cleanSum += oSample[i][j] * oSample[i][j];
		}
		// Calculate SNR of frame & add to sum
		noisySNR += 10 * log10f(cleanSum / powerN);
		enhancedSNR += 10 * log10f(cleanSum / powerN2);
	}
	noisySNR = noisySNR / nFrames;
	enhancedSNR = enhancedSNR / nFrames;
	cout << "The Segmental SNR of the noisy file is " << noisySNR<< endl;
	cout << "The Segmental SNR of the enhanced file is " << enhancedSNR << endl;

	// Free Memory
	for (int i = 0; i < nFrames; i++)
	{
		delete[] oSample[i];
		delete[] nSample[i];
		delete[] eSample[i];
	}
	delete[] frame;
	delete[] oSample;
	delete[] nSample;
	delete[] eSample;
}

// TESTS
TEST_CASE("Evaluation")
{
	Evaluation evaluation;
	float noisySNR = 0;
	float enhancedSNR = 0;

	Wav original(1);
	short int* o = new short int[10]{100, 100, 100, 100 , 100, 100, 100, 100, 100, 100};
	original.sample = o;
	original.nSamples = 10;

	Wav noisy(2);
	short int* n = new short int[10]{ 50, 50, 50, 50 , 50, 50, 50, 50, 50, 50 };
	noisy.sample = n;
	noisy.nSamples = 10;
	
	Wav enhanced(3);
	short int* e = new short int[10]{ 30, 30, 30, 30 , 30, 30, 30, 30, 30, 30};
	enhanced.sample = e;
	enhanced.nSamples = 10;

	evaluation.SSNR(original, noisy, enhanced, 10, 5, noisySNR, enhancedSNR);

	CHECK(noisySNR == doctest::Approx(48.24762f));
	CHECK(enhancedSNR == doctest::Approx(45.32506f));
}