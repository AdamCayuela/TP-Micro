#include <ch.h>
#include <hal.h>
#include "stdutil.h"
#include "radio.h"
#include "frskyFport_fsm.h"



static uint8_t rssi = 0;

static void sendTelemetryAfter300Us(const void *buffer, size_t len,
                                    void *optArg);

static RadioTelemetryCb_t radioTelemetryCb = NULL;

static FportFsmContext fportFsm;
static volatile FportErrorMask errorMsk = FPORT_OK;


static void sendTelemetryAfter300Us(const void *buffer, size_t len,
                                    void *)
{
  if (radioTelemetryCb)
    radioTelemetryCb(buffer, len);
}

const FportFsmContextConfig fportCfg = {
  .tlmSendCb = sendTelemetryAfter300Us,
  .ctrlReceiveCb = NULL,
  .optArg = nullptr,
  .middlePoint = 992,
};

void radioInit(RadioTelemetryCb_t sendTlmAfter300msCb)
{
  radioTelemetryCb = sendTlmAfter300msCb;
  fportContextInit(&fportFsm, &fportCfg);
}

float radioGetControl(ControllerAxis axis)
{
  return (errorMsk == FPORT_OK) ?
    clampTo(-1.0f, 1.0f, fportGetChannel(&fportFsm, axis) / 800.0f) :
    0.0f;
}


uint8_t radioGetRssi(void)
{
  return rssi;
}

void    radioSetTelemetryParam(fportAppId id, uint32_t value)
{
  fportTelemetryUpData(&fportFsm, id,
		       (FportTelemetryValue) {.u32 = value});
}

const char* radioErrorMessageString(FportErrorMask errMsk)
{
  if (!errMsk)
    return("Status OK");
  if (errMsk & FPORT_MALFORMED_FRAME)
    return("Frame Err");
  if (errMsk &  FPORT_CRC_ERROR)
    return("Crc Err");
  if (errMsk & FPORT_APPID_LEN_EXCEDEED)
    return("AppId Array Len Err");
  if (errMsk & FPORT_TIMEOUT)
    return("Timeout");
  if (errMsk & FPORT_RADIO_LINK_LOST)
    return("Link Lost");
  if (errMsk & FPORT_FAILSAFE)
    return("Failsafe");
  if (errMsk & FPORT_READBACK_CTRL_FAIL)
    return("Readback Err");
  return ("Unknown error");
}

FportErrorMask radioFeedByte(uint8_t byte)
{
  return fportFeedFsm(&fportFsm, &byte, sizeof(byte));
}

FportErrorMask radioGetStatus(void)
{
  return errorMsk;
}
