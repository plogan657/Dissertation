#ifndef WAVIO_H
#define WAVIO_H

#include "RIFF.h"

class Wav {
public:
	Wav(int id);
	~Wav();

	bool read(char* fname);
	void save(char* fname);

	int get_nSamples() const { return nSamples; }
	short int* get_sample() const { return sample; }

	// The WAV samples
	unsigned int nSamples;
	short int* sample;
	int x;

	// The WAV header
	RIFF header;


private:
	
	
	
};
#endif