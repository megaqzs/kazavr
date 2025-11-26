#include <Arduino.h>
#include <zmk/debounce.h>
#include "config.h"
#include "wire.h"

// ---------- CONFIG ----------
const uint8_t rowPins[] = {ROWS};     // Arduino digital pins used for rows
const uint8_t colPins[] = {COLUMNS};     // Arduino digital pins used for columns

struct zmk_debounce_config debounce_config = {
    KSCAN_DEBOUNCE_PRESS_MS,
    KSCAN_DEBOUNCE_RELEASE_MS,
};

// ----------------------------

// Internal state array
struct zmk_debounce_state matrix_state[sizeof(rowPins)][sizeof(colPins)] = {0};

unsigned long lastScanMs = 0;

// Helper: read columns while row `r` is driven LOW (others high)
uint8_t readRow(uint8_t r) {
  // Drive current row LOW
  digitalWrite(rowPins[r], LOW);
  // Read columns. Columns use INPUT_PULLUP, so idle=HIGH, pressed=LOW
  uint8_t bits = 0;
  for (uint8_t c = 0; c < sizeof(colPins); ++c) {
    int v = digitalRead(colPins[c]);
    // if column reads LOW => switch is connecting row to column => pressed
    zmk_debounce_update(&matrix_state[r][c], !v, KSCAN_SCAN_PERIOD, &debounce_config);
  }

  // Drive current row HIGH
  digitalWrite(rowPins[r], HIGH);
  return bits;
}

void setupPins() {
  // Setup columns as inputs with pull-ups (idle HIGH)
  for (uint8_t c = 0; c < sizeof(colPins); ++c) {
    pinMode(colPins[c], INPUT_PULLUP);
  }

  // Initialize rows as outputs, set HIGH (inactive)
  for (uint8_t r = 0; r < sizeof(rowPins); ++r) {
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], HIGH);
  }
  // Initialize internal states
  lastScanMs = millis();
}

void scan_loop() {
  for (uint8_t r = 0; r < sizeof(rowPins); ++r) {
    readRow(r);

    for (uint8_t c = 0; c < sizeof(colPins); ++c) {
      if (zmk_debounce_get_changed(&matrix_state[r][c]))
        wpos_upd(r, c, zmk_debounce_is_pressed(&matrix_state[r][c]));
    }
  }
}
