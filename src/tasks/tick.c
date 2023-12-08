#include "app.h"
#include "os.h"
#include "tasks.h"

#include <stdbool.h>
#include <stddef.h>

void task_tick(void *args __attribute__((unused)))
{
  while (true)
  {
    app_handler(ev_tick, NULL);
    os_sleep(APP_TICK);
  }
}
