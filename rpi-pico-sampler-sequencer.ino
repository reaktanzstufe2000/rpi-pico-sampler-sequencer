#include <Arduino.h>
#include <PicoLCD_I2C.h>
#include <I2S.h>
#include "tr909.h"
#include "Encoder.h"
#include "gui.h"
#include "sampler.h"
#include "sequencer.h"



// === Constants ===
#define MAX_BUFFER_SIZE 2048
#define FIXED_SHIFT 8  // Q8.8 format
#define LED_PIN LED_BUILTIN

// === Globals ===
I2S i2s = I2S(OUTPUT);
const long kSampleRate = 22050;

size_t samplePointer = 0;               // Fixed-point pointer for resampling
size_t pitchStep_resample_fixed = 150;  // Step size for resampling (Q8.8) --> -2 octaves with pitch error <= 5 cents; -3 oct < 15 cents
uint16_t pitchStep_shift_fixed = 256;   // Step size for pitch shifting (Q8.8)


PicoLCD_I2C lcd(0, 0x27, 4, 5);
Encoder encoder;

// Track last displayed state
int lastIndex = -1;
int lastValue = -9999;
MenuMode lastMode = EDIT;

Sequencer seq;
SampleVoice kickVoice;
SampleVoice snareVoice;
SampleVoice chVoice;   // Closed Hi-hat
SampleVoice ohVoice;   // Open Hi-hat
SampleVoice tomVoice;  // Mid Tom


// === Core 0 Setup handles GUI ===
void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  encoder.begin(10, 11, 12);
  lcd.begin();

  showSplashScreen();
}

// === Core 1 Setup handles audio ===
void setup1() {
  kickVoice.init(kickSample, kickLength);
  snareVoice.init(snareSample, snareLength);
  chVoice.init(chSample, chLength);
  ohVoice.init(ohSample, ohLength);
  tomVoice.init(tomSample, tomLength);

  i2s.setBitsPerSample(16);
  i2s.setBuffers(4, 64);
  i2s.begin(kSampleRate);

  seq.setupSequence();
}


// === Core 0 Loop handles GUI ===
void loop() {
  static unsigned long last = 0;
  unsigned long now;

  now = millis();
  if (now - last > 2) {  // button scanning frequency
    encoder.updateEncoder();
    if (mode == SCROLL) {
      handleScrollMode();
    } else {
      handleEditMode();
    }
    updateDisplay();
    last = now;
  }
}

// === Core 1 Loop handles audio ===
void loop1() {
  int16_t sample;
  int16_t stereoFrame[2];


  while (true) {
    // poll the sequencer
    seq.poll();
    seq.processQueue();

    // Apply resampling (pitch + speed)
    sample = (kickVoice.render(256) >> 2) + (snareVoice.render(256) >> 2) + (chVoice.render(256) >> 2) + (ohVoice.render(256) >> 2) + (tomVoice.render(params[0].value) >> 2);
    sample = sample >> 1;

    // Apply pitch shifting (pitch only)
    sample = pitchShift(sample, params[1].value, MAX_BUFFER_SIZE / 2);

    // Apply low pass filter
    sample = lowPassFilterMultiPole(sample, params[3].value, params[2].value, params[4].value);  // 64=~880 Hz

    // Apply overdrive
    //sample = overdrive(sample, params[5].value, params[6].value);

    // Scale down volume and prepare stereo frame
    stereoFrame[0] = sample / 2;
    stereoFrame[1] = stereoFrame[0];

    // Write frame to I2S output
    while (!i2s.write(*(int32_t*)stereoFrame, false)) {  // this loop runs while i2s is happy
    }

    updateLEDStatus();  // Show underflow status via LED
  }
  i2s.end();  // Not reached
}
