#ifndef GUI_H
#define GUI_H

#include <Arduino.h>
#include "Encoder.h"
#include <PicoLCD_I2C.h>


// === External Globals ===
extern PicoLCD_I2C lcd;
extern Encoder encoder;

enum MenuMode { SCROLL,
                EDIT };

enum ParamIndex {
  PARAM_TEMPO,
  PARAM_SWING,
  PARAM_RESAMPLE,
  PARAM_PITCH_SHIFT,
  PARAM_FILTER_POLES,
  PARAM_CUTOFF,
  PARAM_RESONANCE,
  PARAM_GAIN,
  PARAM_DRIVE,
  N_PARAM
};

enum SequencerState {
  SEQ_STOP,
  SEQ_PAUSE,
  SEQ_START,
  SEQ_PLAYING
};


struct Parameter {
  const char* name;
  int value;
  int min;
  int max;
};

extern Parameter params[N_PARAM];
extern int selectedIndex;
extern int editValue;
extern MenuMode mode;

// === GUI Functions ===
void showSplashScreen();
void handleEditMode(bool resetTimer = false);
void handleScrollMode();
void updateDisplay();

#endif  // GUI_H