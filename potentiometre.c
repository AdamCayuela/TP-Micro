#include <ch.h>
#include <hal.h>
#include <stdnoreturn.h>
#include "stdutil.h"
#include "potentiometre.h"
#include "display4DS.h"
#include "math.h"


static THD_WORKING_AREA(waFds, 1536);
static noreturn void fds(void *arg);


#define ADC_GRP1_NUM_CHANNELS   2
#define ADC_GRP1_BUF_DEPTH      128
static adcsample_t samples1[ADC_GRP1_BUF_DEPTH] [ADC_GRP1_NUM_CHANNELS];

/*
 * ADC conversion group.
 * Mode:        Linear buffer, 1 samples of 2 channels, SW triggered.
 * Channels:    IN11
 */
static const ADCConversionGroup adcgrpcfg1 = {
  .circular	= TRUE,
  .num_channels = ADC_GRP1_NUM_CHANNELS,
  .end_cb	= NULL, // adc complete callback
  .error_cb	= NULL, // adc error callback
  .cr1		= 0,                        /* CR1 */
  .cr2		= ADC_CR2_SWSTART,          /* CR2 */
  .smpr1	= ADC_SMPR1_SMP_AN11(ADC_SAMPLE_480) | ADC_SMPR1_SMP_AN12(ADC_SAMPLE_480),
  .smpr2	= 0,                        /* SMPR2 */
  .sqr1		= ADC_SQR1_NUM_CH(ADC_GRP1_NUM_CHANNELS),  /* SQR1 */
  .sqr2		= 0,					   /* SQR2 */
  .sqr3		= ADC_SQR3_SQ1_N(ADC_CHANNEL_IN11) | ADC_SQR3_SQ2_N(ADC_CHANNEL_IN12) 	   /* SQR3 */
};



void initPotentiometre(void)
{
  adcStart(&ADCD1, NULL);
  adcStartConversion (&ADCD1, &adcgrpcfg1, (adcsample_t *) samples1, ADC_GRP1_BUF_DEPTH);
  chThdCreateStatic(waFds, sizeof(waFds), NORMALPRIO, fds, NULL);
}


float getPotValue (const uint32_t index)
{
  if (index >= ADC_GRP1_NUM_CHANNELS)
    return -9.99f;
  static float filteredPot[ADC_GRP1_NUM_CHANNELS] = {0.0f}; 
  static float lastRes = 0;
  static uint32_t lastIndex = 0;
  static systime_t ts = 0;
  uint32_t sum=0;

  if ((lastIndex == index) && ((chVTGetSystemTimeX() - ts) < TIME_MS2I(20)))
    return lastRes;


  ts = chVTGetSystemTimeX();
  lastIndex = index;
  for (size_t i=0; i < ADC_GRP1_BUF_DEPTH; i++) { 
    sum += samples1[i][index];
  }
  filteredPot[index] = ((filteredPot[index]) + sum) / 2.0f;

  const float raw = (filteredPot[index] /ADC_GRP1_BUF_DEPTH) / 4090.0f;
  /* if (raw < 0.01) { */
  /*   return (raw * raw * 100); */
  /* } else { */
  /*   return raw; */
  /* } */
  const float expo = powf (raw, expf(1.0f));
  return (lastRes = MIN (expo, 1.0f));
}


static noreturn void fds(void *arg)
{

  (void)arg;
  chRegSetThreadName("fds");
  FdsDriver fds;

  
  fdsStart(&fds, &SD6, 115200, LINE_C13_RESET_OLED, FDS_GOLDELOX);
  fdsSetTextSizeMultiplier(&fds, 2, 2);
  while (true) {
    // On recupère la position du potentiomètre connecté à l'ADC
    // renvoie un float entre 0.0f et 1.0f

    // on affiche la valeur sur l'écran OLED
    fdsGotoXY(&fds, 0,0);
    fdsPrintFmt(&fds, "B=%3.2f%%",getPotValue(0) *100.0f);
    fdsGotoXY(&fds, 0,2);
    fdsPrintFmt(&fds, "V=%3.2f%%",getPotValue(1) *100.0f);
    //    DebugTrace ("pot1 = %02.1f %%    ",getPotValue(0) *100.0f);
    chThdSleepMilliseconds(100);
  }
}
