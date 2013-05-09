#include "sound.h"

void config_square_wave(Square_wave* wave, uint16_t offset, uint8_t val)
{
  switch(offset)
  {
      case 0:
        wave->sweep_period = (val >> 4) & 0x7;
        wave->sweep_mode = (val >> 3) & 0x1;
        wave->sweep_shift = val & 0x7;
        break;

      case 1:
        wave->duty_cycle = (val >> 6) & 0x3;
        wave->length_counter = val & 0x3f;
        break;

      case 2:
        wave->initial_volume = (val >> 4) & 0xf;
        wave->envelope_mode = (val >> 3) & 0x1;
        wave->envelope_period = val & 0x7;
        break;

      case 3:
        wave->frequency &= 0xff00;
        wave->frequency |= val;
        break;

      case 4:
        wave->frequency &= 0x00ff;
        wave->frequency |= ((val & 0x7) << 8);
        wave->reset_flag = (val >> 7) & 0x1;
        wave->counter_consecutive_selection = (val >> 6) & 0x1;
        break;
  }
}

void write_sound_reg(Sound * pSound, uint16_t addr, uint8_t val)
{
  //printf("write sound reg: %x, val: %x\n", addr, val);
  switch(addr)
  {
      case 0xff10:
      case 0xff11:
      case 0xff12:
      case 0xff13:
      case 0xff14:
        config_square_wave(&pSound->square_waves[0], addr-0xff10, val);
        break;

      case 0xff16:
      case 0xff17:
      case 0xff18:
      case 0xff19:
        config_square_wave(&pSound->square_waves[1], addr-0xff15, val);
        break;

      case 0xff25:
        pSound->square_waves[0].enabled = ((val &0x1) || ((val >> 4) & 0x1));
        pSound->square_waves[1].enabled = (((val >> 5) & 0x1) || ((val >> 1) & 0x1));
        break;

  }
	pSound->sound_ram[addr-0xFF10] = val;
	return;
}

uint8_t read_sound_reg(Sound * pSound, uint16_t addr)
{
  //printf("read sound reg: %x\n", addr);
	return pSound->sound_ram[addr-0xFF10];
}

