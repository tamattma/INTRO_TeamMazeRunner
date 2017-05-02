/**
 * \file
 * \brief Main application file
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This provides the main application entry point.
 */

#include "Platform.h"
#include "Platform_Local.h"
#include "Application.h"
#include "Event.h"
#include "LED.h"
#include "WAIT1.h"
#include "CS1.h"
#include "Keys.h"
#include "KeyDebounce.h"
#include "KIN1.h"
#if PL_CONFIG_HAS_SHELL
  #include "CLS1.h"
  #include "Shell.h"
#endif
#if PL_CONFIG_HAS_BUZZER
  #include "Buzzer.h"
#endif
#if PL_CONFIG_HAS_RTOS
  #include "FRTOS1.h"
  #include "RTOS.h"
#endif
#if PL_CONFIG_HAS_QUADRATURE
  #include "Q4CLeft.h"
  #include "Q4CRight.h"
#endif
#if PL_CONFIG_HAS_MOTOR
  #include "Motor.h"
#endif
#if PL_CONFIG_BOARD_IS_ROBO_V2
  #include "PORT_PDD.h"
#endif
#if PL_CONFIG_HAS_LINE_FOLLOW
  #include "LineFollow.h"
#endif
#if PL_CONFIG_HAS_LCD_MENU
  #include "LCD.h"
#include "LCDMenu.h"
#endif
#include "Snake.h"


int cntr = 0;
//Snake_Status gameState;

