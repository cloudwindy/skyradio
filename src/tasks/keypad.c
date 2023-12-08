#include "tasks.h"

#include "app.h"
#include "keypad.h"
#include "os.h"
#include "printf.h"
#include "state.h"

/* Keypad scan routine. */
void task_keypad(void *args __attribute__((unused)))
{
  char holding_key = '\0';
  uint32_t hold_time = 0;
  uint32_t hold_counter = 0;
  uint32_t hold_repeat_time = 0;
  while (true)
  {
    char key = keypad_getchar();
    if (key != '\0' && key == holding_key)
    {
      /* Key is held. */
      hold_time += KEYPAD_TICK;
      if (hold_time > g_st.ui.hold_delay)
      {
        /* This is a long press holding. */
        hold_repeat_time += KEYPAD_TICK;
        if (hold_repeat_time > KEYPAD_REPEAT_INTERVAL)
        {
          EvKey ev = {
            .key = holding_key,
            .key_press = kp_long_holding,
            .hold_time = hold_counter,
          };
          app_handler(ev_key, &ev);
          hold_counter++;
          hold_repeat_time = 0;
        }
      }
    }
    else if (key != '\0')
    {
      /* A new key is pressed. */
      EvKey ev = {
        .key = key,
        .key_press = kp_pressed,
        .hold_time = 0,
      };
      app_handler(ev_key, &ev);

      holding_key = key;
      /**
       * If key is held for shorter than KEYPAD_TICK,
       * we consider this a bounce.
       */
      hold_time = 0;
    }
    else if (hold_time > 0)
    {
      /* Key is released. */
      EvKey ev = {
        .key = holding_key,
        .hold_time = 0,
      };
      if (hold_time > g_st.ui.hold_delay)
      {
        /* This was a long press. */
        ev.key_press = kp_long_released;
      }
      else
      {
        /* This was a short press. */
        ev.key_press = kp_short_released;
      }
      app_handler(ev_key, &ev);
      holding_key = '\0';
      hold_time = 0;
      hold_repeat_time = 0;
    }
    os_sleep(KEYPAD_TICK);
  }
}
