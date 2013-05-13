#ifndef __SOUND_H__
#define __SOUND_H__

#include <stdint.h>
#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>
#include <SDL/SDL_thread.h>

#define SOUND_RAM_SIZE 48
#define SOUND_BUFFER_LEN 1025000
#define SAMPLES_PER_SECOND 21000/*440502097152*/
#define SWEEP_FRAME_128HZ_COUNT 32768

typedef struct _Square_wave {
	uint16_t frequency;
	uint8_t sweep_period;
	uint8_t sweep_shift;
	uint8_t sweep_mode; // 0 = increase, 1 = decrease
	int sweep_counter;
  int sweep_frame_counter;
	uint8_t duty_cycle;
	int duty_counter;
	uint8_t length_counter;
	int length_counter_internal;
	uint8_t length_counter_enabled;
	uint8_t initial_volume;
	uint8_t envelope_mode; // 0 = up, 1 = down
	uint8_t envelope_period;
	uint8_t reset_flag;
	uint8_t counter_consecutive_selection;
	uint8_t output;
	uint8_t enabled;
	uint8_t volume;
	int envelope_counter;
	int internal_freq_counter;
  uint16_t frequency_shadow;
  uint8_t sweep_internal_enabled;
  int output_counter;
} Square_wave;

typedef struct _Sound {
	uint8_t sound_ram[SOUND_RAM_SIZE];
	uint8_t data[SOUND_BUFFER_LEN];
  uint32_t current_play_count;
	int audio_len;
	Square_wave square_waves[2];
	uint8_t noise_channel[SOUND_BUFFER_LEN];
	uint8_t wave_table_channel[SOUND_BUFFER_LEN];
  uint32_t sampleCycleCount;
  SDL_sem* sound_sem;
  SDL_sem* sound_writer_sem;
  uint32_t sampleBufferLength;
  uint32_t tcycles_per_ms;
  uint8_t square_waveforms[4][8];
} Sound;

#define HZ_TO_GB(hz_in) (2048 - (131072/(hz_in)))
#define GB_TO_HZ(gb_in) (131072/(2048 - (gb_in)))
#define GB_TO_TCYCLES(gb_in) (4196000/(GB_TO_HZ((gb_in))))
#define HZ_TO_TCYCLES(hz_in) (GB_TO_TCYCLES(HZ_TO_GB((hz_in))))

void write_sound_reg(Sound * pSound, uint16_t addr, uint8_t val);
uint8_t read_sound_reg(Sound * pSound, uint16_t addr);

void initSound(Sound * pSound);

void sdl_audio_callback(void *udata, uint8_t * stream, int len);
void sdl_enable_audio();
void sdl_disable_audio();

void update_square_timer(Square_wave * square_wave, int tcycles);
void update_square_duty(Square_wave * square_wave, int tcycles);
void update_volume_envelope(Square_wave * square_wave, int tcycles);
void update_sound_buffer(Sound* pSound, Square_wave * square_wave, int tcycles);
void update_waveforms(Sound * pSound, int tcycles);
void update_waveform_tcycle(Sound*pSound);


int get_duty_time_tcycles(uint8_t duty_time_raw, uint8_t frequency_raw);
int get_sweep_time_tcycles(uint8_t sweep_time_raw);

#endif
