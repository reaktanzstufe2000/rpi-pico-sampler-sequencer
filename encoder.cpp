#include "Encoder.h"
#include <Arduino.h>

void Encoder::begin(int clkPin, int dtPin, int swPin) {
  clk = clkPin;
  dt = dtPin;
  sw = swPin;

  pinMode(clk, INPUT);
  pinMode(dt, INPUT);
  pinMode(sw, INPUT_PULLUP);

  lastCLK = digitalRead(clk);
  lastSW = digitalRead(sw);
}

void Encoder::updateEncoder() {
  rotate = 0;
  clicked = false;
  longPress = false;

  unsigned long now = millis();

  // Rotation detection (falling edge only)
  int currentCLK = digitalRead(clk);
  if (lastCLK == HIGH && currentCLK == LOW) {
    rotate = (digitalRead(dt) == LOW) ? -1 : +1;
  }
  lastCLK = currentCLK;

  // Button state
  bool currentSW = digitalRead(sw);

  // Detect press start
  if (currentSW == LOW && lastSW == HIGH) {
    pressStart = now;
    isPressing = true;
  }

  // Detect release
  if (currentSW == HIGH && lastSW == LOW) {
    if (isPressing && (now - pressStart < longPressThreshold)) {
      clicked = true;
    }
    isPressing = false;
    pressStart = 0;
  }

  // Detect long press
  if (currentSW == LOW && isPressing && (now - pressStart >= longPressThreshold)) {
    longPress = true;
  }

  lastSW = currentSW;
}