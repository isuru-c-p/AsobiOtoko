#include "sound.h"

void write_sound_reg(Sound * pSound, uint16_t addr, uint8_t val)
{
	pSound->sound_ram[addr-0xFF10] = val;
	return;
}

uint8_t read_sound_reg(Sound * pSound, uint16_t addr)
{
	return pSound->sound_ram[addr-0xFF10];
}

void initSound(Sound * pSound)
{
	bzero((void*)pSound, sizeof(Sound));
}

void sdl_init_audio()
{
	SDL_AudioSpec fmt;

    /* Set 16-bit stereo audio at 22Khz */
    fmt.freq = 22050;
    fmt.format = AUDIO_S16;
    fmt.channels = 2;
    fmt.samples = 512;        /* A good value for games */
    fmt.callback = sdl_audio_callback;
    fmt.userdata = NULL;

    /* Open the audio device and start playing sound! */
    if ( SDL_OpenAudio(&fmt, NULL) < 0 ) {
        fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
        exit(1);
    }
}

void sdl_play_audio()
{
    SDL_PauseAudio(0);
}

void sdl_disable_audio()
{
	SDL_PauseAudio(1);
}

void sdl_audio_callback(void *udata, uint8_t * stream, int len)
{
	
}

int get_duty_time_tcycles(uint8_t duty_time_raw, uint8_t frequency_raw)
{
	// tcycle frequency = 4.196MHz
	int period_tcycles = GB_TO_TCYCLES(frequency_raw);
	
	// LUT
	switch(duty_time_raw)
	{
		case 0x0:
			return period_tcycles * 0.125;
			
		case 0x1:
			return period_tcycles * 0.25;
			
		case 0x2:
			return period_tcycles * 0.50;
			
		case 0x3:
			return period_tcycles * 0.75;
			
		default:
			return 0;
	}
}

int get_sweep_time_tcycles(uint8_t sweep_time_raw)
{
	// tcycle frequency = 4.196MHz
	switch(sweep_time_raw)
	{
		// no change
		case 0x0:
			return 0;
			
		// 7.8ms
		case 0x1:
			return 32729;
			
		// 15.6ms
		case 0x2:
			return 65458;
			
		// 23.4ms
		case 0x3:
			return 98186;
			
		// error
		default:
			return 0;
	}
}



void update_square_timer(Square_wave * square_wave, int tcycles)
{
	square_wave->internal_freq_counter -= tcycles;
	
	if(square_wave->internal_freq_counter <= 0)
	{
		square_wave->output = 1;
		square_wave->internal_freq_counter = GB_TO_TCYCLES(square_wave->frequency) + square_wave->internal_freq_counter;
	}
	
	return;
}

void update_square_duty(Square_wave * square_wave, int tcycles)
{
	if(square_wave->output == 0)
		return;

	square_wave->duty_counter -= tcycles;
	
	if(square_wave->duty_counter <= 0)
	{
		square_wave->output = 0;
		square_wave->duty_counter = get_duty_time_tcycles(square_wave->duty_cycle, square_wave->frequency) + square_wave->duty_counter;
	}
	
	return;
}

// 256Hz counter
void update_length_counter(Square_wave * square_wave, int tcycles)
{	
	if(square_wave->length_counter == 0)
		return;

	square_wave->length_counter_internal -= tcycles;
	
	if(square_wave->length_counter_internal <= 0)
	{
		square_wave->length_counter_internal = HZ_TO_TCYCLES(256) + square_wave->length_counter_internal;
		square_wave->length_counter--;
		
		if(square_wave->length_counter == 0)
		{
			square_wave->enabled = 0;
			square_wave->output = 0;
		}	
	}
}

// 64Hz
void update_volume_envelope(Square_wave * square_wave, int tcycles)
{
	square_wave->envelope_counter-= tcycles;
	
	if(square_wave->envelope_counter <= 0)
	{
		square_wave->envelope_counter = HZ_TO_TCYCLES(64) + square_wave->envelope_counter;
		
		if(square_wave->sweep_mode)
		{
			square_wave->volume--;
		}
		else
		{
			square_wave->volume++;
		}
		
		if(square_wave->volume > 15)
		{
			square_wave->volume = 15;
		}
		else if(square_wave->volume < 0)
		{
			square_wave->volume = 0;
		}
	}
}

void update_frequency_sweep(Square_wave * square_wave, int tcycles)
{
	square_wave->sweep_counter += tcycles;
	
	if(square_wave->sweep_counter >= get_sweep_time_tcycles(square_wave->sweep_period))
	{
		square_wave->sweep_counter -= get_sweep_time_tcycles(square_wave->sweep_period);
		
		if(square_wave->sweep_mode)
		{
			square_wave->frequency--;
		}
		else
		{
			square_wave->frequency++;
		}
		
	}
	
}

void update_sound_buffer(Square_wave * square_wave, int tcycles)
{	
	// wait for buffer to be cleared
	while(square_wave->audio_len > 0);

	int sampleNo = tcycles * 4196000 * SAMPLES_PER_SECOND;
	
	if(sampleNo > SOUND_BUFFER_LEN)
	{
		printf("Error, sound buffer overflow. tcycles: %d\n", tcycles);
		return;
	}
	
	int i;
	
	for(i = 0 ; i < sampleNo; i++)
	{
		if(square_wave->enabled)
			square_wave->data[i] = square_wave->output;
		else
			square_wave->data[i] = 0;
		// TODO check for transition (1->0 or 0->1) here
	}
	
	square_wave->audio_len = sampleNo;
}

void update_waveforms(Sound * pSound, int tcycles)
{
	// update square waves
	update_square_timer(&pSound->square_waves[0], tcycles);
	update_square_duty(&pSound->square_waves[0], tcycles);
	update_square_timer(&pSound->square_waves[1], tcycles);
	update_square_duty(&pSound->square_waves[1], tcycles);
	
	update_volume_envelope(&pSound->square_waves[0], tcycles);
	update_volume_envelope(&pSound->square_waves[1], tcycles);
	
	update_sound_buffer(&pSound->square_waves[0], tcycles);
	update_sound_buffer(&pSound->square_waves[1], tcycles);	
	
	
}
