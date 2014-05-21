#include <stdio.h>
#include <stdint.h>
#include <sndfile.h>
/*TODO: add error handling to file I/O */

#define BSIZE 1024
#include "samplewrite.h"



int main()
{
	SNDFILE *sfile;
	SF_INFO info;
	SF_INFO out_info;
	sfile = sf_open("sine.wav", SFM_READ, &info);
	
	out_info.format = SF_FORMAT_RAW | SF_FORMAT_DPCM_8;
	//out_info.format = info.format;
	out_info.channels = 1;
	out_info.samplerate = 44100;
	//out = sf_open("test.raw", SFM_WRITE, &out_info);
	int count;
	sf_count_t num_frames = info.frames;

	//int8_t buffer[BSIZE];	
	double buffer[BSIZE];	
	//double buffer[BSIZE];	
	FILE *out = fopen("test.raw", "wb");
	printf("the length of the file is %ld samples\n", info.frames);

	if(info.format & SF_FORMAT_PCM_S8){
		printf("the format is 8-bit PCM!\n");
	}else if(info.format & SF_FORMAT_PCM_16){
		printf("the format is 16-bit PCM!\n");
	}else{
		printf("invalid sample format... please convert to 16 or 8 bit\n");
	}

	//count = sf_read_raw(sfile, buffer, BSIZE * sizeof(int8_t));
	count = sf_read_double(sfile, buffer, BSIZE);
	//sf_write_short(out, buffer, count);
	printf("read %ld items..\n", count);
	write_delta_data(buffer, out, count);
	

	sf_close(sfile);
	fclose(out);
	return 0;
}
