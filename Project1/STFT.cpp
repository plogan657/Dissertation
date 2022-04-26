#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG



#include "STFT.h"
#include <iostream>
#include <math.h>
#include"Wav.h"
#include "doctest.h"
#pragma warning(disable : 4996);
using namespace std;

constexpr auto PI = 3.1415926535897932384626433832795F;;

STFT::STFT(Wav& wav, int frameLength)
{
	sample = wav.get_sample(); // wav sample data
	fileSize = wav.get_nSamples(); // size of test.wav = 214599
	//sample = new short int [fileSize];
	nFFT = frameLength; // length of frame test = 512
	nFFT2 = nFFT / 2; // length of half a frame
	nFrames = (fileSize - nFFT) / (nFFT2 + 1); // number of frames to be analysed
	HamWin = new float[nFFT]; // Array to hold Hamming Window calculation
	re = new float[nFFT2 + 1]; // array for the real numbers
	im = new float[nFFT2 + 1]; // array for imaginary numbers
	p = new float* [nFrames]; // array to hold Phase
	masterRE = new float* [nFrames]; // array of re arrays
	masterIM = new float* [nFrames]; // array of im arrays
	specMag = new float* [nFrames];// array to hold spectrum magnitude

	count2 = 0;

	// Initialise Arrays
	for (int i = 0; i < nFrames; i++)
	{
		p[i] = new float[nFFT2 + 1];
		masterRE[i] = new float[nFFT2 + 1];
		masterIM[i] = new float[nFFT2 + 1];
		specMag[i] = new float[nFFT2 + 1];
	}
	count = 0;
	cout << "FileSize = " << fileSize << endl;
	cout << "nFrames = " << nFrames << endl;
	cout << "nFFT = " << nFFT << endl;
	cout << "nFFT2 = " << nFFT2 << endl;
	cout << "Loading" << endl;

	// Split file into frames and apply hamming window and apply Fourier Transform
	splitFrames();
	// Calculate Fourier Transform for each frame
	calcSpectrumMag();
}

STFT::~STFT()
{
	delete[] re;
	delete[] im;
	delete[] HamWin;

	for (int i = 0; i < nFrames; i++)
	{
		delete[] p[i];
		delete[] masterRE[i];
		delete[] masterIM[i];
		delete[] specMag[i];
	}
	delete[] p;
	delete[] masterRE;
	delete[] masterIM;
	delete[] specMag;
}

void STFT::printSpecMag()
{
	for (int i = 0; i < 3 /*nFrames*/; i++)
	{
		cout << "-------------------- Frame " << i << " -----------------------" << endl;
		for (int j = 0; j <= nFFT2; j++)
		{
			cout << specMag[i][j] << " ";
		}
		cout << " " << endl;
	}
}

void STFT::calcSpectrumMag()
{
	cout << "Starting Spectrum Calc" << endl;
	for (int n = 0; n < nFrames; n++)
	{
		// Iterate through real and imaginary numbers and calculate final spectral Magnitude of each frame
		for (int j = 0; j <= nFFT2; j++)
		{
			specMag[n][j] = (float)sqrt(pow(masterRE[n][j], 2.0) + pow(masterIM[n][j], 2.0));
		}
	}
	cout << "Finished Spectrum Calc" << endl;
}

void STFT::splitFrames()
{
	// Variables for calculating frames
	float* frame = new float[nFFT];
	int start = 0;
	int end = nFFT;
	int k = 0;

	// Calculate Hamming Window
	for (int n = 0; n < nFFT; n++)
	{
		HamWin[n] = 0.54F - 0.46F * (float)cos(2 * n * PI / (nFFT - 1));
	}

	// Calculate frames
	while (count < nFrames)
	{
		for (int i = start; i < end; i++)
		{
			frame[k] = sample[i];
			k++;
		}

		// Update values for next frame
		start = end - nFFT2;
		end = start + nFFT;
		k = 0;
		if (end > fileSize) {
			cout << "End > file size: " << end << endl;
			end = fileSize;
		}

		// Hamming Window Conversion
		for (int n = 0; n < nFFT; n++) frame[n] *= HamWin[n];

		// Perform Fourier Transform on frame
		performFT(frame);
		Pha(p[count]);
		count++;
	}
	delete[] frame;
}

