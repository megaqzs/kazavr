#include <zmk/debounce.h>
#include "config.h"
#include <quirks.h>
#include <pins.h>
#include <wire.h>

#ifdef COL2ROW
#define KS_ACTIVE LOW
#else
#define KS_ACTIVE HIGH
#endif
// ---------- CONFIG ----------
static const pingroup_t rowPins[] = {ROWS};     // digital pins used for rows
static const pingroup_t colPins[] = {COLUMNS};     // digital pins used for columns

static const struct zmk_debounce_config debounce_config = {
    KSCAN_DEBOUNCE_PRESS_MS,
    KSCAN_DEBOUNCE_RELEASE_MS,
};

// ----------------------------

// Internal state array
static struct zmk_debounce_state matrix_state[NUM_USED_ROWS][NUM_USED_COLS] = {0};

// Helper: read columns while row `r` is driven active (others inactive), returns whether or not any switch is active, good luck reading this pile of garbage
bool matrix_scan(const pingroup_t **sgrps, unsigned int *sgrps_size, bool *scan_dir) {
  bool ret = 0;
  int scratchpad[sizeof(colPins)/sizeof(colPins[0])];
  uint8_t c, cg, r = 0, rg;
  int cnxt,rnxt;

  if (sgrps) {
    *sgrps = colPins;
    *sgrps_size = sizeof(colPins)/sizeof(colPins[0]);
    *scan_dir = KS_ACTIVE;
  }

  for (rg = 0; rg < sizeof(rowPins)/sizeof(rowPins[0]); rg++) {
    pingroup_t row = {rowPins[rg].group, 0};
    rnxt = rowPins[rg].pins;
    FOR_GROUP_PINS(rnxt, r) {
      row.pins = ACTIVE_LSB(rnxt);
      // Drive current row active
      setPinState(row, KS_ACTIVE);
      // Read columns
      for (cg = 0; cg < sizeof(colPins)/sizeof(colPins[0]); ++cg) {
        scratchpad[cg] = getPinState(colPins[cg]);
      }
      // Drive current row inactive as soon as possible to avoid resistive losses in case a switch is pressed
      setPinState(row, !KS_ACTIVE);
      c = 0;
      for (cg = 0; cg < sizeof(colPins)/sizeof(colPins[0]); cg++) {
        cnxt = colPins[cg].pins;
        FOR_GROUP_PINS(cnxt, c) {
          // if column reads LOW => switch is connecting row to column => pressed

          zmk_debounce_update(&matrix_state[r][c], !!KS_ACTIVE == !!(scratchpad[cg] & ACTIVE_LSB(cnxt)), KSCAN_SCAN_PERIOD, &debounce_config);
          if (zmk_debounce_get_changed(&matrix_state[r][c])) {
              wpos_upd(r, c, zmk_debounce_is_pressed(&matrix_state[r][c]));
          }
          ret |= zmk_debounce_is_active(&matrix_state[r][c]);
        }
      }
    }
  }
  return ret;
}

#ifdef PG_ISR
void matrix_passive_scan(bool passive) {
  uint8_t rg = 0;
  // set all rows to active if using passive scan and inactive otherwise
  for (rg = 0; rg < sizeof(rowPins)/sizeof(rowPins[0]); ++rg) {
    setPinState(rowPins[rg], !passive ^ !!KS_ACTIVE);
  }
}
#endif

void matrix_init() {
  // Setup columns as inputs with pull-ups (idle HIGH)
  uint8_t c = 0, r = 0;
  for (c = 0; c < sizeof(colPins)/sizeof(colPins[0]); ++c) {
    setPinDir(colPins[c], INPUT);
    setPinPullEn(colPins[c], 1);
    setPinPull(colPins[c], !KS_ACTIVE);
  }

  // Initialize rows as outputs, set HIGH (inactive)
  for (r = 0; r < sizeof(rowPins)/sizeof(rowPins[0]); ++r) {
    setPinDir(rowPins[r], OUTPUT);
    setPinState(rowPins[r], !KS_ACTIVE);
  }
}
