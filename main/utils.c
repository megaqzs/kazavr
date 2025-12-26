#include <stdint.h>
#include <utils.h>
#include <map.h>
#include "config.h"

#define MAPPER(...) MAP_UD_I(ENTRY, __VA_ARGS__)
#define RC(r, c) ,(KEY_I(r,c)) // this is the macro that is used in the config file for the mapping
#define ENTRY(c, data, i) [c] = i+1, // this is the format of an entry in the mapping array

// this is the mapping between the physical key position to the keycodes that are sent to zmk.
static const int mapping[] = {
    MAPPER(KEYMAP)
};


// get the keycode for a given physical row and coloumn.
int get_keycode(uint8_t row, uint8_t column) {
    int index = KEY_I(row+ROW_OFFSET, column+COLUMN_OFFSET);
    if (index >= sizeof(mapping)/sizeof(mapping[0]))
        return -1;
    return mapping[index]-1;
}
