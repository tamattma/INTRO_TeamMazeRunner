/**
 * \file
 * \brief Sumo strategy module
 * \author Remo Mattmann, Caroline Brunner
 *
 * Module to drive robot through sumo battle
 */

#include "Platform.h" /* interface to the platform */

#include "Sumo.h"    /* our own interface */
#if PL_CONFIG_HAS_RTOS
  #include "FRTOS1.h"
#endif
#if PL_CONFIG_HAS_BUZZER
  #include "Buzzer.h"
#endif
#if PL_CONFIG_HAS_REFLECTANCE
  #include "Reflectance.h"
#endif
#if PL_CONFIG_HAS_DRIVE
  #include "Drive.h"
#endif
#if PL_CONFIG_HAS_TURN
  #include "Turn.h"
#endif

SUMO_States state;
SUMO_Strategy strategy;

void SUMO_StateMachine (void) {
	switch(state) {
	case SUMO_IDLE:		// wait on start signal

	break;

	case SUMO_START:	// set strategy

	break;

	case SUMO_WAIT_5s:	// wait and beep for 5s
		BUZ_Beep(400, 100);
	break;

	case SUMO_DUMMY_DRIVE:	// drive with middle speed until line reached

	break;

	case SUMO_DUMMY_TURN:	// turn

	break;

	case SUMO_VOLLGAS_FORWARD:	// drive with full speed until line reached

	break;

	case SUMO_VOLLGAS_LINE:		// reached line and step back

	break;

	case SUMO_VOLLGAS_SEARCH:	// search enemy

	break;

	case SUMO_TRAP_SEARCH:	// search enemy on left or right sensor

	break;

	case SUMO_TRAP_WAIT:	// wait until enemy is near

	break;

	case SUMO_TRAP_BACK:	// step back and wait for enemy to drive in front

	break;

	case SUMO_TRAP_FORWARD:	// drive forward

	break;

	default:
		state = SUMO_VOLLGAS_SEARCH;
	break;
	}
}

static void SumoTask (void *pvParameters) {
  (void)pvParameters; /* not used */
  for(;;) {
    SUMO_StateMachine();
    FRTOS1_vTaskDelay(20/portTICK_PERIOD_MS);
  }
}


void SUMO_Deinit(void) {

}


void SUMO_Init(void) {
	state = SUMO_IDLE;
	strategy = SUMO_MIXED;
	if (xTaskCreate(SumoTask, "Sumo", 600/sizeof(StackType_t), NULL, tskIDLE_PRIORITY+2, NULL) != pdPASS) {
	    for(;;){} /* error */
	}
}
