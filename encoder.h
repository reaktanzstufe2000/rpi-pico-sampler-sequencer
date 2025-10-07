#ifndef ENCODER_H
#define ENCODER_H

class Encoder {
public:
  int rotate = 0;
  bool clicked = false;
  bool longPress = false;

  void begin(int clkPin, int dtPin, int swPin);
  void updateEncoder();

private:
  int clk, dt, sw;
  int lastCLK;
  bool lastSW;

  unsigned long pressStart = 0;
  const unsigned long longPressThreshold = 1000;
  bool isPressing = false;
};

#endif