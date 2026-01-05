#include <zmk/debounce.h>
#include "config.h"
#include <quirks.h>
#include <pins.h>
#include <stdio.h>
#include <wire.h>

#ifdef COL2ROW
#define ROW_ACTIVE LOW
#else
#define ROW_ACTIVE HIGH
#endif
// ---------- CONFIG ----------
static const pingroup_t rowPins[] = {ROWS};     // Arduino digital pins used for rows
static const pingroup_t colPins[] = {COLUMNS};     // Arduino digital pins used for columns

static const struct zmk_debounce_config debounce_config = {
    KSCAN_DEBOUNCE_PRESS_MS,
    KSCAN_DEBOUNCE_RELEASE_MS,
};

// ----------------------------

// Internal state array
static struct zmk_debounce_state matrix_state[NUM_USED_ROWS][NUM_USED_COLS] = {0};

// Helper: read columns while row `r` is driven LOW (others high), returns weather or not any switch is active, good luck reading this pile of garbage
bool matrix_scan(const pingroup_t **sgrps, unsigned int *sgrps_size, bool *scan_dir) {
  bool ret = 0;
  int scratchpad[sizeof(colPins)/sizeof(colPins[0])];
  uint8_t c, cg, r = 0, rg;

  if (sgrps) {
    *sgrps = colPins;
    *sgrps_size = sizeof(colPins)/sizeof(colPins[0]);
    *scan_dir = ROW_ACTIVE;
  }

  for (rg = 0; rg < sizeof(rowPins)/sizeof(rowPins[0]); rg++) {
    pingroup_t row = {rowPins[rg].group, 0};
    for (row.pins = ACTIVE_LSB(rowPins[rg].pins); row.pins != 0;
            r++, row.pins = ACTIVE_LSB(rowPins[rg].pins & ~(row.pins ^ (row.pins-1)))) {
      // Drive current row LOW
      setPinState(row, ROW_ACTIVE);
      // Read columns. Columns use INPUT_PULLUP, so idle=HIGH, pressed=LOW
      for (cg = 0; cg < sizeof(colPins)/sizeof(colPins[0]); ++cg) {
        scratchpad[cg] = getPinState(colPins[cg]);
      }
      // Drive current row HIGH as soon as possible to avoid resistive losses
      setPinState(row, !ROW_ACTIVE);
      c = 0;
      for (cg = 0; cg < sizeof(colPins)/sizeof(colPins[0]); cg++) {
        int tv = colPins[cg].pins;
        while (tv != 0) {
          // if column reads LOW => switch is connecting row to column => pressed

          zmk_debounce_update(&matrix_state[r][c], !(scratchpad[cg] & ACTIVE_LSB(tv)), KSCAN_SCAN_PERIOD, &debounce_config);
          if (zmk_debounce_get_changed(&matrix_state[r][c])) {
              wpos_upd(r, c, zmk_debounce_is_pressed(&matrix_state[r][c]));
          }
          ret |= zmk_debounce_is_active(&matrix_state[r][c]);
          tv ^= ACTIVE_LSB(tv);
          c++;
        }
      }
    }
  }
  return ret;
}

#ifdef PG_ISR
void matrix_passive_scan(bool passive) {
  uint8_t rg = 0;
  // set HIGH (inactive)
  for (rg = 0; rg < sizeof(rowPins)/sizeof(rowPins[0]); ++rg) {
    setPinState(rowPins[rg], passive ? ROW_ACTIVE : !ROW_ACTIVE);
  }
}
#endif

void matrix_init() {
  // Setup columns as inputs with pull-ups (idle HIGH)
  uint8_t c = 0, r = 0;
  for (c = 0; c < sizeof(colPins)/sizeof(colPins[0]); ++c) {
    setPinDir(colPins[c], INPUT);
    setPinPullEn(colPins[c], 1);
    setPinPull(colPins[c], HIGH);
  }

  // Initialize rows as outputs, set HIGH (inactive)
  for (r = 0; r < sizeof(rowPins)/sizeof(rowPins[0]); ++r) {
    setPinDir(rowPins[r], OUTPUT);
    setPinState(rowPins[r], HIGH);
  }
}
