#ifndef EVALUATION_H
#define EVALUATION_H
#include"Wav.h"

class Evaluation {
public:
	Evaluation();
	~Evaluation();
	void SSNR(
		Wav &original, // original clean speech
		Wav &noisy, // noisy speech
		Wav &enhanced, // enhanced speech
		int fileize, // size of file
		int nFFT, // frame size
		float &noisySNR, // variable to hold SNR value
		float &enhancedSNR // variable to hold SNR value
	);

private:
};

#endif