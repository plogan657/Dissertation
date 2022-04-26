#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

#include "RIFF.h"

bool RIFF::read(ifstream& fin)
{
	fin.read(riff_id, 4);
	if (strcmp(riff_id, "RIFF") != 0) return 0;

	fin.read((char*)&file_size, sizeof(int));
	fin.read(wave_id, 4);
	if (strcmp(wave_id, "WAVE") != 0) return 0;

	fin.read(fmt, 4);
	fin.read((char*)&format_len, sizeof(unsigned int));
	fin.read((char*)&format_tag, sizeof(short int));
	fin.read((char*)&num_chn, sizeof(short int));
	fin.read((char*)&sample_rate, sizeof(int));
	fin.read((char*)&bytes_sec, sizeof(int));
	fin.read((char*)&bytes_samp, sizeof(short int));
	fin.read((char*)&bits_samp, sizeof(short int));
	fin.read(data, 4);
	fin.read((char*)&data_size, sizeof(int));
	return 1;
}

void RIFF::write(ofstream& fout)
{
	fout.write(riff_id, 4);
	fout.write((char*)&file_size, sizeof(int));
	fout.write(wave_id, 4);

	fout.write(fmt, 4);
	fout.write((char*)&format_len, sizeof(unsigned int));
	fout.write((char*)&format_tag, sizeof(short int));
	fout.write((char*)&num_chn, sizeof(short int));
	fout.write((char*)&sample_rate, sizeof(int));
	fout.write((char*)&bytes_sec, sizeof(int));
	fout.write((char*)&bytes_samp, sizeof(short int));
	fout.write((char*)&bits_samp, sizeof(short int));
	fout.write(data, 4);
	fout.write((char*)&data_size, sizeof(int));
}
