#include "stdio.h"
#include "string.h"
#include "stdint.h"
#include "stdlib.h"

#define AMIGA 0x0
#define LINEAR 0x1

typedef struct
{
int size;
uint8_t *data;
}
pat_data;

typedef struct
{
	uint32_t header_size;
	uint8_t packing_type;
	uint16_t num_rows; 
	uint16_t data_size;
	uint8_t *data;
}
xm_pat;

typedef struct 
{
	uint32_t size;
	char name[22];
	uint8_t type;
	uint16_t num_samples;

}
xm_ins;

typedef struct
{
	FILE *file;
	char id_text[17];
	char tracker_name[20];
	char module_name[20];
	char var;
	uint16_t version;
	uint32_t header_size;
	uint16_t song_length;
	uint16_t restart_position;
	uint16_t num_channels;
	uint16_t num_patterns;
	uint16_t num_instruments;
	uint16_t freq_table;
	uint16_t speed;
	uint16_t BPM;
	uint8_t ptable[256];

	xm_pat pat[256];
	xm_ins ins[256];
}
xm_file;

void init_xm_pat(xm_file *f, xm_pat *p)
{
	p->header_size = 0x09; 
	p->packing_type = 0x00;
	p->num_rows = 0x40;
	p->data_size = p->num_rows * f->num_channels;
	p->data = (uint8_t *)malloc(sizeof(uint8_t) * p->data_size);
	memset(p->data, 0x80, sizeof(uint8_t) * p->data_size);
}

void init_xm_ins(xm_file *f, xm_ins *i)
{
	i->size = 0x1d;
	memset(i->name, 0, sizeof(char) * 22);
	i->type = 0;
	i->num_samples = 0;
}

void init_xm_file(xm_file *f){
	f->file = fopen("test.xm", "wb");
	memset(f->id_text, 0x0, sizeof(char) * 17);
	sprintf(f->id_text, "Extended Module:");
	memset(f->module_name, 0x0, sizeof(char) * 20);
	sprintf(f->module_name, "Test Module");
	memset(f->tracker_name, ' ', sizeof(char) * 20);
	sprintf(f->tracker_name, "Milkytracker");
	f->var = 0x1a;
	f->version = 0x0104;
	f->header_size = 0x114;
	f->song_length = 0x01;
	f->restart_position = 0x00;
	f->num_channels = 0x08;
	f->num_patterns= 0x01;
	f->num_instruments= 0x01;
	f->freq_table = LINEAR;
	f->speed = 6;
	f->BPM = 125;
	//initialize pattern table
	memset(f->ptable, 0x0, sizeof(uint8_t) * 256);
	f->ptable[0] = 1;	
	init_xm_pat(f, &f->pat[0]);
	init_xm_ins(f, &f->ins[0]);
}

void write_pattern_data(xm_file *f)
{
	int i;
	fwrite(&f->pat[0].header_size, sizeof(uint32_t), 1, f->file);
	fwrite(&f->pat[0].packing_type, sizeof(uint8_t), 1, f->file);
	fwrite(&f->pat[0].num_rows, sizeof(uint16_t), 1, f->file);
	fwrite(&f->pat[0].data_size, sizeof(uint16_t), 1, f->file);
	fwrite(f->pat[0].data, sizeof(uint8_t), f->pat[0].data_size, f->file);
}

void write_instrument_data(xm_file *f)
{
	fwrite(&f->ins[0].size, sizeof(uint32_t), 1, f->file);
	fwrite(f->ins[0].name, sizeof(char), 22, f->file);
	fwrite(&f->ins[0].type, sizeof(uint8_t), 1, f->file);
	fwrite(&f->ins[0].num_samples, sizeof(uint16_t), 1, f->file);
}

void write_header_data(xm_file *f){
	fwrite(f->id_text, sizeof(char), sizeof(f->id_text), f->file);
	fwrite(f->module_name, sizeof(char), sizeof(f->module_name), f->file);
	fwrite(&f->var, sizeof(char), 1, f->file);
	fwrite(f->tracker_name, sizeof(char), sizeof(f->tracker_name), f->file);
	fwrite(&f->version, sizeof(uint16_t), 1, f->file);
	fwrite(&f->header_size, sizeof(uint32_t), 1, f->file);
	fwrite(&f->song_length, sizeof(uint16_t), 1, f->file);
	fwrite(&f->restart_position, sizeof(uint16_t), 1, f->file);
	fwrite(&f->num_channels, sizeof(uint16_t), 1, f->file);
	fwrite(&f->num_patterns, sizeof(uint16_t), 1, f->file);
	fwrite(&f->num_instruments, sizeof(uint16_t), 1, f->file);
	fwrite(&f->freq_table, sizeof(uint16_t), 1, f->file);
	fwrite(&f->speed, sizeof(uint16_t), 1, f->file);
	fwrite(&f->BPM, sizeof(uint16_t), 1, f->file);
	fwrite(f->ptable, sizeof(uint8_t), 256, f->file);
}

void write_xm_file(xm_file *f){
	write_header_data(f);
	write_pattern_data(f);
	write_instrument_data(f);
	fclose(f->file);
}

int main()
{
	xm_file file;

	init_xm_file(&file);

	write_xm_file(&file);

	return 0;
}
