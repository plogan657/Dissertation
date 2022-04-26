#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

#include "Wav.h"
#include <iostream>
#include "doctest.h"
#pragma warning(disable : 4996).
using namespace std;

Wav::Wav(int id) : x(id)
{
	sample = 0;
}

Wav::~Wav()
{
	cout << "Destructor called "  << x << endl;
	delete[] sample;
}

bool Wav::read(char* fname)
{
	ifstream fin(fname, ios::binary);
	if (!fin) return 0;

	// read RIFF header
	if (!header.read(fin)) return 0;

	// number of samples & sample array
	nSamples = header.get_data_size() / header.get_bytes_samp();
	sample = new short int[nSamples];

	// read wav data
	fin.read((char*)sample, nSamples * sizeof(short int));
	fin.close();
	return 1;
}

void Wav::save(char* fname)
{
	ofstream fout(fname, ios::binary);

	// write RIFF header
	header.write(fout);

	// write wav data
	fout.write((char*)sample, nSamples * sizeof(short int));
	fout.close();
}

// TESTS
TEST_CASE("testing Wav") {
	Wav wav(7);
	short int* x = 0;
	CHECK(wav.get_sample() == x);
}
//TEST_CASE("testing Wav read") 
//{
//	Wav wav(8);
//	string input = "_SI633.wav";
//	char* x = new char[100];
//	strcpy(x, input.c_str());
//	wav.read(x);
//	CHECK(wav.get_nSamples() == 78848);
//}
