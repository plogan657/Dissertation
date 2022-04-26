#ifndef RIFF_H
#define RIFF_H

#include <fstream>
using std::ifstream;
using std::ofstream;

class RIFF {
public:
	bool read(ifstream& fin);
	void write(ofstream& fout);

	short int get_bytes_samp() const { return bytes_samp; }
	unsigned int get_data_size() const { return data_size; }

private:
	// RIFF header parameters
	char riff_id[4];
	int file_size;
	char wave_id[4];
	char fmt[4];
	unsigned int format_len;
	short int format_tag;
	short int num_chn;
	int sample_rate;
	int bytes_sec;
	short int bytes_samp;
	short int bits_samp;
	char data[4];
	unsigned int data_size;
};
#endif
