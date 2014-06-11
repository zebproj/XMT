#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include <sndfile.h>
/*TODO: add error handling to file I/O */

#define AMIGA 0x0
#define LINEAR 0x1
#define NOTE 0b1
#define INSTRUMENT 0b10
#define VOLUME 0b100
#define FX 0b1000
#define PARAM 0b10000
#define BSIZE 1024

#define NO_LOOP 0b000
#define FORWARD_LOOP 0b001
#define PING_PONG 0b010
#define BIT_16 0b100

#define NOTEOFF 96

typedef struct 
{
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
}
xm_params;

typedef struct
{

}
xm_ins_params;

typedef struct
{
	uint32_t length;
	uint32_t loop_start;
	uint32_t loop_length;
	uint8_t volume;
	int8_t finetune;
	uint8_t type;
	uint8_t panning;
	int8_t nn;
	int8_t reserved;	
	char sample_name[22];
	char *filename;
}
xm_samp_params;

xm_samp_params new_samp(char *filename)
{
	xm_samp_params s;
	s.volume = 0x40;
	s.finetune = 0;
	s.type = FORWARD_LOOP;
	s.panning = 0x80;
	s.nn = 0;
	s.filename = filename;
	return s;
}

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
		n.note = note + 1;
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

void init_note(xm_params *p)
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
	uint16_t x, y;
}point;

typedef struct
{
	uint32_t length;
	uint32_t loop_start;
	uint32_t loop_length;
	uint8_t volume;
	int8_t finetune;
	uint8_t type;
	uint8_t panning;
	int8_t nn;
	int8_t reserved;	
	char sample_name[22];
	char *filename;
	/*precursor to using libsndfile to embed samples*/
	//int8_t temp_buf[100];
	SNDFILE *sfile;
}
xm_sample;

typedef struct 
{
	uint32_t size;
	char name[22];
	uint8_t type;
	uint16_t num_samples;

	/*if num_samples > 0, these become important */
	uint32_t sample_header_size;
	uint8_t sample_map[96];
	point volume_points[12];
	point envelope_points[12];

	uint8_t num_volume_points;
	uint8_t num_envelope_points;
	uint8_t vol_sustain;
	uint8_t vol_loop_start;
	uint8_t vol_loop_end;
	uint8_t pan_sustain;
	uint8_t pan_loop_start;
	uint8_t pan_loop_end;
	uint8_t vol_type;
	uint8_t pan_type;
	uint8_t vib_type;
	uint8_t vib_sweep;
	uint8_t vib_depth;
	uint8_t vib_rate;
	uint16_t vol_fadeout;
	/*reserved 11-byte thing here(?)*/
	uint16_t reserved[11];

	xm_sample sample[16];
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

	/*test file */
	FILE *test_file;
}
xm_file;

int8_t scale_8(double s){
	return (uint8_t)(s * 0x7f);
}

int8_t write_delta_data(uint8_t *buffer, FILE *out, int count, int8_t prev)
{
	int8_t delta_buffer[count];
	int8_t tmp;
	int i;

	for(i = 0; i < count; i++){
		//tmp = scale_8(buffer[i]);
		tmp = buffer[i] - 0x7f;
		delta_buffer[i] = prev - tmp;
		prev = tmp;

		//delta_buffer[i] = buffer[i];
	}

	fwrite(delta_buffer, sizeof(int8_t), count, out);
	return prev;
}

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
//TODO fix this implementation
void set_nchan(xm_params *p, uint8_t n)
{
	if(n % 2 == 0)	
	p->num_channels = n;
	else
	p->num_channels = n + 1;
	
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
	//i->size = 0x1d;
	i->size = 0x107;
	memset(i->name, 0, sizeof(char) * 22);
	i->type = 0; 
	i->num_samples = 0;
	memset(i->sample_map, 0, sizeof(uint8_t) * 96);
	int k;
	for(k = 0; k < 12; k++){
		i->volume_points[k].x = 0;
		i->volume_points[k].y = 0;
		i->envelope_points[k].x = 0;
		i->envelope_points[k].y = 0;
	}
	i->sample_header_size = 0x28;
	i->num_volume_points = 0;
	i->num_envelope_points = 0;
	i->vol_sustain = 0x0;
	i->vol_loop_start = 0;
	i->vol_loop_end = 0;
	i->pan_sustain = 0;
	i->pan_loop_start = 0;
	i->pan_loop_end = 0;
	i->vol_type = 0;
	i->pan_type = 0;
	i->vib_type = 0;
	i->vib_sweep = 0;
	i->vib_depth = 0;
	i->vib_rate = 0;
	i->vol_fadeout = 0x7fff;
	memset(i->reserved, 0, sizeof(uint16_t) * 11);
} 

