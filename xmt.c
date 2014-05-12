#include "stdio.h"
#include "string.h"
#include "stdint.h"
#include "stdlib.h"

#define AMIGA 0x0
#define LINEAR 0x1
#define NOTE 0b1
#define INSTRUMENT 0b10
#define VOLUME 0b100
#define FX 0b1000
#define PARAM 0b10000
typedef struct
{
	uint8_t pscheme;
	uint8_t note;
	uint8_t instrument;
	uint8_t volume;
	uint8_t fx;
	uint8_t fx_param;
}
xm_note;

xm_note make_note(
		uint8_t note,
		uint8_t ins,
		uint8_t vol,
		uint8_t fx,
		uint8_t param)
{
	xm_note n;
	n.pscheme = 0x80;
	if(note != 0){
		n.pscheme = n.pscheme | NOTE;
		n.note = note;
	}
	if(ins != 0){
		n.pscheme = n.pscheme | INSTRUMENT;
		n.instrument = ins;
	}
	if(vol != 0){
		n.pscheme = n.pscheme | VOLUME;
		n.volume = vol;
	}
	if(fx != 0){
		n.pscheme = n.pscheme | FX;
		n.fx = fx;
	}
	if(param != 0){
		n.pscheme = n.pscheme | PARAM;
		n.fx_param = param;
	}

	return n;
}

void init_note(xm_note *n)
{
	
}

void write_note(FILE *f, xm_note *n)
{
	fwrite(&n->pscheme, sizeof(uint8_t), 1, f);
	if(n->pscheme & NOTE)
		fwrite(&n->note, sizeof(uint8_t), 1, f);
	if(n->pscheme & INSTRUMENT)
		fwrite(&n->instrument, sizeof(uint8_t), 1, f);
	if(n->pscheme & VOLUME)
		fwrite(&n->volume, sizeof(uint8_t), 1, f);
	if(n->pscheme & FX)
		fwrite(&n->fx, sizeof(uint8_t), 1, f);
	if(n->pscheme & PARAM)
		fwrite(&n->fx_param, sizeof(uint8_t), 1, f);
}

typedef struct
{
	uint32_t header_size;
	uint8_t packing_type;
	uint16_t num_rows; 
	uint16_t num_channels;
	uint16_t data_size;
	//uint8_t *data;
	xm_note *data;
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

void add_note(	xm_file *f, 
				uint8_t patnum,
				uint8_t chan,
				uint8_t row,
				xm_note note)
{	
	if((patnum + 1)> f->num_patterns) f->num_patterns = (patnum + 1);
	xm_pat *p = &f->pat[patnum];
	/* limit varibles */
	//row = row % (p->num_rows);
	if(note.pscheme & NOTE) p->data_size++;
	if(note.pscheme & INSTRUMENT) p->data_size++;
	if(note.pscheme & VOLUME) p->data_size++;
	if(note.pscheme & FX) p->data_size++;
	if(note.pscheme & PARAM) p->data_size++;
	p->data[(row * p->num_channels) + chan ] = note;

}

void set_BPM(xm_file *f, uint8_t bpm) 
{
	f->BPM =  bpm;
}
//fix this implementation
void set_nchan(xm_file *f, uint8_t n)
{
	f->num_channels = 1<<n;
}
void set_speed(xm_file *f, uint8_t speed) 
{	
	f->speed = speed;
}

void remove_note(	xm_pat *p, 
					uint8_t row,
					uint8_t chan)
{

}

void init_xm_pat(xm_file *f)
{
	int i, j;

	for(j = 0; j < 256; j++)
	{
		xm_pat *p = &f->pat[j];
		p->header_size = 0x09; 
		p->packing_type = 0x00;
		p->num_rows = 0x40;
		p->num_channels = f->num_channels;
		p->data_size = p->num_rows * f->num_channels;
		//p->data = (uint8_t *)malloc(sizeof(uint8_t) * p->data_size);
		//memset(p->data, 0x80, sizeof(uint8_t) * p->data_size);
		p->data = (xm_note *)malloc(sizeof(xm_note) * p->data_size);
		for(i = 0; i < p->data_size; i++) 
		{
			p->data[i].pscheme = 0x80;
		}
	}
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
	init_xm_pat(f);
	init_xm_ins(f, &f->ins[0]);

}

void update_ptable(xm_file *f, uint8_t pos, uint8_t pnum){
	if((pos + 1) > f->song_length) f->song_length = pos + 1;
	f->ptable[pos] = pnum;	
}
int add_pattern(xm_file *f){
	return f->num_patterns++;
}
void write_pattern_data(xm_file *f)
{
	int i, p;
	
	for(p = 0; p < f->num_patterns; p++)
	{
		fwrite(&f->pat[p].header_size, sizeof(uint32_t), 1, f->file);
		fwrite(&f->pat[p].packing_type, sizeof(uint8_t), 1, f->file);
		fwrite(&f->pat[p].num_rows, sizeof(uint16_t), 1, f->file);
		fwrite(&f->pat[p].data_size, sizeof(uint16_t), 1, f->file);
		for(i = 0; i < f->pat[p].num_rows * f->num_channels; i++){
			write_note(f->file, &f->pat[p].data[i]);
		}
	}
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

	update_ptable(&file, 1, 1);
	/*users shouldn't be able to set the channel numbers after initialization*/
	//set_nchan(&file, 2);
	add_note(&file, 1, 0, 0, make_note(60, 1, 0, 0, 0));
	add_note(	
			&file, 
			0,
			0,
			1,
			make_note(62, 0, 20, 0, 0));

	write_xm_file(&file);

	return 0;
}
