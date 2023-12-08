#include "tasks.h"

#include "led.h"
#include "mem.h"
#include "os.h"
#include "printf.h"
#include "remote.h"
#include "serial.h"

#include <stdbool.h>
#include <string.h>

static void strip_crlf(char *str, size_t *len_p);

void task_remote(void *args __attribute__((unused)))
{
  /* Allocate receive buffer. */
  char *recvbuf = memalloc(2);
  size_t recvbuf_size = 2;
  /* Indicate how much buffer is used. */
  size_t recvcur = 0;
  /* Indicate how much data is incoming. */
  size_t recvlen = 0;

  while (true)
  {
    /* Main loop, check for incoming data. */
    recvlen = serial_recvlen();
    if (recvlen > 0)
    {
      /* Check if our buffer is sufficient for the incoming data. */
      if (recvbuf_size < recvcur + recvlen)
      {
        /* Re-allocate the old buffer with more memory. */
        char *recvbuf_old = recvbuf;
        /* TODO: we should implement realloc(). */
        recvbuf = memalloc(recvbuf_size * 2);
        memcpy(recvbuf, recvbuf_old, recvbuf_size);
        memfree(recvbuf_old);
        recvbuf_size *= 2;
      }

      /**
       * serial_recv:
       * 1. Skip the used parts of the buffer. (+ recvcur)
       * 2. Prevent overflowing. (- recvcur)
       * 3. Write to recvbuf.
       */
      serial_recv(recvbuf + recvcur, recvbuf_size - recvcur);
      /* Indicate more buffer is used. */
      recvcur += recvlen;
      /* Terminate the string. */
      recvbuf[recvcur] = '\0';
      /* The start of data block. */
      char *start_p = recvbuf;
      /* The end of data block. */
      char *end_p = strchr(start_p, REMOTE_DATA_DELIMITER);

      /* Search for data blocks as long as it's not ended. */
      while (end_p != NULL)
      {
        /* Data end. */
        size_t block_size = end_p - start_p + 1;
        /* Remove crlf and again terminate the string. */
        strip_crlf(start_p, &block_size);

        /* Execute command in the data block. */
        char *resp = memalloc(REMOTE_MAX_RESPONSE_SIZE);
        int ret = remote_exec(resp, start_p, block_size);
        size_t resp_len = strlen(resp);

        if (ret < 0)
        {
          /* Failed. */
          printf("remote_exec: %d\n", ret);
        }

        if (resp_len > 0)
        {
          /* We have something to reply. */
          ret = serial_send(resp, resp_len);
          if (ret < 0)
          {
            /* ...but we couldn't. */
            printf("serial_send: %d\n", ret);
          }
        }

        memfree(resp);
        /* Move to the start of next data block. */
        start_p = end_p + 1;
        if (start_p[0] != '\0')
        {
          /* Find the end of next data block, if there is. */
          end_p = strchr(start_p, REMOTE_DATA_DELIMITER);
        }
        else
        {
          break;
        }
      }
      if (start_p != recvbuf)
      {
        /* We just had at least one block. */
        led_blink();
        if (start_p[0] != '\0')
        {
          /* ...but the last block didn't end properly. */
          size_t block_size = strlen(start_p);
          /* Move it to the start of buffer. */
          memmove(recvbuf, start_p, block_size);
          recvcur = block_size;
        }
        else
        {
          recvcur = 0;
        }
      }
    }
    os_sleep(REMOTE_TICK);
  }
}

static void strip_crlf(char *str, size_t *len_p)
{
  int len = *len_p;
  if (str[len - 2] == '\r')
  {
    str[len - 2] = '\0';
    *len_p -= 2;
  }
  else if (str[len - 1] == '\n')
  {
    str[len - 1] = '\0';
    *len_p -= 1;
  }
  else
  {
    str[len] = '\0';
  }
}
