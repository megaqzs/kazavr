#include <hal/msp430/delay.h>
#include <hal/atmega328p/delay.h>

#if 0 // currently not used
// this isn't perfect, but it is good enough in most likelyhood, and assuming no
// advanced features like branch prediction
void cdelay(unsigned long long cycles) [[always_inline]] {
    for (; cycles > CDELAY_LOOP_INIT; cycles -= CDELAY_LOOP_TIME)
        cdelayc(0);
}

#define mdelay(x) cdelay((x)*F_CPU/1000)
#define udelay(x) cdelay((x)*F_CPU/1000000)
#endif

#define mdelayc(x) cdelayc(((unsigned long long) F_CPU)*(x)/1000)
#define udelayc(x) cdelayc(((unsigned long long) F_CPU)*(x)/1000000)
