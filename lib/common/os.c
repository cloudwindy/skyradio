#include "os.h"
#include "printf.h"

#include "FreeRTOS.h"
#include "FreeRTOS/task.h"

/**
 * Wrapper of FreeRTOS vTaskStartScheduler.
 */
void os_start(void) { vTaskStartScheduler(); }

/**
 * Wrapper for vTaskDelay.
 *
 * Do not use this in setup functions.
 */
void os_sleep(uint32_t ms) { vTaskDelay(pdMS_TO_TICKS(ms)); }

/**
 * Wrapper of FreeRTOS xTaskCreate.
 *
 * Priority defaults to 2 if zero is given.
 */
void os_exec(const char *name, void (*task_p)(void *), void *const args,
             uint32_t priority)
{
  if (priority == 0)
  {
    priority = 2;
  }
  if (priority >= configMAX_PRIORITIES)
  {
    printf("os_exec: invalid priority");
    return;
  }
  xTaskCreate(task_p, name, configMINIMAL_STACK_SIZE, args, priority, NULL);
}

/**
 * Wrapper of FreeRTOS vTaskDelete.
 */
void os_exit(void) { vTaskDelete(NULL); }

/**
 * Wrapper of FreeRTOS vPortEnterCritical.
 */
void os_mutex_lock(void) { vPortEnterCritical(); }

/**
 * Wrapper of FreeRTOS vPortExitCritical.
 */
void os_mutex_unlock(void) { vPortExitCritical(); }
