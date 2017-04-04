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

static void BlinkyTask2(void *pvParameters){
	TickType_t xLastWakeTime = xTaskGetTickCount();
	for(;;){
		LED2_Neg();
		vTaskDelayUntil(&xLastWakeTime, 300/portTICK_PERIOD_MS);
	}
}

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
	xTaskHandle taskBlinkHndl2 ;
		BaseType_t res2 = xTaskCreate( 	BlinkyTask2,
										"Blinky2",
										configMINIMAL_STACK_SIZE+50,
										(void*) NULL,
										tskIDLE_PRIORITY,
										&taskBlinkHndl2
										);
#endif

#if PL_LOCAL_CONFIG_BOARD_IS_REMOTE

#endif
}

void RTOS_Deinit(void) {
  /* nothing needed for now */
}

#endif /* PL_CONFIG_HAS_RTOS */
