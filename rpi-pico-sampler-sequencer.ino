/*
 * This rpi-pico-sampler provides you with nice 909 beats!
 * Copyright (C) 2025 reaktanzstufe2000
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
*/

#include <Arduino.h>
#include <PicoLCD_I2C.h>
#include <I2S.h>
#include "tr909.h"
#include "Encoder.h"
#include "gui.h"
#include "sampler.h"
#include "sequencer.h"



// === Constants ===
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

  int flonkus = 1;

  while (true) {
    // poll the sequencer
    if (flonkus) {
      seq.poll(SEQ_START);
      flonkus = 0;
    } else {
      seq.poll(SEQ_PLAYING);
    }
    seq.processQueue();

    // Apply resampling (pitch + speed)
    sample = (kickVoice.render(256) >> 2) + (snareVoice.render(256) >> 2) + (chVoice.render(256) >> 2) + (ohVoice.render(256) >> 2) + (tomVoice.render(params[PARAM_RESAMPLE].value) >> 2);
    sample = sample >> 1;

    // Apply pitch shifting (pitch only)
    sample = pitchShift(sample, params[PARAM_PITCH_SHIFT].value, MAX_BUFFER_SIZE / 2);

    // Apply low pass filter
    sample = lowPassFilterMultiPole(sample, params[PARAM_CUTOFF].value, params[PARAM_FILTER_POLES].value, params[PARAM_RESONANCE].value);  // 64=~880 Hz

    // Apply overdrive
    //sample = overdrive(sample, params[PARAM_GAIN].value, params[PARAM_DRIVE].value);

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