void init_xm_params(xm_params *p)
{
	memset(p->id_text, 0x0, sizeof(char) * 17);
	sprintf(p->id_text, "Extended Module:");
	memset(p->module_name, 0x0, sizeof(char) * 20);
	sprintf(p->module_name, "Test Module");
	memset(p->tracker_name, ' ', sizeof(char) * 20);
	sprintf(p->tracker_name, "Milkytracker");
	p->var = 0x1a;
	p->version = 0x0104;
	p->header_size = 0x114;
	p->song_length = 0x01;
	p->restart_position = 0x00;
	p->num_channels = 0x08;
	p->num_patterns= 0x01;
	p->num_instruments= 0x00;
	p->freq_table = LINEAR;
	p->speed = 6;
	p->BPM = 125;
}

void init_xm_sample(xm_sample *s, xm_samp_params *param)
{
	SF_INFO info;
	s->sfile = sf_open(param->filename, SFM_READ, &info);
	s->length = info.frames;
	s->loop_start = 0;
	s->loop_length= s->length;
	s->volume = param->volume;
	s->finetune= param->finetune;
	s->type = param->type;
	s->panning = param->panning;
	s->nn = param->nn;
	s->reserved = 0;
	memset(s->sample_name, 0, sizeof(char) * 22);
	//memset(s->temp_buf, 0, sizeof(char) * 100);
	//strcpy(s->sample_name, "test sample");
}

void init_xm_file(xm_file *f, xm_params *p){
	f->file = fopen("test.xm", "wb");
	memset(f->id_text, 0x0, sizeof(char) * 17);
	sprintf(f->id_text, "Extended Module:");
	memset(f->module_name, 0x0, sizeof(char) * 20);
	sprintf(f->module_name, "Test Module");
	memset(f->tracker_name, ' ', sizeof(char) * 20);
	sprintf(f->tracker_name, "Milkytracker");
	f->var = p->var;
	f->version = p->version;
	f->header_size = p->header_size;
	f->song_length = p->song_length;
	f->restart_position = p->restart_position;
	f->num_channels = p->num_channels;
	f->num_patterns= p->num_patterns;
	f->num_instruments= p->num_instruments;
	f->freq_table = p->freq_table;
	f->speed = p->speed;
	f->BPM = p->BPM;
	//initialize pattern table
	memset(f->ptable, 0x0, sizeof(uint8_t) * 256);
	init_xm_pat(f);

	/*open test file*/

	f->test_file = fopen("test_file.raw", "w");
}

void update_ptable(xm_file *f, uint8_t pos, uint8_t pnum){
	if((pos + 1) > f->song_length) f->song_length = pos + 1;
	f->ptable[pos] = pnum;	
}
int add_pattern(xm_file *f){
	return f->num_patterns++;
}

int add_instrument(xm_file *f)
{
	f->num_instruments++;
	int n = f->num_instruments;
	init_xm_ins(f, &f->ins[n - 1]);
	return n - 1;
}

