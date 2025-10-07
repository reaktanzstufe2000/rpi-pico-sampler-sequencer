#ifndef GUI_H
#define GUI_H

#include <Arduino.h>
#include "Encoder.h"
#include <PicoLCD_I2C.h>

#define N_PARAM 7

// === External Globals ===
extern PicoLCD_I2C lcd;
extern Encoder encoder;

enum MenuMode { SCROLL, EDIT };

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

#endif // GUI_H