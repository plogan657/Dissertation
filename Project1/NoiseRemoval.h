#ifndef NOISEREMOVAL_H
#define NOISEREMOVAL_H

#include "Wav.h"

class NoiseRemoval {

public:
	NoiseRemoval();
	~NoiseRemoval();

	void SpecSub(float** ob,
		float* no,
		float** es,
		int len,
		int dim,
		float alpha);

	void weinerFilter(float** ob,
		float* no,
		float** es,
		int len,
		int dim,
		//float snr_min,
		float alpha);
};

#endif