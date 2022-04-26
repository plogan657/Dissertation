#ifndef NoiseEstimation_H
#define NoiseEstimation_H

#include "STFT.h"

class NoiseEstimation {

public:
	NoiseEstimation(STFT& stft);
	~NoiseEstimation();

	float* calcEstimation(int startFrame, int endFrame);

private:
	float** specMag;
	int nFFT2;
};

#endif