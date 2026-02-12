/*
  ° Remplacez les ..... par vos noms, prénoms, login Enac, sans enlever les balises <>
  Élève 1 : Prénom <>  Nom <>  Login Enac <>
  Élève 2 : Prénom <>  Nom <>  Login Enac <>
 */

#include <ch.h>		// fichier d'entête de l'ordonnanceur (scheduler) de ChibiOS 
#include <hal.h>	// fichier d'entête de la HAL (hardware abstraction layer)  de ChibiOS 



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
    
    if(palReadLine(LINE_C01_SWITCH0)==PAL_HIGH)speed = 500;

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
