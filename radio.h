#pragma once
#include "frskyFport_fsm.h"

/**
 * @brief   axis of Frsky Taranis controller
 * @note    used as argument for radioGetControl functin
 */
typedef enum  {
  PITCH_LEFT_JOYSTICK=0, ROLL_RIGHT_JOYSTICK,
  PITCH_RIGHT_JOYSTICK, ROLL_LEFT_JOYSTICK,
  ROTARY_KNOB,
  SWITCH_A_3P, SWITCH_B_LONG_3P, SWITCH_C_3P,
  SWITCH_D_SIDE_2P, SWITCH_E_SIDE_MONOSTABLE_2P
} ControllerAxis;

/**
 * @brief   Type of function pointer of the function that send telemetry
 *          back to the controller.
 * @note    The function must wait 300µs, then send back the buffer on the UART choosen
 *          for the controller communication
 */
typedef void(*RadioTelemetryCb_t)(const void *buffer, size_t len);

/**
 * @brief   Radio library initialisation function, must be called first
 * @param[in] sendTlmAfter300msCb : pointer to the function that will 
 *            send back telemetry frame
 * @note    If upstrem telemetry is not used (one do not want to display
 *          parameters on the controller screen), this argument must be NULL
 */
void    radioInit(RadioTelemetryCb_t sendTlmAfter300msCb);


/**
 * @brief   
 * @param[in] byte : byte that just has been read from the UART  
 *            
 * @note    this fonction is the entry point to the module state machine that decode 
 *          frsky frames. This function must be called in a loop in a dedicated thread
 */
FportErrorMask radioFeedByte(uint8_t byte);


/**
 * @brief   
 * @param[in] axis : axis of the controller we want to know current value
 * @return normalised value read on that axis in the range -1.0f, 1.0f           
 * @note  ° see ControllerAxis enum to see the list of available axis
 *        ° for joysticks and knob, return [-1.0 .. 1.0] value 
 *        ° for 2 positions switches, return [0 or != 0] value
 *        ° for 3 positions switches, return [<0, 0 or >0] value
 */
float   radioGetControl(ControllerAxis axis);


/**
 * @brief get Received Signal Strength Indicator
 * @return value in rand [0 .. 100] from no signal to excellent signal
 */
uint8_t radioGetRssi(void);


/**
 * @brief get status of the decoder state machine
 * @return FportErrorMask value, see description of enum in frskyFport_fsm.h
 * @note : possibles values  : FPORT_OK, FPORT_MALFORMED_FRAME,
 *    FPORT_CRC_ERROR, FPORT_APPID_LEN_EXCEDEED,
 *    FPORT_TIMEOUT, FPORT_FAILSAFE, FPORT_RADIO_LINK_LOST,
 *    FPORT_READBACK_CTRL_FAIL, FPORT_INTERNAL_ERROR
 */
FportErrorMask radioGetStatus(void);

/**
 * @brief send back upstream telemetry value that will be 
 * @param[in] id : id of the parameter that we send
 * @param[in] value : value of the parameter that we send
 * @note : ° see description of id in frskyFportAppId.h
 *         ° sendTlmAfter300msCb must be implemented
 *         ° radioInit must be called with sendTlmAfter300msCb address
 */
void    radioSetTelemetryParam(fportAppId id, uint32_t value);


/**
 * @brief return ascii version of FportErrorMask status
 * @param[in] errMsk : error reported by radioGetStatus or 
 *            radioFeedByte functions
 * @return : pointer to null terminated string describing status
 * @note : for debug purpose
 */
const char* radioErrorMessageString(FportErrorMask errMsk);


/**
 * @brief custom ID for uptime in seconds
 */
#define FPORT_CUSTOM_UPTIME_ID 0x1000

/**
 * @brief custom ID for QNH in pascal
 */
#define FPORT_CUSTOM_QNH_ID 0x1001