#if PL_CONFIG_HAS_EVENTS
void APP_EventHandler(EVNT_Handle event) {
  /*! \todo handle events */
  switch(event) {
  case EVNT_STARTUP:
    break;

  case EVNT_LED_HEARTBEAT:
	  //do heartbeat stuff
	  LED1_On();
	  WAIT1_Waitms(100);
	  LED1_Off();
	  WAIT1_Waitms(100);
	  LED1_On();
	  WAIT1_Waitms(100);
	  LED1_Off();
	  break;

  case EVNT_HELLO_WORLD:
	  CLS1_SendNum32s(cntr++, CLS1_GetStdio()->stdOut);
	  CLS1_SendStr((uint8_t*)"  loading...\r\n", CLS1_GetStdio()->stdOut);
	  break;

  case EVNT_SW1_PRESSED: // Joystick nach rechts
	  SNAKE_ButtonRight();
	  /*gameState = Snake_GetStatus();
	  if (gameState != Snake_NotRunning){
		  if (gameState = Snake_Running){
			  Snake_SetRightEVNT();
		  }
		  else if (gameState = Snake_Pause){
			  Snake_AnyButton();
		  }
	  }
	  else{
		  //LCD_SetEvent(LCD_BTN_RIGHT);
	  }*/
	  //LED1_On();
	  //CLS1_SendNum32s(cntr++, CLS1_GetStdio()->stdOut);
	  //CLS1_SendStr((uint8_t*)"  I'm alive!!!\r\n", CLS1_GetStdio()->stdOut);
	  //CLS1_SendStr((uint8_t*)"  EVNT_SW1_PRESSED\r\n", CLS1_GetStdio()->stdOut);
	  //LED1_Off();
	  break;
  case EVNT_SW1_RELEASED:
	  //CLS1_SendNum32s(cntr++, CLS1_GetStdio()->stdOut);
	  //CLS1_SendStr((uint8_t*)"  EVNT_SW1_RELEASED\r\n", CLS1_GetStdio()->stdOut);
	  break;
  case EVNT_SW1_LPRESSED:
	  //CLS1_SendNum32s(cntr++, CLS1_GetStdio()->stdOut);
	  //CLS1_SendStr((uint8_t*)"  EVNT_SW1_LPRESSED\r\n", CLS1_GetStdio()->stdOut);
	  break;

  case EVNT_SW2_PRESSED: // Joystick nach links
	  SNAKE_ButtonLeft();
	  /*gameState = Snake_GetStatus();
	  if (gameState != Snake_NotRunning){
		  if (gameState = Snake_Running){
			  Snake_SetLeftEVNT();
		  }
		  else if (gameState = Snake_Pause){
			  Snake_AnyButton();
		  }
	  }
	  else{
		  //LCD_SetEvent(LCD_BTN_LEFT);
	  }*/
	  //LED1_On();
	  //CLS1_SendNum32s(cntr++, CLS1_GetStdio()->stdOut);
	  //CLS1_SendStr((uint8_t*)"  Feed me!!!\r\n", CLS1_GetStdio()->stdOut);
	  //CLS1_SendStr((uint8_t*)"  EVNT_SW2_PRESSED\r\n", CLS1_GetStdio()->stdOut);
	  //LED1_Off();
	  break;
  	case EVNT_SW2_RELEASED:
  	  //CLS1_SendNum32s(cntr++, CLS1_GetStdio()->stdOut);
  	  //CLS1_SendStr((uint8_t*)"  EVNT_SW2_RELEASED\r\n", CLS1_GetStdio()->stdOut);
  	  break;
    case EVNT_SW2_LPRESSED:
      //CLS1_SendNum32s(cntr++, CLS1_GetStdio()->stdOut);
      //CLS1_SendStr((uint8_t*)"  EVNT_SW2_LPRESSED\r\n", CLS1_GetStdio()->stdOut);
  	  break;

  case EVNT_SW3_PRESSED: // Joystick nach unten
	  SNAKE_ButtonDown();
	  /*gameState = Snake_GetStatus();
	  if (gameState != Snake_NotRunning){
		  if (gameState = Snake_Running){
			  Snake_SetDownEVNT();
		  }
		  else if (gameState = Snake_Pause){
			  Snake_AnyButton();
		  }
	  }
	  else{
		  //LCD_SetEvent(LCD_BTN_DOWN);
	  }*/
	  //LED1_On();
	  //CLS1_SendNum32s(cntr++, CLS1_GetStdio()->stdOut);
	  //CLS1_SendStr((uint8_t*)"  I'm hungry!\r\n", CLS1_GetStdio()->stdOut);
	  //CLS1_SendStr((uint8_t*)"  EVNT_SW3_PRESSED\r\n", CLS1_GetStdio()->stdOut);
	  //LED1_Off();
	  break;
  case EVNT_SW3_RELEASED:
	  //CLS1_SendNum32s(cntr++, CLS1_GetStdio()->stdOut);
	  //CLS1_SendStr((uint8_t*)"  EVNT_SW3_RELEASED\r\n", CLS1_GetStdio()->stdOut);
	  break;
  case EVNT_SW3_LPRESSED:
	  //CLS1_SendNum32s(cntr++, CLS1_GetStdio()->stdOut);
	  //CLS1_SendStr((uint8_t*)"  EVNT_SW3_LPRESSED\r\n", CLS1_GetStdio()->stdOut);
	  break;

  case EVNT_SW4_PRESSED: // Joystick gedr�ckt
	  SNAKE_ButtonCenter();
	  /*gameState = Snake_GetStatus();
	  if (gameState != Snake_NotRunning){
		  if (gameState = Snake_Running){
			  Snake_SetPauseEVNT();
		  }
		  else if (gameState = Snake_Pause){
			  Snake_AnyButton();
		  }
	  }
	  else{
		  //LCD_SetEvent(LCD_BTN_CENTER);
	  }*/
	  //LED1_On();
	  //CLS1_SendNum32s(cntr++, CLS1_GetStdio()->stdOut);
	  //CLS1_SendStr((uint8_t*)"  Entertain me!\r\n", CLS1_GetStdio()->stdOut);
	  //CLS1_SendStr((uint8_t*)"  EVNT_SW4_PRESSED\r\n", CLS1_GetStdio()->stdOut);
	  //LED1_Off();
	  break;
  case EVNT_SW4_RELEASED:
	  //CLS1_SendNum32s(cntr++, CLS1_GetStdio()->stdOut);
	  //CLS1_SendStr((uint8_t*)"  EVNT_SW4_RELEASED\r\n", CLS1_GetStdio()->stdOut);
	  break;
  case EVNT_SW4_LPRESSED:
	  //CLS1_SendNum32s(cntr++, CLS1_GetStdio()->stdOut);
	  //CLS1_SendStr((uint8_t*)"  EVNT_SW4_LPRESSED\r\n", CLS1_GetStdio()->stdOut);
	  break;

  case EVNT_SW5_PRESSED: // Joystick nach oben
	  SNAKE_ButtonUp();
	  /*gameState = Snake_GetStatus();
	  if (gameState != Snake_NotRunning){
		  if (gameState = Snake_Running){
			  Snake_SetUpEVNT();
		  }
		  else if (gameState = Snake_Pause){
			  Snake_AnyButton();
		  }
	  }
	  else{
		  //LCD_SetEvent(LCD_BTN_UP);
	  }*/
	  //LED1_On();
	  //CLS1_SendNum32s(cntr++, CLS1_GetStdio()->stdOut);
	  //CLS1_SendStr((uint8_t*)"  Send me some data!\r\n", CLS1_GetStdio()->stdOut);
	  //CLS1_SendStr((uint8_t*)"  EVNT_SW5_PRESSED\r\n", CLS1_GetStdio()->stdOut);
	  //LED1_Off();
	  break;
  case EVNT_SW5_RELEASED:
	  //CLS1_SendNum32s(cntr++, CLS1_GetStdio()->stdOut);
	  //CLS1_SendStr((uint8_t*)"  EVNT_SW5_RELEASED\r\n", CLS1_GetStdio()->stdOut);
	  break;
  case EVNT_SW5_LPRESSED:
	  //CLS1_SendNum32s(cntr++, CLS1_GetStdio()->stdOut);
	  //CLS1_SendStr((uint8_t*)"  EVNT_SW5_LPRESSED\r\n", CLS1_GetStdio()->stdOut);
	  break;

  case EVNT_SW6_PRESSED: // Button left
	  //LED1_On();
	  //CLS1_SendNum32s(cntr++, CLS1_GetStdio()->stdOut);
	  //CLS1_SendStr((uint8_t*)"  I'm so lonely...\r\n", CLS1_GetStdio()->stdOut);
	  //CLS1_SendStr((uint8_t*)"  EVNT_SW6_PRESSED\r\n", CLS1_GetStdio()->stdOut);
	  //LED1_Off();
	  break;
  case EVNT_SW6_RELEASED:
	  //CLS1_SendNum32s(cntr++, CLS1_GetStdio()->stdOut);
	  //CLS1_SendStr((uint8_t*)"  EVNT_SW6_RELEASED\r\n", CLS1_GetStdio()->stdOut);
	  break;
  case EVNT_SW6_LPRESSED:
	  //CLS1_SendNum32s(cntr++, CLS1_GetStdio()->stdOut);
	  //CLS1_SendStr((uint8_t*)"  EVNT_SW6_LPRESSED\r\n", CLS1_GetStdio()->stdOut);
	  break;

  case EVNT_SW7_PRESSED: // Button right
	  //LED1_On();
	  //CLS1_SendNum32s(cntr++, CLS1_GetStdio()->stdOut);
	  //CLS1_SendStr((uint8_t*)"  Never ever unplug me again!\r\n", CLS1_GetStdio()->stdOut);
	  //CLS1_SendStr((uint8_t*)"  EVNT_SW7_PRESSED\r\n", CLS1_GetStdio()->stdOut);
	  //LED1_Off();
	  break;
	case EVNT_SW7_RELEASED:
		//CLS1_SendNum32s(cntr++, CLS1_GetStdio()->stdOut);
		//CLS1_SendStr((uint8_t*)"  EVNT_SW7_RELEASED\r\n", CLS1_GetStdio()->stdOut);
	  break;
  case EVNT_SW7_LPRESSED:
	  //CLS1_SendNum32s(cntr++, CLS1_GetStdio()->stdOut);
	  //CLS1_SendStr((uint8_t*)"  EVNT_SW7_LPRESSED\r\n", CLS1_GetStdio()->stdOut);
	  break;



  default:
    break;
   } /* switch */
}
#endif /* PL_CONFIG_HAS_EVENTS */

