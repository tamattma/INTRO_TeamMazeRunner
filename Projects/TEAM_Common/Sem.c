/**
 * \file
 * \brief Semaphore usage
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * Module using semaphores.
 */

/**
 * \file
 * \brief Semaphore usage
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * Module using semaphores.
 */

#include "Platform.h" /* interface to the platform */
#if PL_CONFIG_HAS_SEMAPHORE
#include "FRTOS1.h"
#include "Sem.h"
#include "LED.h"
#include "Buzzer.h"

static xSemaphoreHandle sem;

static void vSlaveTask(void *pvParameters) {
   for(;;) {
	   if(xSemaphoreTake(sem,10/portTICK_PERIOD_MS) == pdPASS){
#ifdef PL_LOCAL_CONFIG_BOARD_IS_ROBO
		   BUZ_PlayTune(BUZ_TUNE_WELCOME);
#elif PL_LOCAL_CONFIG_BOARD_IS_REMOTE
		   LED1_Neg();
#endif
	   }
	   vTaskDelay(50/portTICK_PERIOD_MS);
  }
}

static void vMasterTask(void *pvParameters) {
  /*! \todo send semaphore from master task to slave task */
  for(;;) {
    /*! \todo Implement functionality */
	  if(xSemaphoreGive(sem) == pdPASS){
#if PL_LOCAL_CONFIG_BOARD_IS_REMOTE
		  LED1_Neg();
#elif PL_LOCAL_CONFIG_BOARD_IS_ROBOT
		  LED2_Neg();
#endif
	  }
	  vTaskDelay(100/portTICK_PERIOD_MS);
  }
}

void SEM_Deinit(void) {
}

/*! \brief Initializes module */
void SEM_Init(void) {
  if (xTaskCreate(vMasterTask, "Master", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL) != pdPASS) {
    for(;;){} /* error */
  }
  if (xTaskCreate(vSlaveTask, "Master", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL) != pdPASS) {
      for(;;){} /* error */
    }
}
#endif /* PL_CONFIG_HAS_SEMAPHORE */
