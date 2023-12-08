#include "keypad.h"
#include "delay.h"
#include "keypad_conf.h"

#define BITMASK_SWITCH(x)                                                      \
  for (uint64_t bit = 1; x >= bit; bit *= 2)                                   \
    if (x & bit)                                                               \
      switch (bit)

static uint8_t keypad_get_rows(void);
static void keypad_set_col(uint8_t col, bool set);
static void keypad_clear_cols(void);

char keypad_getchar(void)
{
  Key keys = keypad_scan();
  if (keys == 0)
  {
    return '\0';
  }
  BITMASK_SWITCH(keys)
  {
    case BIT15:
      return '1';
    case BIT14:
      return '2';
    case BIT13:
      return '3';
    case BIT12:
      return 'a';
    case BIT11:
      return '4';
    case BIT10:
      return '5';
    case BIT9:
      return '6';
    case BIT8:
      return 'b';
    case BIT7:
      return '7';
    case BIT6:
      return '8';
    case BIT5:
      return '9';
    case BIT4:
      return 'c';
    case BIT3:
      return '*';
    case BIT2:
      return '0';
    case BIT1:
      return '#';
    case BIT0:
      return 'd';
  }
  return '\0';
}

/**
 * Scan matrix keypad.
 */
Key keypad_scan(void)
{
  uint16_t keys = 0;
  uint8_t i = 4;
  while (i--)
  {
    /* Scan from 1 to 4 */
    keypad_set_col(i, true);
    udelay(1);
    keys |= keypad_get_rows() << i * 4;
    keypad_set_col(i, false);
  }
  keypad_clear_cols();
  return keys;
}

static uint8_t keypad_get_rows(void)
{
  uint16_t gpios = gpio_port_read(KEYPAD_BANK_ROW);
  uint8_t row = 0;
  row |= !!(gpios & KEYPAD_ROW4) << 0;
  row |= !!(gpios & KEYPAD_ROW3) << 1;
  row |= !!(gpios & KEYPAD_ROW2) << 2;
  row |= !!(gpios & KEYPAD_ROW1) << 3;
  return row;
}

static void keypad_set_col(uint8_t col, bool set)
{
  uint16_t gpios = 0;
  switch (col)
  {
    case 0:
      gpios = KEYPAD_COL4;
      break;
    case 1:
      gpios = KEYPAD_COL3;
      break;
    case 2:
      gpios = KEYPAD_COL2;
      break;
    case 3:
      gpios = KEYPAD_COL1;
      break;
  }
  if (set)
  {
    gpio_set(KEYPAD_BANK_COL, gpios);
  }
  else
  {
    gpio_clear(KEYPAD_BANK_COL, gpios);
  }
}

static void keypad_clear_cols(void)
{
  gpio_clear(KEYPAD_BANK_COL, KEYPAD_COL_ALL);
}
