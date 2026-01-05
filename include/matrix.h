#include <stdbool.h>
#include <stdint.h>
#include <quirks.h>
#include <pins.h>
void matrix_init();

#ifdef PG_ISR
void matrix_passive_scan(bool passive);
#endif

bool matrix_scan(const pingroup_t **sgrps, unsigned int *sgrps_size, bool *scan_dir);
