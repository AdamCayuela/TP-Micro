/*
  Remplacez les ..... par vos noms, prénoms, login Enac, sans enlever les balises <>
  Élève 1 : Prénom <>  Nom <>  Login Enac <>
  Élève 2 : Prénom <>  Nom <>  Login Enac <>
*/

/*
  QUESTION 1 : N° d'esclave du MPL3115A2   => 0x60
  QUESTION 2 : adresse du registre Chip_ID => 0x0C
  QUESTION 3 : valeur de Chip_ID           => 0xC4
  QUESTION 4 : pression affichée	   => 980 hPa
  QUESTION 5 : adresse du registre de temperature 
                         (partie entière)  => 0x04
  QUESTION 6 : adresse du registre de temperature 
                    (partie fractionnaire) => 0x05
  QUESTION 7 : température lue		   => 23°C
  */

#include <ch.h>
#include <hal.h>
#include "stdutil.h"	//   permet d'utiliser la fonction DebugTrace qui affiche dans le terminal
#include "ttyConsole.h" //   le shell
#include "baro_MPL3115A2.h" // fonction d'initialisation du baromètre
#include "radioLink.h"  // module de gestion de la liaison avec la télécommande

/*
  ° connecter B6 (uart1_tx) sur le rx du convertisseur USB-série AVEC UN JUMPER
  ° connecter B7 (uart1_rx) sur le tx du convertisseur USB-série AVEC UN JUMPER
  ° connecter C0 sur led0
  ° connecter SCL sur un bus inutilisé, 
      repiquer vers 
         1) une résistance de pullup inutilisée, 
         2) SCL du baro
  ° connecter SDA sur un bus inutilisé, 
       repiquer vers
       1) une résistance de pullup inutilisée, 
       2) SDA du baro
*/

static volatile uint32_t blinkPeriod = 1000;
const uint8_t baroNumSlave = 0x60;
const uint8_t imuNumSlave = 0x68;

// Cette tâche a une fonction de heartbeat : l'arret du clignotement indique
// que le systeme s'est arreté de fonctionner
static THD_WORKING_AREA(waBlinker, 304);	// declaration de la pile du thread blinker
static void  blinker (void *)	        // fonction d'entrée du thread blinker
{
  chRegSetThreadName("blinker");		// on nomme le thread
  
  while (true) {				// boucle infinie
    palToggleLine(LINE_C00_LED1);		// clignotement de la led 
    chThdSleepMilliseconds(blinkPeriod);	// 1hz nominal, 10hz si on sort de main
  }
}

void i2cBaroPrintDevId(void){
  const uint8_t txBuf[]={0x0C};
  uint8_t rxBuf[1];
  msg_t status = i2cMasterTransmitTimeout(
    &I2CD1,
    baroNumSlave,
    txBuf, sizeof(txBuf),
    rxBuf,sizeof(rxBuf),
    TIME_MS2I(100));

    if(status==MSG_OK){
      if(rxBuf[0]==0xC4){
        DebugTrace("Chip ID = 0x%x",rxBuf[0]);
      }
      else DebugTrace("Error value\n");
    }
    else DebugTrace("Error config\n");
}

void calculTemp(void){
  const uint8_t txBuf[]={0x04};
  uint8_t rxBuf[2];
  msg_t status = i2cMasterTransmitTimeout(
    &I2CD1,
    baroNumSlave,
    txBuf, sizeof(txBuf),
    rxBuf,sizeof(rxBuf),
    TIME_MS2I(100));
    if(status==MSG_OK){
      DebugTrace("Temp = %f",(float)rxBuf[0] + rxBuf[1]/256.0);
    }
    else DebugTrace("Error config\n");
}

void accelZ(void){
  const int8_t txBuf[]={0x3B};
  int8_t rxBuf[6];

  msg_t status = i2cMasterTransmitTimeout(
    &I2CD1,
    imuNumSlave,
    txBuf, sizeof(txBuf),
    rxBuf,sizeof(rxBuf),
    TIME_MS2I(100));

    if(status==MSG_OK){
      DebugTrace("Acceleration (brute) en Z : %lf",rxBuf[4]*pow(2,9)+rxBuf[5]);
      DebugTrace("Acceleration (G) en Z : %lf",(rxBuf[4]*pow(2,9)+rxBuf[5])/pow(2,15));
      DebugTrace("Acceleration (m/s) en Z : %lf\n",(rxBuf[4]*pow(2,9)+rxBuf[5])/pow(2,15)*9.81);
    }
    else DebugTrace("Error config\n");
}

int main (void)
{

    /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */

  I2CConfig maConfigI2C = {
  .op_mode = OPMODE_I2C,
  .clock_speed = 100000,
  .duty_cycle = STD_DUTY_CYCLE
};

  halInit();
  chSysInit();
  initHeap();
  radioLinkStart(); // démarrage du module de liaison avec le recepteur de radiocommande
  consoleInit();
  chThdCreateStatic(waBlinker, sizeof(waBlinker), NORMALPRIO, &blinker, NULL); 
  i2cStart(&I2CD1, &maConfigI2C);

  /* ========================
     Debut de votre code principal : 
     ======================== */
  while(1){
    i2cBaroPrintDevId();
    chThdSleepMilliseconds(1000);
    DebugTrace("%f",i2cBaroGetPressure(&I2CD1));
    chThdSleepMilliseconds(1000);
    calculTemp();
    chThdSleepMilliseconds(1000);
    accelZ();
  }

  /* ========================
     fin de cotre code 
     ce qui suit ne doit jamais être executé 
     ======================== */
  DebugTrace("sortir du main n'a pas de sens pour un programme embarqué");
  // le cligotement passe à 10Hz pour indiquer que l'on est arrivé là par erreur
  blinkPeriod = 100;
  chThdSleep(TIME_INFINITE);
}

  


/* ========================
     Debut de vos fonctions qui seront appelées dans votre code principal
   ======================== */
 
