#include "baro_MPL3115A2.h"
#include "stdutil.h"


#define  OVERSAMPLING (0x0) // no oversampling conv in 6 ms
static const uint8_t mplAdr =  0x60;
static const uint8_t statusReg[] = {0x00};
static const uint8_t pressureReg[] = {0x01};
//static const uint8_t tempReg[] = {0x04};
static const uint8_t oneShotMode[] = {0x26, 0x2 | OVERSAMPLING};
static const uint8_t enableEvent[] = {0x13, 0x07};

static msg_t i2cInitBaro(I2CDriver *i2cd)
{
  msg_t status;
  
  
  status = i2cMasterTransmitTimeout(i2cd, mplAdr, enableEvent, sizeof(enableEvent),	
				      NULL, 0, TIME_MS2I(100)) ;	
  if (status == MSG_OK) {
    status = i2cMasterTransmitTimeout(i2cd, mplAdr, oneShotMode, sizeof(oneShotMode),
			   NULL, 0, TIME_MS2I(100)) ;
  }
  
  return status;
}


float i2cBaroGetPressure(I2CDriver *i2cd)
{
  uint8_t rxbuf[4];
  bool  notReady;
  msg_t status;
  uint32_t  rawB;
  static bool initDone = false;
  

  if (!initDone) {
    i2cInitBaro(i2cd);
    initDone = true;
  }
  
  
  status = i2cMasterTransmitTimeout(i2cd, mplAdr, oneShotMode, sizeof(oneShotMode),	
				    NULL, 0, TIME_MS2I(100)) ;
  if (status !=  MSG_OK) {
    DebugTrace("baroPrintPressure I²C error");
  }
  
  do {
    status = i2cMasterTransmitTimeout(i2cd, mplAdr,
				      statusReg, sizeof(statusReg),	
			     rxbuf, 2, TIME_MS2I(100)) ;
    notReady = !(rxbuf[0] & 1<<2);
    if (notReady || (status != MSG_OK)) {
      chThdSleepMilliseconds(2);
    }
  } while (notReady);
  
  status = i2cMasterTransmitTimeout(i2cd, mplAdr, pressureReg,
				    sizeof(pressureReg),	
				    (uint8_t *)&rawB, 3, TIME_MS2I(100)) ;
  
  if (status == MSG_OK) {
    const uint32_t swapVal = (SWAP_ENDIAN32(rawB<<8)) ;
    const float pressure = swapVal / 6400.0f;
    return pressure;
  } else {
    return -1000;
  }
}
