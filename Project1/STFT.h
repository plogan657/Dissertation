#ifndef STFT_H
#define STFT_H

#include "Wav.h"

class STFT {
public:
	STFT(Wav &wav, int frameLength);
	~STFT();

	void splitFrames();
	void performFT(float* x);
	void calcSpectrumMag();
	void printSpecMag();
	short int* getSample() { return sample; }
	int get_count() { return count; }
	float** getSpecMag() { return specMag; }
	int getNFFT2() { return nFFT2; }
	
	// Inverse
	void Pha(float* p);
	void calcInverseSTFT();
	void inverseSTFT(float* x);
	void inverseSTFT2(float* a, float* p, float* x);
	void reverseSplit(float* x);

	int nFrames;
	int nFFT;
	int nFFT2;

	short int* sample;
private:
	
	int fileSize;
	float* HamWin;
	float* re;
	float* im;
	float* spec;
	float** p;
	float** masterRE;
	float** masterIM;
	float** specMag;
	int count;
	int count2;
	int count3 = 0;

};

#endif
