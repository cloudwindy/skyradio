#include "delay.h"

#include "FreeRTOS.h"
#include "FreeRTOS/task.h"

#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/timer.h>

#define DELAY_PRECISE_TIMER (TIM3)

/**
 * Sleep using precise timer.
 */
void mdelay(uint32_t ms)
{
  for (uint32_t i = 0; i < ms; i++)
    udelay(1000);
}

/**
 * Sleep using precise timer.
 */
void udelay(uint16_t us)
{
  timer_set_counter(DELAY_PRECISE_TIMER, 0);
  while (timer_get_counter(DELAY_PRECISE_TIMER) < us)
    ;
}
