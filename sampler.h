#ifndef SAMPLER_H
#define SAMPLER_H

#include <Arduino.h>
#include <I2S.h>

// === Constants ===
#define FIXED_SHIFT 8
#define MAX_BUFFER_SIZE 512
#define LED_PIN LED_BUILTIN

// === External Globals ===
extern I2S i2s;
extern size_t sampleLength;

// === Audio Processing Functions ===

// Resample: changes pitch and speed together
//int16_t resample(const int16_t* sampleArray, size_t& samplePointer, size_t pitchStep_resample_fixed);
class SampleVoice {
public:
  const int16_t* sampleArray = nullptr;
  size_t sampleLength = 0;
  size_t samplePointer = 0;
  bool playing = false;
  uint8_t velocity = 127;  // default full velocity

  SampleVoice() = default;

  void init(const int16_t* array, size_t length);
  void trigger(uint8_t vel = 127);
  int16_t render(size_t pitchStep_fixed);
};



// PitchShift: changes pitch only, preserves timing
int16_t pitchShift(int16_t inputSample, uint16_t pitchStep_shift_fixed, uint16_t buffer_size);

// Multi-pole low-pass filter with resonance
int16_t lowPassFilterMultiPole(int16_t input, uint8_t alpha, uint8_t poles, uint8_t resonance);

// Overdrive effect
int16_t overdrive(int16_t input, uint8_t gain, uint8_t drive);

// Updates LED based on I2S buffer underflow status
void updateLEDStatus();

#endif // SAMPLER_H