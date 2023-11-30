#include "setup.h"
#include "os.h"
#include "tasks.h"
#include "printf.h"

int main(void)
{
  setup();

  /**
   * This task won't not run immediately because RTOS
   * isn't started yet.
  */
  os_exec("task_init", task_init, NULL, 0);
  /**
   * Start up RTOS.
   *
   * It will configure the SysTick by itself.
   */
  os_start();
  printf("os shut down.\n");
  /* Wait for IWDG reset. */
  for (;;)
    ;
  return 0;
}

