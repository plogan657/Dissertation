#pragma once
class NoiseSimulation
{
public:
	NoiseSimulation();
	~NoiseSimulation();

	void createWhiteNoise(float* x, int nSamples);
	float* createNoiseSim(short* noiseSample, short* speechSample, int nSamples, float snr);
};

