#include <ch.h>
#include <hal.h>
#include "radioLink.h"
#include "radio.h"

static THD_WORKING_AREA(waReceiveFrame, 1024);
static void receiveFrame (void *arg);
static void sendTelemetryAfter300Us(const void *buffer, size_t len);

void radioLinkStart(void)
{
  static const SerialConfig fportSerialConfig = {
    .speed = 115200,
    .cr1 = 0, // 8 bits, no parity, one stop bit
    .cr2 = USART_CR2_STOP1_BITS, 
    .cr3 = USART_CR3_HDSEL // half duplex
  };

  sdStart(&SD2, &fportSerialConfig);
  radioInit(&sendTelemetryAfter300Us);
  chThdCreateStatic(waReceiveFrame, sizeof(waReceiveFrame), NORMALPRIO, receiveFrame, NULL);
}

static void receiveFrame (void *)
{
  chRegSetThreadName("receiveFrame");

  while (true) {
    uint8_t byte;
    sdRead(&SD2, &byte, sizeof(byte));
    radioFeedByte(byte);
  }
}

static void sendTelemetryAfter300Us(const void *buffer, size_t len)
{
  uint8_t loopbackBuffer[24];

  chThdSleepMicroseconds(300);
  sdWrite(&SD2, buffer, len);

  // on est en half duplex, du coup tout ce qui est émis est aussi reçu
  // il faut donc lire ce qu'on a reçu pour vider le buffer.
  // si on ne le fait pas, ça utilise du CPU dans la machine
  // à états de reception pour rien, mais ça ne plante pas.
  sdRead(&SD2, loopbackBuffer, len);
}