static const KIN1_UID RoboIDs[] = {
  /* 0: L20, V2 */ {{0x00,0x03,0x00,0x00,0x67,0xCD,0xB7,0x21,0x4E,0x45,0x32,0x15,0x30,0x02,0x00,0x13}},
  /* 1: L21, V2 */ {{0x00,0x05,0x00,0x00,0x4E,0x45,0xB7,0x21,0x4E,0x45,0x32,0x15,0x30,0x02,0x00,0x13}},
  /* 2: L4, V1  */ {{0x00,0x0B,0xFF,0xFF,0x4E,0x45,0xFF,0xFF,0x4E,0x45,0x27,0x99,0x10,0x02,0x00,0x24}}, /* revert right motor */
  /* 3: L23, V2 */ {{0x00,0x0A,0x00,0x00,0x67,0xCD,0xB8,0x21,0x4E,0x45,0x32,0x15,0x30,0x02,0x00,0x13}}, /* revert left & right motor */
  /* 4: L11, V2 */ {{0x00,0x19,0x00,0x00,0x67,0xCD,0xB9,0x11,0x4E,0x45,0x32,0x15,0x30,0x02,0x00,0x13}}, /* revert right encoder, possible damaged motor driver? */
  /* 5: L4, V1 */  {{0x00,0x0B,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x4E,0x45,0x27,0x99,0x10,0x02,0x00,0x24}},
};

