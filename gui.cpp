#include "gui.h"

int originalValue = 0;

// === GUI State ===
Parameter params[N_PARAM] = {
  { "Resample", 256, 1, 2048 },
  { "Pitch Shift", 256, 1, 2048 },
  { "Filter Poles", 4, 0, 4 },
  { "Cutoff", 255, 0, 255 },
  { "Resonance", 0, 0, 255 },
  { "Gain", 0, 0, 255 },
  { "Drive", 0, 0, 255 }
};

int selectedIndex = 0;
int editValue = 0;
MenuMode mode = SCROLL;

// Track last displayed state
static int lastIndex = -1;
static int lastValue = -9999;
static MenuMode lastMode = EDIT;

void showSplashScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PS 2100");
  lcd.setCursor(0, 1);
  lcd.print("pocket sampler");
  delay(1000);

  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Value:");
}

void handleEditMode(bool resetTimer) {
  static unsigned long lastActivity = 0;
  unsigned long now = millis();

  if (resetTimer) {
    lastActivity = now;
    return;
  }

  // Timeout: exit edit mode after 10 seconds of inactivity
  if (now - lastActivity > 10000) {
    mode = SCROLL;
    return;
  }

  // Detect activity
  if (encoder.rotate != 0 || encoder.clicked || encoder.longPress) {
    lastActivity = now;
  }

  // Live update parameter value
  if (encoder.rotate != 0) {
    params[selectedIndex].value += encoder.rotate;
    params[selectedIndex].value = constrain(
      params[selectedIndex].value,
      params[selectedIndex].min,
      params[selectedIndex].max);

    // Keep editValue in sync for display
    editValue = params[selectedIndex].value;
  }

  // Cancel edit (restore previous value — optional)
  if (encoder.longPress) {
    params[selectedIndex].value = originalValue;
    editValue = originalValue;
    mode = SCROLL;
  }

  // Confirm edit
  if (encoder.clicked) {
    mode = SCROLL;
  }
}

void handleScrollMode() {
  if (encoder.rotate != 0) {
    selectedIndex += encoder.rotate;
    if (selectedIndex < 0) selectedIndex = N_PARAM - 1;
    if (selectedIndex > N_PARAM - 1) selectedIndex = 0;
  }

  if (encoder.clicked) {
    mode = EDIT;
    originalValue = params[selectedIndex].value;
    editValue = originalValue;
    handleEditMode(true);  // reset timer
  }
}

void updateDisplay() {
  static bool blinkState = false;
  static unsigned long lastBlink = 0;
  unsigned long now = millis();

  int currentValue = (mode == EDIT ? editValue : params[selectedIndex].value);

  if (selectedIndex != lastIndex) {
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 0);
    lcd.print(params[selectedIndex].name);
    lastIndex = selectedIndex;
  }

  if (currentValue != lastValue) {
    lcd.setCursor(7, 1);
    lcd.print("     ");
    lcd.setCursor(7, 1);
    char buf[10];
    itoa(currentValue, buf, 10);
    lcd.print(buf);
    lastValue = currentValue;
  }

  // Blinking edit indicator
  if (mode == EDIT) {
    if (now - lastBlink > 500) {
      blinkState = !blinkState;
      lcd.setCursor(15, 0);
      lcd.write(blinkState ? '*' : ' ');
      lastBlink = now;
    }
  } else if (lastMode == EDIT) {
    lcd.setCursor(15, 0);
    lcd.write(' ');
  }

  lastMode = mode;
}