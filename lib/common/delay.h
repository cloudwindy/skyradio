#ifndef DELAY_H
#define DELAY_H

#include <stdint.h>

typedef void (*interval_cb_func)(void);

void mdelay(uint32_t ms);
void udelay(uint16_t us);

#endif
