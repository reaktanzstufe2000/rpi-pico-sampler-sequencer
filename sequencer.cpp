#include "Sequencer.h"

// MIDIQueue methods
bool MidiQueue::enqueue(byte note, byte velocity, byte channel) {
  int nextTail = (tail + 1) % SIZE;
  if (nextTail == head) return false;  // full
  buffer[tail] = { note, velocity, channel };
  tail = nextTail;
  return true;
}

bool MidiQueue::dequeue(MidiEvent &event) {
  if (head == tail) return false;  // empty
  event = buffer[head];
  head = (head + 1) % SIZE;
  return true;
}

bool MidiQueue::isEmpty() const {
  return head == tail;
}

void MidiQueue::clear() {
  head = tail = 0;
}

// Sequencer methods

// funky init
void Sequencer::setupSequence() {
  for (int i = 0; i < 32; i++) {
    // Kick: solid on every beat (0, 4, 8, ..., 28)
    sequence[0][i] = { 36, (i % 4 == 0) ? 127 : 0 };

    // Snare: on every second kick (steps 4, 12, 20, 28)
    bool isSnareStep = (i % 8 == 4);
    uint8_t snareVel = isSnareStep ? 100 : 0;

    // Ghost snares in steps 16–31, excluding main snare hits
    if (i >= 16 && !isSnareStep && random(0, 100) < 30) {
      snareVel = random(40, 80);
    }
    sequence[1][i] = { 38, snareVel };

    // Open Hi-hat: sparkle between beats (2, 6, 10, ..., 30)
    sequence[3][i] = { 46, (i % 4 == 2) ? 70 : 0 };

    // Closed Hi-hat: randomly on beat or step after, with varying velocity
    bool chActive = (i % 4 == 0 || i % 4 == 1) && random(0, 100) < 50;
    uint8_t chVel = chActive ? random(30, 90) : 0;
    sequence[2][i] = { 42, chVel };

    // Mid Tom: fills on some empty steps for wiggle
    bool kickHere = (i % 4 == 0);
    bool snareHere = (sequence[1][i].velocity > 0);
    bool ohHere = (i % 4 == 2);
    bool chHere = (sequence[2][i].velocity > 0);

    bool empty = !kickHere && !snareHere && !ohHere && !chHere;
    sequence[4][i] = { 47, (empty && random(0, 100) < 40) ? random(60, 100) : 0 };
  }
}


// void Sequencer::poll() {
//   unsigned long now = micros();

//   // Handle STOP and PAUSE states
//   if (state == SEQ_STOP) {
//     currentStep = 0;
//     return;
//   }
//   if (state == SEQ_PAUSE) {
//     return;
//   }

//   // Only proceed if we're in START mode
//   int tempo = params[PARAM_TEMPO].value;  // BPM from GUI
//   int swing = params[PARAM_SWING].value;  // 0–100 from GUI

//   unsigned long step_interval_us = 60000000 / tempo / PPQN;  // microseconds per step

//   bool isSwingStep = (currentStep % 2 == 1);                         // every second step is swing step
//   unsigned long swing_offset_us = (step_interval_us * swing) / 100;  // Apply swing offset to base interval
//   unsigned long interval = isSwingStep
//                              ? step_interval_us + swing_offset_us
//                              : step_interval_us - swing_offset_us;

//   if (now >= nextStepTime) {
//     for (int i = 0; i < INSTRUMENTS; i++) {
//       Step s = sequence[i][currentStep];
//       if (s.velocity > 0) {
//         midiOut.enqueue(s.note, s.velocity, MIDI_CHANNEL);
//       }
//     }

//     currentStep = (currentStep + 1) % STEPS;
//     nextStepTime += interval;
//   }

// }



void Sequencer::processQueue() {
  MidiEvent evt;
  while (midiOut.dequeue(evt)) {
    switch (evt.note) {
      case 36:  // Kick
        kickVoice.trigger(evt.velocity);
        break;
      case 38:  // Snare
        snareVoice.trigger(evt.velocity);
        break;
      case 42:  // Closed Hi-hat
        chVoice.trigger(evt.velocity);
        break;
      case 46:  // Open Hi-hat
        ohVoice.trigger(evt.velocity);
        break;
      case 47:  // Mid Tom
        tomVoice.trigger(evt.velocity);
        break;
        // Add more mappings as needed
    }
  }
}


void Sequencer::poll(SequencerState state) {
  unsigned long now = micros();

  // Handle sequencer state
  switch (state) {
    case SEQ_STOP:
      currentStep = 0;
      return;

    case SEQ_PAUSE:
      return;

    case SEQ_START:
      nextStepTime = now;  // reset timeline on start
      break;
  }

  // Get tempo and swing parameters
  int tempo = params[PARAM_TEMPO].value;  // BPM from GUI
  int swing = params[PARAM_SWING].value;  // 0–100 from GUI

  // Calculate base step interval in microseconds
  unsigned long step_interval_us = 60000000 / tempo / PPQN;

  // Determine if current step is a swing step
  bool isSwingStep = (currentStep % 2 == 1);

  // Apply swing offset
  unsigned long swing_offset_us = (step_interval_us * swing) / 100;
  unsigned long interval = isSwingStep // if this one is a swing step, the next on will be closer
                             ? step_interval_us - swing_offset_us
                             : step_interval_us + swing_offset_us;

  // Trigger step if scheduled time has arrived
  if (now >= nextStepTime) {
    for (int i = 0; i < INSTRUMENTS; i++) {
      Step s = sequence[i][currentStep];
      if (s.velocity > 0) {
        midiOut.enqueue(s.note, s.velocity, MIDI_CHANNEL);
      }
    }

    currentStep = (currentStep + 1) % STEPS;
    nextStepTime += interval;
  }
}