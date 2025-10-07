#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <Arduino.h>
#include "sampler.h"
#include "gui.h"

extern SampleVoice kickVoice;
extern SampleVoice snareVoice;
extern SampleVoice chVoice;   // Closed Hi-hat
extern SampleVoice ohVoice;   // Open Hi-hat
extern SampleVoice tomVoice;  // Mid Tom
extern Parameter params[N_PARAM];

// Constants
const int BPM = 120;
const int STEPS = 32;
const int INSTRUMENTS = 5;
const int PPQN = 4;

const byte MIDI_CHANNEL = 1;

// Step structure
struct Step {
  byte note;
  byte velocity;
};

// MIDI event structure
struct MidiEvent {
  byte note;
  byte velocity;
  byte channel;
};

// MIDI queue class
class MidiQueue {
public:
  static const int SIZE = 16;
  MidiEvent buffer[SIZE];
  int head = 0;
  int tail = 0;

  bool enqueue(byte note, byte velocity, byte channel);
  bool dequeue(MidiEvent &event);
  bool isEmpty() const;
  void clear();
};

// Sequencer class
class Sequencer {
public:
  Step sequence[INSTRUMENTS][STEPS];
  SequencerState state = SEQ_STOP;
  unsigned long nextStepTime = 0;
  int currentStep = 0;
  MidiQueue midiOut;

  void setupSequence();
  void poll(SequencerState state);
  void processQueue();
};

#endif // SEQUENCER_H