long add_samp(xm_file *f, xm_samp_params *s, uint8_t ins)
{
	if(ins > f->num_instruments)
	{
		printf("invalid instrument number..\n");
		ins = ins % f->num_instruments;
	}
	xm_ins *i = &f->ins[ins];
	i->num_samples++;
	init_xm_sample(&i->sample[i->num_samples - 1], s);
	return i->num_samples;
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

void write_sample_data(xm_file *f, int insnum)
{
		xm_sample *s = &f->ins[insnum].sample[0];
		//init_xm_sample(s);
		fwrite(&s->length, sizeof(uint32_t), 1, f->file);
		fwrite(&s->loop_start, sizeof(uint32_t), 1, f->file);
		fwrite(&s->loop_length, sizeof(uint32_t), 1, f->file);
		fwrite(&s->volume, sizeof(uint8_t), 1, f->file);
		fwrite(&s->finetune, sizeof(int8_t), 1, f->file);
		fwrite(&s->type, sizeof(uint8_t), 1, f->file);
		fwrite(&s->panning, sizeof(uint8_t), 1, f->file);
		fwrite(&s->nn, sizeof(int8_t), 1, f->file);
		fwrite(&s->reserved, sizeof(int8_t), 1, f->file);
		fwrite(&s->sample_name, sizeof(char), 22, f->file);
		uint8_t buffer[BSIZE];
		int count;
		int8_t prev = 0;
		while(count != 0)
		{
			//count = sf_read_double(s->sfile, buffer, BSIZE);
			count = sf_read_raw(s->sfile, buffer, BSIZE * sizeof(uint8_t));
			//write_delta_data(buffer,f->file, count);
			prev = write_delta_data(buffer,f->file, count, prev);
		}
		//fwrite(&s.temp_buf, sizeof(int8_t), 100, f->file);
		sf_close(s->sfile);

}

void write_instrument_data(xm_file *f)
{
	int i;
	for(i = 0; i < f->num_instruments; i++) 
	{
		fwrite(&f->ins[i].size, sizeof(uint32_t), 1, f->file);
		fwrite(f->ins[i].name, sizeof(char), 22, f->file);
		fwrite(&f->ins[i].type, sizeof(uint8_t), 1, f->file);
		fwrite(&f->ins[i].num_samples, sizeof(uint16_t), 1, f->file);
		if(f->ins[i].num_samples!= 0){
			fwrite(&f->ins[i].sample_header_size, sizeof(uint16_t), 1, f->file);
			fwrite(&f->ins[i].sample_header_size, sizeof(uint16_t), 1, f->file);
			fwrite(&f->ins[i].sample_map, sizeof(uint8_t), 96, f->file);
			fwrite(&f->ins[i].volume_points, sizeof(point), 12, f->file);
			fwrite(&f->ins[i].envelope_points, sizeof(point), 12, f->file);
			fwrite(&f->ins[i].num_volume_points, sizeof(uint8_t), 1, f->file);
			fwrite(&f->ins[i].num_envelope_points, sizeof(uint8_t), 1, f->file);
			fwrite(&f->ins[i].vol_sustain, sizeof(uint8_t), 1, f->file);
			fwrite(&f->ins[i].vol_loop_start, sizeof(uint8_t), 1, f->file);
			fwrite(&f->ins[i].vol_loop_end, sizeof(uint8_t), 1, f->file);
			fwrite(&f->ins[i].pan_sustain, sizeof(uint8_t), 1, f->file);
			fwrite(&f->ins[i].pan_loop_start, sizeof(uint8_t), 1, f->file);
			fwrite(&f->ins[i].pan_loop_end, sizeof(uint8_t), 1, f->file);
			fwrite(&f->ins[i].vol_type, sizeof(uint8_t), 1, f->file);
			fwrite(&f->ins[i].pan_type, sizeof(uint8_t), 1, f->file);
			fwrite(&f->ins[i].vib_type, sizeof(uint8_t), 1, f->file);
			fwrite(&f->ins[i].vib_sweep, sizeof(uint8_t), 1, f->file);
			fwrite(&f->ins[i].vib_depth, sizeof(uint8_t), 1, f->file);
			fwrite(&f->ins[i].vib_rate, sizeof(uint8_t), 1, f->file);
			fwrite(&f->ins[i].vol_fadeout, sizeof(uint16_t), 1, f->file);
			fwrite(&f->ins[i].reserved, sizeof(uint16_t), 11, f->file);

			write_sample_data(f, i);
		}
	}

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

void write_xm_file(xm_file *f)
{
	write_header_data(f);
	write_pattern_data(f);
	write_instrument_data(f);
	fclose(f->file);
	fclose(f->test_file);
}

int main()
{
	xm_params p; 
	xm_file file;
	init_xm_params(&p);
	set_nchan(&p, 8);
	init_xm_file(&file, &p);

	int sine = add_instrument(&file);
	xm_samp_params sparams = new_samp("pad8bit.wav");
	add_samp(&file, &sparams, sine);
	//sparams = new_samp("brendan.wav");
	int note[] = {72, 74, 79, 83};
	int i;

	for(i = 0; i < 4; i++) 
	{
		add_note(&file, 0, i, i * 4, make_note(note[i], 1, 0, 0, 0));
		add_note(&file, 0, i, 32, make_note(NOTEOFF, 0, 0, 0, 0));
	}

	write_xm_file(&file);

	return 0;
}
