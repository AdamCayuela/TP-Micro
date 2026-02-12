/*
  Remplacez les ..... par vos noms, prénoms, login Enac, sans enlever les balises <>
  Élève 1 : Prénom <>  Nom <>  Login Enac <>
 */

#include <ch.h>
#include <hal.h>
#include "stdutil.h"
#include "adcHelper.h"
#include "ttyConsole.h"


/*
  ° connecter B6 (uart1_tx) sur terminal rx
  ° connecter B7 (uart1_rx) sur terminal tx
  ° connecter C0 sur led0
  ° connecter C1 sur le potentiomètre à bouton 
*/

/*
  Zone de réponses aux questions

 */

#define ADC_GRP1_NUM_CHANNELS   4
#define ADC_GRP1_BUF_DEPTH      100


// buffer qui contient les échantillons une fois la convertion effectuée
static adcsample_t samples[ADC_GRP1_NUM_CHANNELS * ADC_GRP1_BUF_DEPTH];

float convertisseur(int valeur){
  return((valeur*3.3)/4095);
}

int main(void)
{

    /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */

  halInit();
  chSysInit();
  initHeap();

  consoleInit();

  // fonction helper qui cree la structure de configuration 
  const ADCConversionGroup adcgrpcfg = adcGetConfig(ADC_GRP1_NUM_CHANNELS,
						    ADC_CHANNEL_IN11,
                ADC_CHANNEL_IN12,
                ADC_CHANNEL_VBAT,
                ADC_CHANNEL_SENSOR,
						    ADC_END);
  
  // demarre le périphérique ADC
  adcStart(&ADCD1, NULL);
  adcSTM32EnableVBATE();
  adcSTM32EnableTSVREFE();

  consoleLaunch();   

  while (true) {
    adcConvert(&ADCD1, &adcgrpcfg, samples, ADC_GRP1_BUF_DEPTH);
    
    uint32_t totpot=0;
    uint32_t tottourpot=0;
    uint32_t totvbat=0;
    uint32_t totTemp=0;
    for(int i=0;i<100;i++){
      totpot+=samples[3+i*4];
      tottourpot+=samples[3+i*4];
      totvbat+=samples[3+i*4];
      totTemp+=samples[3+i*4];
    }

    totpot=totpot/100;
    tottourpot=tottourpot/100;
    totvbat=totvbat/100;
    totTemp=totTemp/100;
    
    const adcsample_t potSample = samples[0];
    const adcsample_t tourPotSample = samples[1];
    const adcsample_t VBAT = samples[2]*2;
    const adcsample_t Temp = 25+(((totTemp*3.3)/4095)-0.76)*400;


    DebugTrace("echantillon adc potentiometre (1) = %u", potSample);
    DebugTrace("conversion en volts = %.3f", convertisseur(potSample));
    DebugTrace("echantillon adc potentiometre (2) = %u", tourPotSample);
    DebugTrace("conversion en volts = %.3f", convertisseur(tourPotSample));
    DebugTrace("tension pile = %.2f", convertisseur(VBAT));
    DebugTrace("tension pile = %.2d°C", Temp);

    
    palToggleLine(LINE_C00_LED0); 	
    chThdSleepSeconds(1);
  }
}

