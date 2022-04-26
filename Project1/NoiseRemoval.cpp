#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG


#include "NoiseRemoval.h"
#include "Wav.h"
#include <iostream>
#include "doctest.h"
using namespace std;

NoiseRemoval::NoiseRemoval()
{

}

NoiseRemoval::~NoiseRemoval()
{

}

void NoiseRemoval::SpecSub(
	float** ob, // magnitude spectrum (power spectrum)
	float* no, // noise estimate (average estimation of noise)
	float** es, // estimation 
	int len, // number of frames
	int dim, // size of frame
	float alpha // parameter usually 1.1?
)
{
	for (int i = 0; i < len; i++)
	{
		for (int j = 0; j < dim; j++)
		{
			float s = (ob[i][j] * ob[i][j]) - alpha * (no[j] * no[j]);
 			if (s < 0.) s = 0;
			es[i][j] = sqrt(s);
			// Not sure about this next line
			ob[i][j] = es[i][j];
		}
	}
}

void NoiseRemoval::weinerFilter(
	float** ob, // magnitude spectrum
	float* no, // noise estimate
	float** es, // estimation
	int len, // number of frames
	int dim, // size of frame
	//float snr_min, // minimum db for snr???
	float alpha // parameter
)
{
	float snr_min = -10;
	// snr floor db -> ratio
	snr_min = pow(10., snr_min / 10.);
	// over subtract factor
	float beta = 1.1f;

	float r = 1.F;

	// for each freq index k
	for (int k = 0; k < dim; k++)
	{
		// for each frame t
		for (int t = 0; t < len; t++)
		{
			// input periodgram
			float o = ob[t][k] * ob[t][k];

			// noise psd estimate
			float n = no[k] * no[k];

			// snr estimation
			float snr = -1.F;
			if (n > 0.)
			{
				// spectral subtraction
				float s = o - beta * n;

				// priori SNR estimate at t
				float f = s / n; if (f < 0.)f = 0;

				// smoothed snr estimate at t
				snr = alpha * r + (1.F - alpha) * f;
			}
			if (snr < snr_min) snr = snr_min;
			
			// filter weight h & filtering
			float h = snr / (1.F + snr);
			float s = h * ob[t][k];
			es[t][k] = s;
			ob[t][k] = es[t][k];

			// a posteriori snr estimate at t
			r = 1.F;
			if (n > 0.F) r = (s * s) / n;
		}
	}
}

// TESTS
TEST_CASE("NoiseRemoval")
{
	NoiseRemoval noiseRemoval;

	float** ob = new float* [1];
	ob[0] = new float[10];

	for (int i = 0; i < 1; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			ob[i][j] = 100;
		}
	}
	float* no{ new float[10]{10,20,30,40,50,60,70,80,90,100} };

	float** es = new float* [1];
	es[0] = new float[10];
	int len = 1;
	int dim = 10;
	float alpha = 1.1f;

	SUBCASE("Spectral Subtraction")
	{
		noiseRemoval.SpecSub(ob, no, es, len, dim, alpha);
		CHECK(es[0][0] == 99.44848f);
		CHECK(es[0][5] == 77.71744f);
		CHECK(es[0][9] == 0.0f);

		delete[] ob[0], es[0];
		delete[] ob, es, no;
	}
	SUBCASE("Wiener Filter")
	{
		noiseRemoval.weinerFilter(ob,no,es, len, dim, -10);
		CHECK(es[0][0] == doctest::Approx(99.90732f));
		CHECK(es[0][1] == doctest::Approx(9.09091f));
		CHECK(es[0][9] == doctest::Approx(9.09091f));

		delete[] ob[0], es[0];
		delete[] ob, es, no;
	}
}