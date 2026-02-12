/*
  ° Remplacez les ..... par vos noms, prénoms, login Enac, sans enlever les balises <>
  Élève 1 : Prénom <>  Nom <>  Login Enac <>
  Élève 2 : Prénom <>  Nom <>  Login Enac <>
 */

#include <ch.h>		// fichier d'entête de l'ordonnanceur (scheduler) de ChibiOS 
#include <hal.h>	// fichier d'entête de la HAL (hardware abstraction layer)  de ChibiOS 
  

void rotateRight(){
  msg_t led4 = palReadLine(LINE_C04_LED4);
  palWriteLine(LINE_C04_LED4,palReadLine(LINE_C03_LED3));
  palWriteLine(LINE_C03_LED3,palReadLine(LINE_C02_LED2));
  palWriteLine(LINE_C02_LED2,palReadLine(LINE_C01_LED1));
  palWriteLine(LINE_C01_LED1,led4);
}

void rotateLeft(){
  msg_t led1 = palReadLine(LINE_C01_LED1);
  palWriteLine(LINE_C01_LED1,palReadLine(LINE_C02_LED2));
  palWriteLine(LINE_C02_LED2,palReadLine(LINE_C03_LED3));
  palWriteLine(LINE_C03_LED3,palReadLine(LINE_C04_LED4));
  palWriteLine(LINE_C04_LED4,led1);
}

void toggleLeds(int speed){
  msg_t led1,led2,led3,led4;
  led1=palReadLine(LINE_C01_LED1);
  led2=palReadLine(LINE_C02_LED2);
  led3=palReadLine(LINE_C03_LED3);
  led4=palReadLine(LINE_C04_LED4);
     if(led1==PAL_HIGH){
    palClearLine(LINE_C01_LED1);
  }
  if(led2==PAL_HIGH){
    palClearLine(LINE_C02_LED2);
  }
  if(led3==PAL_HIGH){
    palClearLine(LINE_C03_LED3);
  }
  if(led4==PAL_HIGH){
    palClearLine(LINE_C04_LED4);
  }
  chThdSleepMilliseconds(speed);
if(led1==PAL_HIGH){
    palSetLine(LINE_C01_LED1);
  }
  if(led2==PAL_HIGH){
    palSetLine(LINE_C02_LED2);
  }
  if(led3==PAL_HIGH){
    palSetLine(LINE_C03_LED3);
  }
  if(led4==PAL_HIGH){
    palSetLine(LINE_C04_LED4);
  }
  chThdSleepMilliseconds(speed);
}


/*
 * le programme va commencer ici à la mise sous tension du microcontroleur
 */
int  main (void)
{
  //  initialisation de la HAL : les pilotes de periphérique sont initialisés
  halInit();
  
  // initialisation de l'ordonnanceur du noyau RTOS : le main devient un thread
  chSysInit();
  int speed=200;

  // boucle infinie : sur un microcontroleur, la notion de sortie de la fonction
  //                  main n'a pas de sens.
  // LINE_C00_LED0 fait reference à la definition de la broche dans le fichier board.cfg
  while (true) {
    
    if(palReadLine(LINE_C05_SWITCH0)==PAL_HIGH)speed = 500;
    if(palReadLine(LINE_B01_BPR)==PAL_LOW){
      rotateRight();
      chThdSleepMilliseconds(500);
    }
    if(palReadLine(LINE_B00_BPL)==PAL_LOW){
      rotateLeft();
      chThdSleepMilliseconds(500);
    }
    else{
      toggleLeds(speed);
    }

    palSetLine(LINE_C00_LED0);   // niveau logique HAUT
    chThdSleepMilliseconds(speed);  // attente de 500 millisecondes
    palClearLine(LINE_C00_LED0); // niveau logique BAS
    chThdSleepMilliseconds(speed); // attente de 500 millisecondes

    speed=200;

    // on aurait aussi pu utiliser palToggleLine :
    //    palToggleLine(LINE_C00_LED0);
    //    chThdSleepMilliseconds(500);
  }
}
