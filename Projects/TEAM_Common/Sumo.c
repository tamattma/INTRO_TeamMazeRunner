/**
 * \file
 * \brief Sumo strategy module
 * \author Remo Mattmann, Caroline Brunner
 *
 * Module to drive robot through sumo battle
 */

#include "Platform.h" /* interface to the platform */
#if PL_LOCAL_CONFIG_BOARD_IS_ROBO
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
#if PL_HAS_DISTANCE_SENSOR
  #include "Distance.h"
#endif

SUMO_States state;
SUMO_Strategy strategy;
uint8_t wall;
int32_t MAX_SPEED = 6000; // muss noch abgeklärt werden
bool running;

bool SUMO_isRunning (void) {
	return running;
}

void SUMO_StateMachine (void) {
	switch(state) {
	case SUMO_IDLE:		// wait on start signal
	//	DRV_Stop(50/portTICK_PERIOD_MS);
	break;

	case SUMO_WAIT_5s:	// wait and beep for 5s
		BUZ_Beep(392, 100);
		vTaskDelay(999/portTICK_PERIOD_MS);
		BUZ_Beep(392, 100);
		vTaskDelay(999/portTICK_PERIOD_MS);
		BUZ_Beep(392, 100);
		vTaskDelay(999/portTICK_PERIOD_MS);
		BUZ_Beep(523, 100);
		vTaskDelay(499/portTICK_PERIOD_MS);
		BUZ_Beep(523, 100);
		vTaskDelay(499/portTICK_PERIOD_MS);
		BUZ_Beep(784, 1000);
		vTaskDelay(999/portTICK_PERIOD_MS);

		switch (strategy) {
		case SUMO_DUMMY:
			state = SUMO_DUMMY_DRIVE;
			break;

		case SUMO_VOLLGAS:
			state = SUMO_VOLLGAS_SEARCH;
			break;

		case SUMO_TRAP:
			state = SUMO_TRAP_SEARCH;
			break;

		case SUMO_MIXED:
			state = SUMO_TRAP_SEARCH;
			break;

		default:
			state = SUMO_DUMMY_DRIVE;
			break;
		}
	break;

	case SUMO_DUMMY_DRIVE:	// drive with middle speed until line reached
		DRV_SetMode(DRV_MODE_SPEED);
		DRV_SetSpeed(1000, 1000);
		do{
			vTaskDelay(50/portTICK_PERIOD_MS);
		} while(REF_GetLineKind()==REF_LINE_FULL);
		TURN_Turn(TURN_STEP_BORDER_BW, NULL); // drive backward for some time
		state = SUMO_DUMMY_TURN;
	break;

	case SUMO_DUMMY_TURN:	// turn
		TURN_TurnAngle(120, NULL);
		state = SUMO_DUMMY_DRIVE;
	break;

	case SUMO_VOLLGAS_FORWARD:	// drive with full speed until line reached
		DRV_SetMode(DRV_MODE_SPEED);
		DRV_SetSpeed(MAX_SPEED, MAX_SPEED);
		while(REF_GetLineKind()==REF_LINE_FULL){
			vTaskDelay(5/portTICK_PERIOD_MS);
		}
		TURN_Turn(TURN_STEP_BORDER_BW, NULL); // drive backward for some time
		state = SUMO_VOLLGAS_LINE;
	break;

	case SUMO_VOLLGAS_LINE:		// reached line and step back
		TURN_Turn(TURN_STEP_BORDER_BW, NULL); // drive backward for some time
		state = SUMO_VOLLGAS_SEARCH;
	break;

	case SUMO_VOLLGAS_SEARCH:	// search enemy
		wall = DIST_CheckSurrounding();
		if (wall == 0) {
			TURN_TurnAngle(10, NULL); // etwas drehen
		} else if (wall == 0xf) {
			state = SUMO_DUMMY_DRIVE; // Fehler
		} else {	// Objekt gesichtet, aber wo?
			if (wall < 2) {			// Front
				state = SUMO_VOLLGAS_FORWARD;
			} else if (wall < 3) {	// Rear
				TURN_TurnAngle(160, NULL);
			} else if (wall < 5) {	// Left
				TURN_TurnAngle(-100, NULL);
			} else if (wall < 9) {	// Right
				TURN_TurnAngle(80, NULL);
			}
		}
	break;

	case SUMO_TRAP_SEARCH:	// search enemy on left or right sensor
		wall = DIST_CheckSurrounding();
		if (wall == 0) {
			TURN_TurnAngle(10, NULL); // etwas drehen
		} else {	// Objekt gesichtet, aber wo?
			if (wall < 2) {			// Front
				TURN_TurnAngle(-100, NULL);
			} else if (wall < 3) {	// Rear
				TURN_TurnAngle(80, NULL);
			} else if (wall < 5) {	// Left
				TURN_TurnAngle(170, NULL);
			} else if (wall < 9) {	// Right
				state = SUMO_TRAP_WAIT;
			}
		}
	break;

	case SUMO_TRAP_WAIT:	// wait until enemy is near
		while(!DIST_NearRightObstacle(150)){
			vTaskDelay(5/portTICK_PERIOD_MS);
		}
		DRV_SetSpeed(-MAX_SPEED, -MAX_SPEED);
		state = SUMO_TRAP_BACK;
	break;

	case SUMO_TRAP_BACK:	// step back and wait for enemy to drive in front
		DRV_Stop(50/portTICK_PERIOD_MS);
		while(!DIST_NearFrontObstacle(50)) {
			vTaskDelay(20/portTICK_PERIOD_MS);
		}
	break;

	case SUMO_TRAP_FORWARD:	// drive forward
		DRV_SetMode(DRV_MODE_SPEED);
		DRV_SetSpeed(MAX_SPEED, MAX_SPEED);
		while(REF_GetLineKind()==REF_LINE_FULL){
			vTaskDelay(5/portTICK_PERIOD_MS);
		}
		DRV_Stop(50/portTICK_PERIOD_MS);
		if(strategy == SUMO_TRAP) {
			state = SUMO_TRAP_SEARCH;
		} else if (strategy == SUMO_MIXED) {
			state = SUMO_VOLLGAS_LINE;
		} else {
			state = SUMO_DUMMY_TURN;
		}
	break;

	default:
		state = SUMO_VOLLGAS_SEARCH;
	break;
	}
}

static void SumoTask (void *pvParameters) {
  (void)pvParameters; /* not used */
  TickType_t xLastWakeTime = xTaskGetTickCount();
  for(;;) {
    SUMO_StateMachine();
    vTaskDelayUntil(&xLastWakeTime, 20/portTICK_PERIOD_MS);
  }
}

void SUMO_Start(SUMO_Strategy strat){
	strategy = strat;
	state = SUMO_WAIT_5s;
	running = TRUE;
}

void SUMO_Stop(){
	state = SUMO_IDLE;
	running = FALSE;
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
#endif
