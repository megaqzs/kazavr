#include <stdint.h>
#include <utils.h>
#include <map.h>
#include "config.h"

#define MAPPER(...) MAP_UD_I(ENTRY, __VA_ARGS__)
#define RC(r, c) ,(KEY_I(r,c))
#define ENTRY(c, data, i) [c] = i+1,

static const int mapping[] = {
    MAPPER(KEYMAP)
};


int get_keycode(uint8_t row, uint8_t column) {
    int index = KEY_I(row+ROW_OFFSET, column+COLUMN_OFFSET);
    if (index >= sizeof(mapping)/sizeof(mapping[0]))
        return -1;
    return mapping[index]-1;
}
