#include "sampler.h"


// === Resample: changes pitch and speed together ===
// int16_t resample(const int16_t* sampleArray, size_t& samplePointer, size_t pitchStep_resample_fixed) {
//   samplePointer += pitchStep_resample_fixed;
//   size_t index = samplePointer >> FIXED_SHIFT;

//   // Optional: wrap around if sample ends
//   if (index >= sampleLength) {
//     samplePointer = 0;
//     index = 0;
//   }

//   return sampleArray[index];
// }


void SampleVoice::init(const int16_t* array, size_t length) {
  sampleArray = array;
  sampleLength = length;
}

void SampleVoice::trigger(uint8_t vel) {
  samplePointer = 0;
  velocity = vel;
  playing = true;
}

int16_t SampleVoice::render(size_t pitchStep_fixed) {
  if (!playing || sampleArray == nullptr) return 0;

  samplePointer += pitchStep_fixed;
  size_t index = samplePointer >> FIXED_SHIFT;

  if (index >= sampleLength) {
    playing = false;
    return 0;
  }

  // Apply velocity as volume scaling
  int32_t sample = sampleArray[index];
  sample = (sample * velocity) / 127; // if problematic use >> 7

  return (int16_t)sample;
}


// === PitchShift: changes pitch only, preserves timing ===
int16_t pitchShift(int16_t inputSample, uint16_t pitchStep_shift_fixed, uint16_t buffer_size) {
  static int16_t buffer[MAX_BUFFER_SIZE] = { 0 };
  static uint32_t readIndex_fixed = 0;
  static uint16_t writeIndex = 0;

  if (buffer_size > MAX_BUFFER_SIZE) {
    buffer_size = MAX_BUFFER_SIZE;
  }

  buffer[writeIndex] = inputSample;

  uint16_t index1 = readIndex_fixed >> FIXED_SHIFT;
  uint16_t index2 = (index1 + 1) % buffer_size;
  uint8_t frac = readIndex_fixed & 0xFF;

  int32_t s1 = buffer[index1];
  int32_t s2 = buffer[index2];
  int32_t interpolated = ((s1 * (256 - frac)) + (s2 * frac)) >> FIXED_SHIFT;

  readIndex_fixed += pitchStep_shift_fixed;
  if (readIndex_fixed >= (buffer_size << FIXED_SHIFT)) {
    readIndex_fixed -= (buffer_size << FIXED_SHIFT);
  }

  writeIndex = (writeIndex + 1) % buffer_size;

  return (int16_t)interpolated;
}

// === Multi-pole low-pass filter with resonance ===
int16_t lowPassFilterMultiPole(int16_t input, uint8_t alpha, uint8_t poles, uint8_t resonance) {
  static int32_t lp1 = 0, lp2 = 0, lp3 = 0, lp4 = 0;

  // feedback from previous cycle
  int32_t feedback = (lp4 * resonance) >> 8;
  int32_t modifiedInput = input + feedback;

  // Always compute all poles
  lp1 = ((lp1 * (256 - alpha)) + (modifiedInput * alpha)) >> 8;
  lp2 = ((lp2 * (256 - alpha)) + (lp1 * alpha)) >> 8;
  lp3 = ((lp3 * (256 - alpha)) + (lp2 * alpha)) >> 8;
  lp4 = ((lp4 * (256 - alpha)) + (lp3 * alpha)) >> 8;

  // Return the selected pole
  switch (poles) {
    case 0: return input;
    case 1: return constrain(lp1, -32768, 32767);
    case 2: return constrain(lp2, -32768, 32767);
    case 3: return constrain(lp3, -32768, 32767);
    default: return constrain(lp4, -32768, 32767);
  }
}


// === Overdrive effect ===
// input: 16-bit audio sample
// gain: amplification factor (1–8)
// drive: saturation intensity (0–255)
int16_t overdrive(int16_t input, uint8_t gain, uint8_t drive) {
  // Apply gain
  int32_t amplified = (int32_t)input * gain;

  // Soft clipping using tanh-like approximation
  int32_t clipped = (amplified * drive) / (abs(amplified) + drive);

  // Clamp to 16-bit range
  if (clipped > 32767) clipped = 32767;
  if (clipped < -32768) clipped = -32768;

  return (int16_t)clipped;
}


// === Updates LED based on I2S buffer underflow status ===
void updateLEDStatus() {
  digitalWrite(LED_PIN, i2s.getOverUnderflow() ? HIGH : LOW);
}