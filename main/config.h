#define BAUD 1000000 // 115200 is the default for nice nano, a higher value is better for efficency, but is less resistent to noise.

// set this option to use defer based debounce like zmk instead of eager debounce
//#define DEFER_DEBOUNCE

// the number you need to the current position to go to the next row given position = row*MAX_COLS+column
#define ROW_STRIDE 12

#define KSCAN_DEBOUNCE_PRESS_MS 6
#define KSCAN_DEBOUNCE_RELEASE_MS 5

// this defines number of milliseconds to wait between each scan
#define KSCAN_SCAN_PERIOD 1

// the top left row and column of this keyboard in the KEYMAP config
#define ROW_OFFSET 0
#define COLUMN_OFFSET 0

// this specifies which pins are used for the rows and columns, and status led if desired
#if defined(__AVR_ATmega328P__)
#define NUM_USED_ROWS 5
#define NUM_USED_COLS 6  

#define LED LED_BUILTIN
#define NUM_USED_ROWS 5
#define NUM_USED_COLS 6  

#define ROWS {PD, GPIN2|GPIN3|GPIN4|GPIN5|GPIN6}
#define COLUMNS D7,{PB, GPIN0|GPIN1|GPIN2|GPIN3|GPIN4}
// this is equivilent to the definitions below, but it is faster
//#define ROWS D2,D3,D4,D5,D6
//#define COLUMNS D7,D8,D9,D10,D11,D12
#else
#define NUM_USED_ROWS 5
#define NUM_USED_COLS 6  

#define LED P2_6
#define ROWS {PORT1, GPIN0|GPIN3|GPIN4|GPIN5|GPIN6/*|GPIN7*/}
#define COLUMNS {PORT2, GPIN0|GPIN1|GPIN2|GPIN3|GPIN4|GPIN5/*|GPIN6|GPIN7*/}
#endif

// this sets the diode direction to ground on rows, otherwise ground on columns
#define COL2ROW

// this defines the position of the keys in the whole keyboard, in order to compute the mapping between a key position and the value it represents to zmk
#define KEYMAP \
RC(0,0) RC(0,1) RC(0,2) RC(0,3) RC(0,4) RC(0,5)         RC(0,6)  RC(0,7) RC(0,8) RC(0,9) RC(0,10) RC(0,11) \
RC(1,0) RC(1,1) RC(1,2) RC(1,3) RC(1,4) RC(1,5)         RC(1,6)  RC(1,7) RC(1,8) RC(1,9) RC(1,10) RC(1,11) \
RC(2,0) RC(2,1) RC(2,2) RC(2,3) RC(2,4) RC(2,5)         RC(2,6)  RC(2,7) RC(2,8) RC(2,9) RC(2,10) RC(2,11) \
RC(3,0) RC(3,1) RC(3,2) RC(3,3) RC(3,4) RC(3,5)         RC(3,6)  RC(3,7) RC(3,8) RC(3,9) RC(3,10) RC(3,11) \
                        RC(4,3) RC(4,4) RC(4,1)         RC(4,7)  RC(4,8)                                   \
                                RC(4,5) RC(4,2)         RC(4,6)
