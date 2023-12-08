#include "os.h"
#include "tasks.h"

#include <libopencm3/stm32/iwdg.h>

void task_watchdog(void *args __attribute__((unused)))
{
  while (true)
  {
    iwdg_reset();
    os_sleep(50);
  }
}