void STFT::performFT(float* x)
{
	// Calculating each part of the complex numbers
	// real part
	re[0] = 0.;
	for (int n = 0; n < nFFT; n++)
	{
		re[0] += x[n];
	}

	re[nFFT2] = 0.;

	for (int n = 0; n < nFFT; n++)
	{
		float a = (n % 2) ? -x[n] : x[n];
		re[nFFT] += a;
	}

	for (int k = 1; k <= nFFT2; k++)
	{
		re[k] = 0.;
		for (int n = 0; n < nFFT; n++)
		{
			re[k] += x[n] * cos(2 * PI * k * n / nFFT);
		}
	}

	// Imaginary Part
	im[0] = im[nFFT2] = 0.;
	for (int k = 1; k <= nFFT2; k++)
	{
		im[k] = 0.;
		for (int n = 0; n < nFFT; n++)
		{
			im[k] += x[n] * sin(2 * PI * k * n / nFFT);
		}
	}
	// Store Numbers
	for (int i = 0; i <= nFFT2; i++)
	{
		masterRE[count][i] = re[i];
		masterIM[count][i] = im[i];
	}

	//count++;
}

void STFT::Pha(float* p)
{
	float ph;
	for (int k = 0; k <= nFFT2; k++) {
		if (re[k] == 0.)
		{
			ph = (im[k] >= 0.F) ? PI / 2.F : -PI / 2.F;
		}
		else
		{
			ph = atan(im[k] / re[k]);
			if (re[k] < 0. && im[k] < 0.)
			{
				ph -= PI;
			}
			else if (re[k] < 0. && im[k] >= 0.)
			{
				ph += PI;
			}
		}
		p[k] = ph;
	}
}
void STFT::inverseSTFT(float* x)
{
	for (int n = 0; n < nFFT; n++) {
		float a = (n % 2) ? -re[nFFT2] : re[nFFT2];
		x[n] = re[0] + a;


		for (int k = 1; k < nFFT2; k++)
		{
			x[n] += re[k] * (float)cos((2 * PI * k * n) / nFFT) + im[k] * (float)sin((2 * PI * k * n) / nFFT);
		}


		for (int k = nFFT2 + 1; k < nFFT; k++)
		{
			int k1 = nFFT - k;
			int k2 = k - nFFT2;

			float a = (re[k1] * (float)cos((2 * PI * k2 * n) / nFFT)) - im[k1] * ((float)sin((2 * PI * k2 * n) / nFFT));

			if (n % 2) a = -a;
			x[n] += a;
		}
		x[n] /= nFFT;
	}
}

void STFT::inverseSTFT2(float* a, float* p, float* x)
{
	// a = mag spec
	// x space for reconstructed frame
	// do for every frame
	for (int k = 0; k <= nFFT2; k++) 
	{
		re[k] = a[k] * cos(p[k]);
		im[k] = a[k] * sin(p[k]);
	}
	inverseSTFT(x);
}

void STFT::calcInverseSTFT()
{
	float* w = new float[fileSize]; // Reconstructed wav for output
	float* x = new float[nFFT];

	for (int n = 0; n < fileSize; n++) w[n] = 0;

	for (int t = 0; t < nFrames; t++)
	{
		inverseSTFT2(specMag[t], p[t], x);

		// Inverse Hamming Window and overlap
		float* _w = w + t * nFFT2;
		for (int n = 0; n < nFFT; n++) 
		{
			_w[n] += x[n];
		} 
	}

	for (int i = 0; i < fileSize; i++)
	{
		sample[i] = w[i];
	}
	delete[] x;
	delete[] w;
}

// TESTS
TEST_CASE("STFT")
{
	Wav wav(6);
	short int* e = new short int[788];
	wav.sample = e;
	wav.nSamples = 788;
	STFT stft(wav, 50);
	SUBCASE("Constructor")
	{
		CHECK(stft.nFFT == 50);
		CHECK(stft.nFFT2 == 25);
		CHECK(stft.getSpecMag()[0][0] == 341065.5625f);
	}

	SUBCASE("Pha")
	{
		float* p = new float[256 + 1];

		stft.Pha(p);
		float x = p[0];

		CHECK(p[0] == doctest::Approx(3.14159f));
		delete[] p;
	}

	SUBCASE("Inverse STFT")
	{
		float* sample = new float[stft.nFFT];
		for (int i = 0; i < stft.nFFT; i++)
		{
			sample[0] = 1;
		}
		stft.inverseSTFT(sample);
		CHECK(sample[0] == doctest::Approx(-1028.09558f));
		delete[] sample;
	}
	
	SUBCASE("Inverse STFT2")
	{
		int nFFT = stft.nFFT;

		// Create Mock Phase
		float* p = new float [stft.nFFT2 + 1];
		for (int i = 0; i <= stft.nFFT2; i++)
		{
			p[i] = (float)1;
		}

		// Create mock x
		float* x = new float[nFFT];
		for (int i = 0; i < nFFT; i++)
		{
			x[i] = 1;
		}

		stft.inverseSTFT2(stft.getSpecMag()[0], p, x);

		CHECK(x[0] == doctest::Approx(7001.92236f));
		delete[] x;
		delete[] p;
	}
}


