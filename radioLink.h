#include "radio.h"
#pragma once

/**
  * @brief      initialize radio controler link module on SD2
  *
  * @notes	initialize serial driver SD2
  *             implement sendTelemetry callback
  *             call radioInit with sendTelemetry callback function address
  *             launch receiveFrame thread
**/
void radioLinkStart(void);
