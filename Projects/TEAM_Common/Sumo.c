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
int32_t MAX_SPEED = 2000; // muss noch abgeklärt werden
bool running;

int32_t driveL;
int32_t driveR;

bool SUMO_isRunning (void) {
	return running;
}

void SUMO_StateMachine (void) {
	switch(state) {
	case SUMO_IDLE:		// wait on start signal
		DRV_SetSpeed(0,0);
		DRV_Stop(200/portTICK_PERIOD_MS);
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
		DRV_SetMode(DRV_MODE_SPEED);
	break;

	case SUMO_DUMMY_DRIVE:	// drive with middle speed until line reached
		DRV_SetMode(DRV_MODE_SPEED);
		DRV_SetSpeed(1000,1000);
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
		do{
			vTaskDelay(50/portTICK_PERIOD_MS);
		} while(REF_GetLineKind()==REF_LINE_FULL);
		TURN_Turn(TURN_STEP_BORDER_BW, NULL); // drive backward for some time
		TURN_Turn(TURN_STEP_BORDER_BW, NULL); // drive backward for some time
		state = SUMO_VOLLGAS_SEARCH;
	break;

	case SUMO_VOLLGAS_SEARCH:	// search enemy
		if (DIST_GetDistance(DIST_SENSOR_FRONT)>10) {
			state = SUMO_VOLLGAS_FORWARD;
		} else if (DIST_GetDistance(DIST_SENSOR_RIGHT)>10) {
			TURN_TurnAngle(80, NULL);
		} else if (DIST_GetDistance(DIST_SENSOR_LEFT)>10) {
			TURN_TurnAngle(-100, NULL);
		} else if (DIST_GetDistance(DIST_SENSOR_REAR)>10) {
			TURN_TurnAngle(160, NULL);
		} else {
			DRV_SetMode(DRV_MODE_SPEED);
			DRV_SetSpeed(800, -800); // drehen
		}
	break;

	case SUMO_TRAP_SEARCH:	// search enemy on left or right sensor
		if (DIST_GetDistance(DIST_SENSOR_RIGHT)<0) {
			DRV_SetMode(DRV_MODE_SPEED);
			DRV_SetSpeed(400, -400); // drehen
		} else {	// Objekt gesichtet
			state = SUMO_TRAP_WAIT;
		}
	break;

	case SUMO_TRAP_WAIT:	// wait until enemy is near
		while(DIST_GetDistance(DIST_SENSOR_RIGHT)>40){
			vTaskDelay(5/portTICK_PERIOD_MS);
		}
		TURN_Turn(TURN_STEP_LINE_BW_POST_LINE, NULL);
		state = SUMO_TRAP_BACK;

	break;

	case SUMO_TRAP_BACK:	// wait for enemy to drive in front
		while((DIST_GetDistance(DIST_SENSOR_FRONT)>100)||(DIST_GetDistance(DIST_SENSOR_FRONT)==-1)) {
			vTaskDelay(20/portTICK_PERIOD_MS);
		}
		state = SUMO_TRAP_FORWARD;
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
	strategy = SUMO_DUMMY;
	running = FALSE;
	if (xTaskCreate(SumoTask, "Sumo", 600/sizeof(StackType_t), NULL, tskIDLE_PRIORITY+2, NULL) != pdPASS) {
	    for(;;){} /* error */
	}
}
#endif
