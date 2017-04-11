/**
 * \file
 * \brief Real Time Operating System (RTOS) main program.
 * \author Erich Styger, erich.styger@hslu.ch
 */

#include "Platform.h"
#if PL_CONFIG_HAS_RTOS
#include "RTOS.h"
#include "FRTOS1.h"
#include "LED.h"

static void BlinkyTask(void *pvParameters){
	TickType_t xLastWakeTime = xTaskGetTickCount();
	for(;;){
		LED1_Neg();
		vTaskDelayUntil(&xLastWakeTime, 500/portTICK_PERIOD_MS);
	}
}

static void BlinkTask(void *pvParameters) {
	TickType_t xLastWakeTime = xTaskGetTickCount ();
	for (;;) {
		LED1_Neg();
		vTaskDelayUntil(&xLastWakeTime, 500/portTICK_PERIOD_MS );
	}
}
#endif
void RTOS_Init(void) {
  /*! \todo Create tasks here */
#if PL_LOCAL_CONFIG_BOARD_IS_ROBO

	xTaskHandle taskBlinkHndl ;
	BaseType_t res = xTaskCreate( 	BlinkyTask,
									"Blinky",
									configMINIMAL_STACK_SIZE+50,
									(void*) NULL,
									tskIDLE_PRIORITY,
									&taskBlinkHndl
									);

#if PL_LOCAL_CONFIG_BOARD_IS_REMOTE
BaseType_t res;
xTaskHandle taskHndl;
res = xTaskCreate(BlinkTask, "Blink", configMINIMAL_STACK_SIZE+50, (void*) NULL, tskIDLE_PRIORITY, &taskHndl);
#endif
}

void RTOS_Deinit(void) {
  /* nothing needed for now */
}

#endif /* PL_CONFIG_HAS_RTOS */
