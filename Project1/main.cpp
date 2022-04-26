#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG


#include <iostream>
#include "Wav.h"
#include "STFT.h"
#include "NoiseEstimation.h"
#include "NoiseRemoval.h"
#include "Evaluation.h"

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include "NoiseSimulation.h"

using namespace std;

#define FILE_NAME_LEN 100

int main(int argc, char** argv)
{
	// Comment to not run tests
	doctest::Context ctx;
	ctx.setOption("abort-after", 5);  // default - stop after 5 failed asserts
	ctx.applyCommandLine(argc, argv); // apply command line - argc / argv
	ctx.setOption("no-breaks", true); // override - don't break in the debugger
	int res = ctx.run();              // run test cases unless with --no-run
	if (ctx.shouldExit())              // query flags (and --exit) rely on this
		return res;                   // propagate the result of the tests


	// your code goes here
	
	//_CrtDumpMemoryLeaks();

	cout << "Please select an option" << endl;
	cout << "1. Perform Noise Reduction On Noisy Audio" << endl;
	cout << "2. Perform Evaluation of Noise Reduction" << endl;

	int option;
	cin >> option;

	if (option == 1)
	{
		cout << "Give wav file name to read: ";
		char* fname = new char[FILE_NAME_LEN];
		cin >> fname;

	

		// read the wav file
		Wav wav(1);

		if (!wav.read(fname)) {
			cout << "Invalid wav file: " << fname << endl;
			return 0;
		}

		delete[] fname;
		cout << "Please input a frame size" << endl;
		int frameSize;
		cin >> frameSize;
		

		// Short-Time Fourier Transform
		STFT stft(wav, frameSize);

		// Noise estimation
		cout << "Noise Estimation Started" << endl;
		NoiseEstimation noiseEst(stft);
		float* noise = noiseEst.calcEstimation(0, 20);
		cout << noise[0];
		cout << "Noise Estimation Finished" << endl;

		// NoiseRemoval
		NoiseRemoval noiseRemoval;
		float** es = new float* [stft.nFrames];

		for (int i = 0; i < stft.nFrames; i++)
		{
			es[i] = new float[stft.nFFT2 + 1];
		}

		// Spectral Subtraction
		cout << "Spectral Subtraction Started" << endl;
		noiseRemoval.SpecSub(stft.getSpecMag(), noise, es, stft.nFrames, stft.nFFT2, 1.1);
		cout << "Spectral Subtraction Ended" << endl;

		// Wiener Filtering
		cout << "Wiener Filtering Started" << endl;
		noiseRemoval.weinerFilter(stft.getSpecMag(), noise, es, stft.nFrames, stft.nFFT2, 0.98);
		cout << "Wiener Filtering Ended" << endl;

		// Inverse STFT
		cout << "Inverse STFT Started" << endl;
		stft.calcInverseSTFT();
		cout << "Inverse STFT Ended" << endl;

		// save to a new wav file
		/*cout << "Give wav file name to save: ";
		cin >> fname;
		wav.save(fname);*/

		// Delete
		delete[] noise;
		for (int i = 0; i < stft.nFrames; i++) delete[] es[i];
		delete[] es;
	}

	// Noise Reduction Evaluation
	else if (option == 2)
	{
		char* speechInput = new char[FILE_NAME_LEN];
		char* noiseInput = new char[FILE_NAME_LEN];
		int fileSize = 0;
		int frameSize = 0;
		int snr = 0;


		Wav cleanSpeechWav(2);
		Wav noiseWav(3);

		cout << "Please enter the speech file" << endl;
		cin >> speechInput;
		if (!cleanSpeechWav.read(speechInput)) {
			cout << "Invalid wav file: " << speechInput << endl;
			return 0;
		}

		cout << "Please enter the frame size" << endl;
		cin >> frameSize;

		//set fileSize
		fileSize = cleanSpeechWav.nSamples;

		// NoiseSimulation
		NoiseSimulation noiseSim;

	
		char wnoise;
		cout << "Do you wish to use white noise?" << endl;
		cout << "Enter y for yes, n for no" << endl;
		cin >> wnoise;


		// White Noise
		if (wnoise == 'y') 
		{
			cout << "Please Enter The SNR value" << endl;
			cin >> snr;

			//Create White Noise Sample
			float* whiteNoise = new float[fileSize];
			noiseSim.createWhiteNoise(whiteNoise, fileSize);
			noiseWav.nSamples = fileSize;
			noiseWav.sample = new short[fileSize];
			for (int i = 0; i < fileSize; i++) noiseWav.sample[i] = whiteNoise[i];
			delete[] whiteNoise;
		}
		// Coloured Noise
		else {
			cout << "Please enter the noise file" << endl;
			cin >> noiseInput;
			if (!noiseWav.read(noiseInput)) {
				cout << "Invalid wav file: " << noiseInput << endl;
				return 0;
			}

			//int snr;
			cout << "Please enter the SNR value" << endl;
			cin >> snr;

			//Ensure Noise is same length as speech
			if (noiseWav.nSamples < cleanSpeechWav.nSamples)
			{
				int count = 0;
				int oldFileSize = noiseWav.nSamples;
				short int* temp = new short int[noiseWav.nSamples];
				for (int i = 0; i < noiseWav.nSamples; i++) temp[i] = noiseWav.sample[i];
				noiseWav.nSamples = fileSize;
				noiseWav.sample = new int short[fileSize];
				for (int i = 0; i < fileSize; i++)
				{
					noiseWav.sample[i] = temp[count];
					count++;
					if (count > oldFileSize) count = 0;
				}
				delete[] temp;
			}
		}

		// Make Wav file to add noise to
		float* noisySpeech = noiseSim.createNoiseSim(noiseWav.sample, cleanSpeechWav.sample, fileSize, snr);
		Wav noisyWav(6);
		noisyWav.sample = new short int[fileSize];
		noisyWav.nSamples = fileSize;
		for (int i = 0; i < fileSize; i++)
		{
			noisyWav.sample[i] = noisySpeech[i];
		}
		
		// Make Wav file which will be enhanced
		Wav enhancedWav(4);
		enhancedWav.sample = new short int[fileSize];
		enhancedWav.nSamples = fileSize;
		enhancedWav.header = cleanSpeechWav.header;
		for (int i = 0; i < fileSize; i++)
		{
			enhancedWav.sample[i] = noisySpeech[i];
		}

		// STFT
		STFT stft(enhancedWav, frameSize);

		// Save simulated noisy file
		char* noisy = new char[FILE_NAME_LEN];
		cout << "Give Noisy wav file name to save: ";
		cin >> noisy;
		enhancedWav.save(noisy);

		delete[] noisy;

		// Noise estimation
		cout << "Noise Estimation Started" << endl;
		NoiseEstimation noiseEst(stft);
		float* noise = noiseEst.calcEstimation(0, 20);
		cout << noise[0];
		cout << "Noise Estimation Finished" << endl;

		// NoiseRemoval
		NoiseRemoval noiseRemoval;
		float** es = new float* [stft.nFrames];
		for (int i = 0; i < stft.nFrames; i++)
		{
			es[i] = new float[stft.nFFT2 + 1];
		}

		// Spectral Subtraction
		cout << "Spectral Subtraction Started" << endl;
		noiseRemoval.SpecSub(stft.getSpecMag(), noise, es, stft.nFrames, stft.nFFT2, 1.1);
		cout << "Spectral Subtraction Ended" << endl;

		// Wiener Filtering
		cout << "Wiener Filtering Started" << endl;
		noiseRemoval.weinerFilter(stft.getSpecMag(), noise, es, stft.nFrames, stft.nFFT2, 0.98);
		cout << "Wiener Filtering Ended" << endl;

		// Inverse STFT
		cout << "Inverse STFT Started" << endl;
		stft.calcInverseSTFT();
		cout << "Inverse STFT Ended" << endl;
		
		// Evaluation of noise reduction
		Evaluation evaluation;
		float noisySNR, enhancedSNR = 0;
		evaluation.SSNR(cleanSpeechWav, noisyWav, enhancedWav, cleanSpeechWav.get_nSamples(), frameSize, noisySNR, enhancedSNR);

		// Save enhanced audio
		char* enhancedName= new char[FILE_NAME_LEN];
		cout << "Give enhanced wav file name to save: ";
		cin >> enhancedName;
		enhancedWav.save(enhancedName);

		// delete
		for (int i = 0; i < stft.nFrames; i++) delete[] es[i];
		delete[] speechInput; 
		delete[] noiseInput; 
		delete[] noisySpeech; 
		delete[] enhancedName;
		delete[] es;
		delete[] noise;
	}

	
	return 0;
}
