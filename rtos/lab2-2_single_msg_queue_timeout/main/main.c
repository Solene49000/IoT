#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "esp_log.h"

/* FreeRTOS.org includes. */
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "my_helper_fct.h"


/**
 * @brief Starting point function
 * 
 */

/* Default stack size for tasks */
static const uint32_t STACK_SIZE = 4000;

/* Task Priority */
static const uint32_t T1_PRIO = 2;
static const uint32_t T2_PRIO = 2;
static const uint32_t T3_PRIO = 3;

/*Task period*/
#define TASK_PERIOD pdMS_TO_TICKS(500)

/* Used as a loop counter to create a very crude delay. */
//#define mainDELAY_LOOP_COUNT		(0x1FFFFF)

/* The tasks functions. */
void Task1(void *pvParameters);
void Task2(void *pvParameters);
void Task3(void *pvParameters);

/*Queue global declaration*/
QueueHandle_t xQueue1;

void app_main(void) {
	
	DISPLAY("Start of app_main task, priority = %d",uxTaskPriorityGet(NULL));
	
	/*Create the Queue*/
	xQueue1 = xQueueCreate(5, sizeof(uint32_t));
	
	vTaskSuspendAll () ;
	
	/* Create the first tasks. */
	
	xTaskCreatePinnedToCore(Task1,	/* Pointer to the function that implements the task. */
		"Task 1",				/* Text name for the task.  This is to facilitate debugging only. */
		STACK_SIZE,				/* Stack depth  */
		(void*)"Task 1",		/* Pass the text to be printed in as the task parameter. */
		T1_PRIO,				/* Task priority */
		NULL,
		CORE_1);					/* We are not using the task handle. */

	/* Create the second task. */
	xTaskCreatePinnedToCore(Task2,	/* Pointer to the function that implements the task. */
		"Task 2",				/* Text name for the task.  This is to facilitate debugging only. */
		STACK_SIZE,				/* Stack depth  */
		(void*)"Task 2",		/* Pass the text to be printed in as the task parameter. */
		T2_PRIO,				/* Task priority */
		NULL,
		CORE_0);					/* We are not using the task handle. */

	/* Create the third task. */
	xTaskCreatePinnedToCore(Task3,	/* Pointer to the function that implements the task. */
		"Task 3",				/* Text name for the task.  This is to facilitate debugging only. */
		STACK_SIZE,				/* Stack depth  */
		(void*)"Task 3",		/* Pass the text to be printed in as the task parameter. */
		T3_PRIO,				/* Task priority */
		NULL,
		CORE_0);	
	


	xTaskResumeAll () ;

	DISPLAY("==== Exit APP_MAIN ====");
	
	/* to ensure its exit is clean */
	vTaskDelete(NULL);
}
/*-----------------------------------------------------------*/

void Task1(void *pvParameters) {
	
	TickType_t xLastWakeTime;

    // Initialise xLastWakeTime avec l'heure actuelle
    xLastWakeTime = xTaskGetTickCount();
	int32_t message = 42;
	// Post
	for (;; ) {
		uint32_t result = xQueueSend (xQueue1, &message, 0) ;
		// Check result
		if ( result ) {
			DISPLAY("POSTED : Message envoyé");
		}
		else{
			DISPLAY("FAILED : Erreur envoi message");
		}
		// Compute time : 40 ms
		COMPUTE_IN_TIME_MS (40) ;
		// block periodically : 500 ms
		 vTaskDelayUntil(&xLastWakeTime, TASK_PERIOD);
	}
	
}

void Task2(void *pvParameters) {
	static const char * TAG = " MsgTimeOut ";
	int32_t lReceivedValue;
	//BaseType_t xStatus;
	for (;; ) {
		/*
		xQueueReceive(xQueue1, &lReceivedValue, portMAX_DELAY);		
		// display task number and message
		DISPLAY ("Tache 2 a recu un message : %d", lReceivedValue) ;
		// Compute time : 30 ms
		COMPUTE_IN_TIME_MS (30) ;*/
		if ( xQueueReceive (xQueue1, &lReceivedValue, pdMS_TO_TICKS(300)) ) {
			DISPLAYI (TAG , " Task 2 , mess = %d", lReceivedValue ) ;
			COMPUTE_IN_TIME_MS (30) ;
		}
		else {
			DISPLAYE (TAG , " Task 2 , Timeout !") ;
			COMPUTE_IN_TIME_MS (10) ;
		}

	}
}

void Task3(void *pvParameters) {
	for (;; ) {
	// Task blocked during 100 ms
		DISPLAY ("Task3 go to blocked state for 100ms") ;
		vTaskDelay ( pdMS_TO_TICKS (100) ) ;
		DISPLAY ("Task3 is deblocked") ;
		// Compute time : 20 ms
		COMPUTE_IN_TIME_MS (20) ;
	}
}