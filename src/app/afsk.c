#include "afsk.h"
#include "delay.h"
#include "printf.h"
#include "pwm.h"

#include <libopencm3/stm32/gpio.h>

void test_tx(void)
{
  PwmGenerator gen = {
    .gpio_port = GPIOB,
    .gpios = GPIO1,
    .low_freq = 1200,
    .high_freq = 2200,
    .default_cycle = 128,
  };
  pwm_init(&gen);

  while (true)
  {
    gpio_set(GPIOB, GPIO1);
    mdelay(3);
    gpio_clear(GPIOB, GPIO1);
    mdelay(3);
  }
  // pwm_gen_rt(&gen, 1200, 1000000, 0);
  // printf("Font TX\n");
  // for (size_t i = 0; i < 1520; i++)
  // {
  //   iwdg_reset();
  //   pwm_gen_rt(&gen, 1000, 5, 0);
  //   pwm_send(&gen, Font.data[i], 5, 5);
  //   printf("Byte %i TX\n", i + 1);
  // }
  printf("Done\n");
}
