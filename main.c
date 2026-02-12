/*
  Remplacez les ..... par vos noms, prénoms, login Enac, sans enlever les balises <>
  Élève 1 : Prénom <.................>  Nom <.................>  Login Enac <........>
  Élève 2 : Prénom <.................>  Nom <.................>  Login Enac <........>


 */


#include <ch.h>
#include <hal.h>
#include "stdutil.h"
#include "ttyConsole.h"


/*
  ° connecter B6 (uart1_tx) sur terminal rx
  ° connecter B7 (uart1_rx) sur terminal tx
  ° connecter B2 sur le joystick : flèche vers le haut
  ° connecter C0 sur led_0
  ° connecter C1 sur led_8
*/

/*
  Zone de réponses aux questions :

 */





/*
#                  _____                                        _          
#                 / ____|                                      | |         
#                | |  __    ___   _ __     ___   _ __    __ _  | |         
#                | | |_ |  / _ \ | '_ \   / _ \ | '__|  / _` | | |         
#                | |__| | |  __/ | | | | |  __/ | |    | (_| | | |         
#                 \_____|  \___| |_| |_|  \___| |_|     \__,_| |_|         
#                 _____                   _ __                                
#                |  __ \                 | '_ \                               
#                | |__) |  _   _   _ __  | |_) |   ___    ___     ___         
#                |  ___/  | | | | | '__| | .__/   / _ \  / __|   / _ \        
#                | |      | |_| | | |    | |     | (_) | \__ \  |  __/        
#                |_|       \__,_| |_|    |_|      \___/  |___/   \___|        
#                 _______   _                                    
#                |__   __| (_)                                   
#                   | |     _    _ __ ___     ___   _ __         
#                   | |    | |  | '_ ` _ \   / _ \ | '__|        
#                   | |    | |  | | | | | | |  __/ | |           
#                   |_|    |_|  |_| |_| |_|  \___| |_|           
*/

static void gptcb (GPTDriver *gptp);

static const GPTConfig gptcfg = {
				 .frequency    = 10000, // 10 Khz
				 .callback     = &gptcb,
};

static void gptcb (GPTDriver *gptp)
{
  (void) gptp;
  // la première chose à faire est d'interrompre l'ordonnanceur dans
  // un etat stable
  chSysLockFromISR();

  
  // ecrire votre code executé sous interruption du timer (GPT) ici
  
  
  
  // on remet en fonction l'ordonnanceur
  chSysUnlockFromISR();
}


static THD_WORKING_AREA(waHeartbeat, 512);
static void heartbeat (void *) 
{
  chRegSetThreadName("heartbeat");
  
  
  while (true) { 
    palToggleLine(LINE_C00_LED_HEARTBEAT); 	
    chThdSleepMilliseconds(500);
  }
}


/*
#                 ______           _      _          
#                |  ____|         | |    (_)         
#                | |__    __  __  | |_    _          
#                |  __|   \ \/ /  | __|  | |         
#                | |____   >  <   \ |_   | |         
#                |______| /_/\_\   \__|  |_|         
*/
static void palcb(void *) {
  // la première chose à faire est d'interrompre l'ordonnanceur dans
  // un etat stable
  chSysLockFromISR();

  // ecrire votre code executé sous interruption lors d'un appui
  // ou d'un relachement de la broche connectée à JOYSTICK_UP (EXTI) ici
  
  // on remet en fonction l'ordonnanceur
  chSysUnlockFromISR();
}




int main(void)
{

  halInit();
  chSysInit();
  initHeap();

  consoleInit();
  consoleLaunch();

  // lancement du thread qui fait cligoter la led
  chThdCreateStatic(waHeartbeat, sizeof(waHeartbeat), NORMALPRIO, heartbeat, NULL);

  // initialisation et armement du timer en mode continu
  gptStart(&GPTD1, &gptcfg);
  gptStartContinuous(&GPTD1, 50000); // compte jusqu'à 50.000 à 10khz puis recommence
 
  // Activation  PAL callback.
  // Lors d'un changement d'état sur la broche GPIOB2, une interruption est générée et
  // la fonction de callback  palcb sera appelée
  palEnableLineEvent(LINE_B02_JOYSTICK_UP, PAL_EVENT_MODE_BOTH_EDGES);
  palSetLineCallback(LINE_B02_JOYSTICK_UP, &palcb, NULL);
  

  
  // le thread principal affiche juste l'evolution du compteur du timer
  while(true) {
    chprintf(chp, "\rCNT = %lu       ",
             gptGetCounterX(&GPTD1));
    chThdSleepMilliseconds(100);
  }

}
