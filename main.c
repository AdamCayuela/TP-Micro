/*
  Remplacez les ..... par vos noms, prénoms, login Enac, sans enlever les balises <>
  Élève 1 : Prénom <.................>  Nom <.................>  Login Enac <........>
  Élève 2 : Prénom <.................>  Nom <.................>  Login Enac <........>
*/


#include <ch.h>
#include <hal.h>
#include <math.h>
#include "stdutil.h"
#include "ttyConsole.h"
#include "potentiometre.h"

#define POT_BOUTON 0 // potentiometre à bouton : se tourne à la main
#define POT_VIS    1 // potentiometre à empreinte : se tourne avec un tournevis

/*

 ° connecter B6 (uart1_tx) sur terminal rx
  ° connecter B7 (uart1_rx) sur terminal tx
  ° connecter C0 sur LED0 
  ° connecter A15 (sortie PWM TIM2_CH1) sur LED8 
  ° connecter C1 (ADC1_IN11) sur le potentiomètre à bouton
  ° connecter C2 (ADC1_IN12) sur le potentiomètre à empreinte cruciforme
  ° connecter C13 au reset de l'ecran OLED
  ° verifier que les jumpers C6-Rx et C7-Tx de l'ecran fds soient connectés
  ° laisser le jumper entre +3.3V et TOPs pour ces potentiomètres
 */

/*
  Zone de réponses aux questions

 */

#define PWM_FREQ         1
#define TICKS_PER_PERIOD 10000 // we can also use 1e4 notation

// ne pas modifer la macro ci dessous, ne jouer que sur les deux macros ci dessus
#define TICK_FREQ (PWM_FREQ * TICKS_PER_PERIOD)

// pour les parties 1 (persistance) et 2 (servo) et 3 (il variation continue du pwm)
// n'y a pas besoin de modifier  la structure de configuration.
// Il n'y a que dans la partie Utilisation des 4 canaux
// que le champ .channels de cette structure devra être modifié
static PWMConfig pwmcfg = {	// pwm d'une frequence d'un hz et 10000 pas de quantification
  .frequency = TICK_FREQ,        // TickFreq : PwmFreq(1) * ticksPerPeriod(10000)  
  .period    = TICKS_PER_PERIOD, //   tickPerPeriod (10000)
  .callback  = NULL,		 //   pas de callback de passage à l'etat actif
  .channels  = {
    // sortie active, polarité normale, pas de callback
    {.mode = PWM_OUTPUT_ACTIVE_HIGH, .callback = NULL},
    // sortie inactive
    {.mode = PWM_OUTPUT_DISABLED, .callback = NULL},
    // sortie inactive
    {.mode = PWM_OUTPUT_DISABLED, .callback = NULL},
    // sortie inactive
    {.mode = PWM_OUTPUT_DISABLED, .callback = NULL}
  },
  .cr2  = 0, // doit être initialisé à 0 (voir stm32f4 reference manuel)
  .dier = 0  // doit être initialisé à 0 (voir stm32f4 reference manuel)
};





static THD_WORKING_AREA(waBlinker, 256);
static void blinker (void *);

static THD_WORKING_AREA(waPwmCommand, 512);
static void pwmCommand(void *) 
{
  chRegSetThreadName("pwmCommand");

  while (true) {

    // renvoie un float entre 0.0f quand le potentiometre à bouton est tourné à gauche
    //                     et 1.0f quand il est tourné à droite
    const float potentiometerVal =  getPotValue(POT_BOUTON); 

    // pour la led, on veut balayer toute la plage de 0 à tickPerPeriod
    // attention, pour le servomoteur ce ne sera plus le cas !
    const pwmcnt_t newDuty = potentiometerVal * TICKS_PER_PERIOD; 

    pwmEnableChannel(&PWMD2, 0, newDuty); // entre 0 et tickPerPeriod
    
    // la frequence  maximum pour changer la valeur du pwm est la valeur du pwm :
    // si le pwm est à 50hz, on ne doit pas le changer plus de 50 fois par seconde
    chThdSleepMilliseconds(20); 
  }
  
}
 


int main(void)
{

  halInit();
  chSysInit();
  initHeap();

  consoleInit();

  pwmStart(&PWMD2, &pwmcfg);
  initPotentiometre();

  chThdCreateStatic(waBlinker, sizeof(waBlinker), NORMALPRIO, blinker, NULL);
  chThdCreateStatic(waPwmCommand, sizeof(waPwmCommand), NORMALPRIO, pwmCommand, NULL);

  // cette fonction ne rend pas la main, aucune instruction située après ne sera exécutée
  consoleLaunch();  
  
  chThdSleep(TIME_INFINITE); 
}


static void blinker (void *)
{
  chRegSetThreadName("blinker");
  while (true) { 
    palToggleLine(LINE_C00_LED0); 	
    chThdSleepMilliseconds(500);
  }
}
