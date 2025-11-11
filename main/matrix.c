#include <Arduino.h>
#include "config.h"
#include "wire.h"

// ---------- CONFIG ----------
// Adjust pins to match your hardware
const uint8_t rowPins[] = {ROWS};     // Arduino digital pins used for rows
const uint8_t colPins[] = {COLUMNS};     // Arduino digital pins used for columns

// Debounce time in milliseconds
const unsigned long DEBOUNCE_MS = 8;

// How often we run a full scan (ms) - scanner uses active row strobing, but this caps UI printing
const unsigned long SCAN_INTERVAL_MS = 5;

// ----------------------------

// Internal state arrays
uint8_t lastRaw[sizeof(rowPins)];          // last raw bitmask read for each row
uint8_t debounced[sizeof(colPins)];        // stable debounced state (bit = 1 when pressed)
unsigned long lastChangeMs[sizeof(rowPins)]; // last time the raw value changed for that row

unsigned long lastScanMs = 0;

// Helper: read columns while row `r` is driven LOW (others high)
uint8_t readRowRaw(uint8_t r) {
  // Drive all rows HIGH (inactive)
  for (uint8_t i = 0; i < sizeof(rowPins); ++i) {
    digitalWrite(rowPins[i], HIGH);
    pinMode(rowPins[i], OUTPUT);
  }

  // Drive current row LOW
  digitalWrite(rowPins[r], LOW);

  // Small short delay to let signals settle
  // (we keep it tiny; the scan interval and debounce handle stability)
  delayMicroseconds(5);

  // Read columns. Columns use INPUT_PULLUP, so idle=HIGH, pressed=LOW
  uint8_t bits = 0;
  for (uint8_t c = 0; c < sizeof(colPins); ++c) {
    int v = digitalRead(colPins[c]);
    // if column reads LOW => switch is connecting row to column => pressed
    if (v == LOW) {
      bits |= (1 << c);
    }
  }

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
  for (uint8_t r = 0; r < sizeof(rowPins); ++r) {
    lastRaw[r] = 0;
    debounced[r] = 0;
    lastChangeMs[r] = millis();
  }
  lastScanMs = millis();
}

void scan_loop() {
  unsigned long now = millis();
  // Keep scanning at SCAN_INTERVAL_MS
  if ((now - lastScanMs) < SCAN_INTERVAL_MS) return;
  lastScanMs = now;

  for (uint8_t r = 0; r < sizeof(rowPins); ++r) {
    uint8_t raw = readRowRaw(r); // bit=1 means pressed for that col in this row

    if (raw != lastRaw[r]) {
      // raw changed: update change timestamp and store lastRaw
      lastRaw[r] = raw;
      lastChangeMs[r] = now;
      // do not immediately update debounced state — wait for stability
    } else {
      // raw same as previous. check if stable long enough to accept.
      if ((now - lastChangeMs[r]) >= DEBOUNCE_MS) {
        // if stable and different from debounced state => update and emit events
        if (raw != debounced[r]) {
          uint8_t changedBits = raw ^ debounced[r];
          // For each column that changed, emit event
          for (uint8_t c = 0; c < sizeof(colPins); ++c) {
            uint8_t mask = (1 << c);
            if (changedBits & mask) {
              bool pressed = (raw & mask) != 0;
              wpos_upd(r, c, pressed);
            }
          }
          debounced[r] = raw;
        }
      }
    }
  }
}
