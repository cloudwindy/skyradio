#include "led.h"
#include "os.h"
#include "pwm.h"

static PwmGenerator led_pwm_gen = {
  .gpio_port = LED_BANK,
  .gpios = LED,
};

void led_blink(void)
{
  led_on();
  os_sleep(LED_DELAY);
  led_off();
  os_sleep(LED_DELAY);
}

void led_on(void) { gpio_clear(LED_BANK, LED); }

void led_off(void) { gpio_set(LED_BANK, LED); }

void led_pwm_dimming(int brightness)
{
  pwm_gen_rt(&led_pwm_gen, 200, 1000, brightness);
}