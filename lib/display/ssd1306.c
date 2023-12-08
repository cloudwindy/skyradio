#include "ssd1306.h"
#include "delay.h"
#include "led.h"
#include "ssd1306_conf.h"
#include "ssd1306_opcodes.h"

#include <math.h>
#include <stdlib.h>
#include <string.h> // For memcpy

static void reset(void);
static void write_command(uint8_t opcode);
static void write_data(uint8_t *data, size_t size);
static void select(void);
static void deselect(void);

static void gpio_set_s(uint32_t gpioport, uint16_t gpios);
static void gpio_clear_s(uint32_t gpioport, uint16_t gpios);

const uint8_t init_sequence_opcodes[] = {
  OP_SET_DISPLAY_ON_OFF(0),
  OP_SET_MEMORY_ADDRESSING_MODE,
  OP_SET_MEMORY_ADDRESSING_MODE_PAGE,
  OP_SET_PAGE_START_ADDRESS(0),
#ifdef SSD1306_MIRROR_VERT
  OP_SET_COM_OUTPUT_SCAN_DIRECTION(0),
#else
  OP_SET_COM_OUTPUT_SCAN_DIRECTION(8),
#endif
  OP_SET_LOWER_START_ADDRESS(0),
  OP_SET_HIGHER_START_ADDRESS(0),
  OP_SET_DISPLAY_START_LINE(0),
  OP_SET_CONTRAST_CONTROL,
  0xFF,
#ifdef SSD1306_MIRROR_HORIZ
  OP_SET_SEGMENT_REMAP(0),
#else
  OP_SET_SEGMENT_REMAP(1),
#endif
#ifdef SSD1306_INVERSE_COLOR
  OP_SET_NORMAL_INVERSE_DISPLAY(1),
#else
  OP_SET_NORMAL_INVERSE_DISPLAY(0),
#endif

// Set multiplex ratio.
#if (SSD1306_HEIGHT == 32)
  OP_SET_MULTIPLEX_RATIO,
  0x1F,
#elif (SSD1306_HEIGHT == 64)
  OP_SET_MULTIPLEX_RATIO,
  0x3F,
#elif (SSD1306_HEIGHT == 128)
  // Found in the Luma Python lib for SH1106.
  0xFF,
  0x3F, // Seems to work for 128px high displays too.
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif
  OP_SET_ENTIRE_DISPLAY_ON(0),
  OP_SET_DISPLAY_OFFSET,
  0x00,
  OP_SET_DISPLAY_CLOCK_DIVIDE_RATIO_OSCILLATOR_FREQUENCY,
  0xF0,
  OP_SET_PRECHARGE_PERIOD,
  0x22,
  OP_SET_COM_PINS_HARDWARE_CONFIGURATION,
#if (SSD1306_HEIGHT == 32)
  0x02,
#elif (SSD1306_HEIGHT == 64)
  0x12,
#elif (SSD1306_HEIGHT == 128)
  0x12,
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif
  OP_SET_VCOMH_DESELECT_LEVEL,
  0x20, // 0x20,0.77xVcc
  OP_SET_DC_DC_ENABLE,
  0x14,
  OP_SET_DISPLAY_ON_OFF(1),
};

/* Initialize the oled screen */
void setup_ssd1306(void)
{
  deselect();
  // Reset OLED
  reset();

  // Wait for the screen to boot
  mdelay(100);

  // Init OLED
  select();
  const uint8_t *cmd_p = init_sequence_opcodes;
  for (size_t i = 0; i < sizeof(init_sequence_opcodes); i++)
  {
    write_command(*cmd_p++);
  }
  deselect();
}

/* Write the screenbuffer with changed to the screen */
void ssd1306_update(uint8_t *buf)
{
  // Write data to each page of RAM. Number of pages
  // depends on the screen height:
  //
  //  * 32px   ==  4 pages
  //  * 64px   ==  8 pages
  //  * 128px  ==  16 pages
  select();
  for (uint8_t i = 0; i < SSD1306_HEIGHT / 8; i++)
  {
    // Set the current RAM page address.
    write_command(OP_SET_PAGE_START_ADDRESS(i));
    write_command(OP_SET_LOWER_START_ADDRESS(SSD1306_X_OFFSET_LOWER));
    write_command(OP_SET_HIGHER_START_ADDRESS(SSD1306_X_OFFSET_UPPER));
    write_data(&buf[SSD1306_WIDTH * i], SSD1306_WIDTH);
  }
  deselect();
}

static void reset(void)
{
  // Reset the OLED
  gpio_clear(SSD1306_BANK_RES, SSD1306_RES);
  mdelay(1);
  gpio_set(SSD1306_BANK_RES, SSD1306_RES);
}

// Send a byte to the command register.
static void write_command(uint8_t opcode)
{
  // Switch to command mode.
  gpio_clear_s(SSD1306_BANK_DC, SSD1306_DC);
  spi_send(SSD1306_SPI, (uint16_t)opcode);
  udelay(1);
}

// Send data.
static void write_data(uint8_t *data, size_t size)
{
  // Switch to data mode.
  gpio_set_s(SSD1306_BANK_DC, SSD1306_DC);
  // This single line of code has caused a lot of frustration.
  while (size--)
  {
    spi_send(SSD1306_SPI, (uint16_t)*data++);
    udelay(1);
  }
}

static void select(void) { gpio_clear_s(SSD1306_BANK_CS, SSD1306_CS); }

static void deselect(void) { gpio_set_s(SSD1306_BANK_CS, SSD1306_CS); }

/**
 * gpio_set with delay so they don't cause problems.
 */
static void gpio_set_s(uint32_t gpioport, uint16_t gpios)
{
  udelay(1);
  gpio_set(gpioport, gpios);
  udelay(1);
}

/**
 * gpio_clear with delay so they don't cause problems.
 */
static void gpio_clear_s(uint32_t gpioport, uint16_t gpios)
{
  udelay(1);
  gpio_clear(gpioport, gpios);
  udelay(1);
}