#include "w25q.h"
#include "w25q_opcodes.h"

#include "delay.h"
#include "os.h"

#define WRITE(data) (uint32_t) spi_xfer(W25Q_SPI, (uint8_t)(data))
#define READ(offset) (uint32_t) spi_xfer(W25Q_SPI, 0x00) << (8 * offset)

static void write_seq(const uint8_t *data, size_t size) __attribute__((unused));
static void select(void);
static void deselect(void);

uint16_t w25q_read_id(void)
{
  select();

  WRITE(OP_READ_MANUFACTURER_DEVICE_ID);
  WRITE(0);
  WRITE(0);
  WRITE(0);
  uint32_t id = 0;
  id |= READ(3);
  id |= READ(2);
  id |= READ(1);
  id |= READ(0);

  deselect();
  return id;
}

uint32_t w25q_read_jedec_id(void)
{
  select();

  WRITE(OP_READ_JEDEC_ID);
  uint32_t jedec_id = 0;
  jedec_id |= READ(3);
  jedec_id |= READ(2);
  jedec_id |= READ(1);
  jedec_id |= READ(0);

  deselect();
  return jedec_id;
}

void w25q_read(uint8_t *buf, uint32_t addr, size_t size)
{
  select();

  WRITE(OP_READ_DATA);
  WRITE(addr << 16);
  WRITE(addr << 8);
  WRITE(addr);
  while (size--)
  {
    *buf++ = READ(0);
  }

  deselect();
}

void w25q_reset(void)
{
  select();

  WRITE(OP_ENABLE_RESET_AND_RESET_DEVICE(false));
  WRITE(OP_ENABLE_RESET_AND_RESET_DEVICE(true));

  deselect();
  os_sleep(100);
}

void w25q_sleep(void)
{
  select();

  WRITE(OP_POWER_DOWN);

  deselect();
}

void w25q_wakeup(void)
{
  select();

  WRITE(OP_RELEASE_POWER_DOWN_DEVICE_ID);

  deselect();
}

static void write_seq(const uint8_t *data, size_t size)
{
  while (size--)
  {
    WRITE(*data++);
  }
}

static void select(void) { gpio_clear(W25Q_BANK_CS, W25Q_CS); }

static void deselect(void) { gpio_set(W25Q_BANK_CS, W25Q_CS); }