void sdl_init_audio(Sound* pSound)
{
	SDL_AudioSpec fmt, obtained;

    /* Set 16-bit stereo audio at 22Khz */
    fmt.freq = SAMPLES_PER_SECOND;
    fmt.format = AUDIO_S16;
    fmt.channels = 0;
    fmt.samples = 512;        /* A good value for games */
    fmt.callback = sdl_audio_callback;
    fmt.userdata = (void*)pSound;

    /* Open the audio device and start playing sound! */
    if ( SDL_OpenAudio(&fmt, &obtained) < 0 ) {
        fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
        exit(1);
    }

    pSound->sound_sem = SDL_CreateSemaphore(0);
    pSound->sampleBufferLength = obtained.samples;
    if(pSound->sound_sem == NULL)
    {
      fprintf(stderr, "Error creating sound semaphore!\n");
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

void initSound(Sound * pSound)
{
	bzero((void*)pSound, sizeof(Sound));
  sdl_init_audio(pSound);
  sdl_play_audio();
}

void sdl_audio_callback(void *udata, uint8_t * stream, int len)
{
  Sound* pSound = (Sound*)udata;
  //Square_wave* squareWave1 = &(pSound->square_waves[0]);
  /*if(squareWave1->audio_len == 0)
  {
     return;
  }*/

  //printf("audio callback, square audio_len: %d, len: %d\n", squareWave1->audio_len, len);

  //uint32_t tcyclesPerSample = ((pSound->tcycles_per_ms*1000)/(SAMPLES_PER_SECOND));
  //printf("tcycles per sample: %d\n", tcyclesPerSample);
  int i, j;
  //SDL_SemWait(pSound->sound_sem);
  for(i = 0; i < len; i++)
  {
    float total = 0;
    uint32_t tcyclesPerSample = (4194304/(SAMPLES_PER_SECOND));
    for(j = 0; j < tcyclesPerSample; j++)
    {
      pSound->current_play_count = (pSound->current_play_count+1) % SOUND_BUFFER_LEN;
      total += pSound->data[pSound->current_play_count];
    }

    float avg = (total / tcyclesPerSample);
    //printf("avg: %f\n", avg);
    stream[i] = (avg > (65535/2)) ? 1 : 0;

      /*if((squareWave1->current_play_count+i) >= squareWave1->audio_len)
      {
        stream[i] = 0;
      }
      else
      {
        stream[i] = squareWave1->data[squareWave1->current_play_count+i];
      }*/
  }
  //SDL_SemPost(pSound->sound_sem);
  /*squareWave1->current_play_count += len;
  //printf("current_play_count: %d\n", squareWave1->current_play_count);

  if(squareWave1->current_play_count >= squareWave1->audio_len)
  {
    squareWave1->current_play_count = 0;
    SDL_SemPost(pSound->sound_sem);
  }*/
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
    //printf("square 1, internal_freq_counter: %d\n", square_wave->internal_freq_counter);
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
	  //printf("square 0, duty_counter: %d\n", square_wave->duty_counter);
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

  uint32_t sweep_time = get_sweep_time_tcycles(square_wave->sweep_period);
	if(square_wave->sweep_counter >= sweep_time)
	{
		square_wave->sweep_counter -= sweep_time;

		if(square_wave->sweep_mode)
		{
      square_wave->frequency = square_wave->frequency - (square_wave->frequency >> square_wave->sweep_shift);
		}
		else
		{
      square_wave->frequency = square_wave->frequency + (square_wave->frequency >> square_wave->sweep_shift);
		}

		square_wave->output = 1;
		square_wave->internal_freq_counter = GB_TO_TCYCLES(square_wave->frequency);
    square_wave->duty_counter = get_duty_time_tcycles(square_wave->duty_cycle, square_wave->frequency);
	}

}

void update_sound_buffer(Sound* pSound, Square_wave * square_wave, int tcycles)
{
	// wait for buffer to be cleared
	//while(square_wave->audio_len > 0);

  //printf("buffered output: %d\n", square_wave->output);
	/*int sampleNo = tcycles * (SAMPLES_PER_SECOND/4196000.0f);

  printf("sample No: %d\n", sampleNo);

	if(sampleNo > SOUND_BUFFER_LEN)
	{
		printf("Error, sound buffer overflow. tcycles: %d\n", tcycles);
		return;
	}*/

	int i;// = square_wave->audio_len;

  Square_wave* squareWave1 = &(pSound->square_waves[0]);
  Square_wave* squareWave2 = &(pSound->square_waves[1]);


	for(i = 0 ; i < tcycles; i++)
	{
    update_waveform_tcycle(pSound);
    uint8_t output1 = (squareWave1->enabled) ? squareWave1->output : 0;
    uint8_t output2 = (squareWave2->enabled) ? squareWave2->output : 0;

    uint32_t sampleRaw = (output1*(squareWave1->volume << 12)) + (output2*(squareWave2->volume << 12));
    uint16_t sample = (sampleRaw > 65535) ? 65535 : sampleRaw;

    pSound->data[(pSound->audio_len+i) % SOUND_BUFFER_LEN] = sample;

		/*if(square_wave->enabled)
    {
			square_wave->data[(square_wave->audio_len+i) % SOUND_BUFFER_LEN] = ((65535*square_wave->volume*square_wave->output)/16);
    }
		else
    {
			square_wave->data[(square_wave->audio_len+i) % SOUND_BUFFER_LEN] = 0;
    }*/
		// TODO check for transition (1->0 or 0->1) here
	}

	pSound->audio_len = (pSound->audio_len+tcycles) % SOUND_BUFFER_LEN;
  /*if(square_wave->audio_len >= pSound->sampleBufferLength)
  {
    //printf("play\n");
    sdl_play_audio();
    SDL_SemWait(pSound->sound_sem);
    sdl_disable_audio();
    square_wave->audio_len = 0;
  }*/
}

void update_waveform_tcycle(Sound*pSound)
{
	// update square waves
	update_square_timer(&pSound->square_waves[0], 1);
	update_square_duty(&pSound->square_waves[0], 1);
  update_frequency_sweep(&pSound->square_waves[0], 1);
  update_square_timer(&pSound->square_waves[1], 1);
	update_square_duty(&pSound->square_waves[1], 1);

	update_volume_envelope(&pSound->square_waves[0], 1);
	update_volume_envelope(&pSound->square_waves[1], 1);
}

void update_waveforms(Sound * pSound, int tcycles)
{

  update_sound_buffer(pSound, NULL, tcycles);
  //update_sound_buffer(pSound, &pSound->square_waves[1], tcycles);

  /*pSound->sampleCycleCount += tcycles;

  if(pSound->sampleCycleCount >= (4196000/SAMPLES_PER_SECOND))
  {
  	update_sound_buffer(pSound, &pSound->square_waves[0], pSound->sampleCycleCount);
    pSound->sampleCycleCount = 0;
  }*/
	//update_sound_buffer(&pSound->square_waves[1], tcycles);
}