static void APP_AdoptToHardware(void) {
  KIN1_UID id;
  uint8_t res;

  res = KIN1_UIDGet(&id);
  if (res!=ERR_OK) {
    for(;;); /* error */
  }
#if PL_CONFIG_HAS_MOTOR
  if (KIN1_UIDSame(&id, &RoboIDs[2])) { /* L4 */
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_LEFT), TRUE); /* revert left motor */
    (void)Q4CLeft_SwapPins(TRUE);
    (void)Q4CRight_SwapPins(TRUE);
  } else if (KIN1_UIDSame(&id, &RoboIDs[0])) { /* L20 */
    (void)Q4CRight_SwapPins(TRUE);
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_LEFT), TRUE); /* revert left motor */
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), TRUE); /* revert left motor */
  } else if (KIN1_UIDSame(&id, &RoboIDs[3])) { /* L23 */
    (void)Q4CRight_SwapPins(TRUE);
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_LEFT), TRUE); /* revert left motor */
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), TRUE); /* revert left motor */
  } else if (KIN1_UIDSame(&id, &RoboIDs[4])) { /* L11 */
    (void)Q4CRight_SwapPins(TRUE);
  } else if (KIN1_UIDSame(&id, &RoboIDs[5])) { /* L4 */
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_LEFT), TRUE); /* revert left motor */
    (void)Q4CLeft_SwapPins(TRUE);
    (void)Q4CRight_SwapPins(TRUE);
  }
#endif
#if PL_CONFIG_HAS_QUADRATURE && PL_CONFIG_BOARD_IS_ROBO_V2
  /* pull-ups for Quadrature Encoder Pins */
  PORT_PDD_SetPinPullSelect(PORTC_BASE_PTR, 10, PORT_PDD_PULL_UP);
  PORT_PDD_SetPinPullEnable(PORTC_BASE_PTR, 10, PORT_PDD_PULL_ENABLE);
  PORT_PDD_SetPinPullSelect(PORTC_BASE_PTR, 11, PORT_PDD_PULL_UP);
  PORT_PDD_SetPinPullEnable(PORTC_BASE_PTR, 11, PORT_PDD_PULL_ENABLE);
  PORT_PDD_SetPinPullSelect(PORTC_BASE_PTR, 16, PORT_PDD_PULL_UP);
  PORT_PDD_SetPinPullEnable(PORTC_BASE_PTR, 16, PORT_PDD_PULL_ENABLE);
  PORT_PDD_SetPinPullSelect(PORTC_BASE_PTR, 17, PORT_PDD_PULL_UP);
  PORT_PDD_SetPinPullEnable(PORTC_BASE_PTR, 17, PORT_PDD_PULL_ENABLE);
#endif
}

#include "CLS1.h"

void App_HandleEvents()
{
	TickType_t xLastWakeTime = xTaskGetTickCount();
	for(;;){
		KEY_Scan();
		EVNT_HandleEvent(APP_EventHandler, TRUE);
		vTaskDelayUntil(&xLastWakeTime, 500/portTICK_PERIOD_MS);
	}
}

void APP_Start(void) {
#if PL_CONFIG_HAS_RTOS
#if configUSE_TRACE_HOOKS /* FreeRTOS using Percepio Trace */
  #if TRC_CFG_RECORDER_MODE==TRC_RECORDER_MODE_SNAPSHOT
    vTraceEnable(TRC_START); /* start streaming */
  #elif TRC_CFG_RECORDER_MODE==TRC_RECORDER_MODE_STREAMING
  //  vTraceEnable(TRC_INIT); /* Streaming trace, start streaming */
  #endif
#endif /* configUSE_TRACE_HOOKS */
#endif
  PL_Init();
#if PL_CONFIG_HAS_EVENTS
  EVNT_SetEvent(EVNT_STARTUP);
#endif
#if PL_CONFIG_HAS_SHELL && CLS1_DEFAULT_SERIAL
  CLS1_SendStr((uint8_t*)"Hello World!\r\n", CLS1_GetStdio()->stdOut);
#endif
  APP_AdoptToHardware();
#if PL_CONFIG_HAS_RTOS
  BaseType_t res;
  xTaskHandle taskHndl;
  res = xTaskCreate(App_HandleEvents, "EvntHandler", configMINIMAL_STACK_SIZE+50, (void*) NULL, tskIDLE_PRIORITY+1, &taskHndl);
 vTaskStartScheduler(); /* start the RTOS, create the IDLE task and run my tasks (if any) */
  /* does usually not return! */
#else
  __asm volatile("cpsie i"); /* enable interrupts */
  for(;;) {
	  WAIT1_Waitms(100); /* just wait for some arbitrary time */

	  KEY_Scan();
	  EVNT_HandleEvent(APP_EventHandler, TRUE);
  }
#endif
}

