/****************************************************************************
 * Copyright (C) 2020 by Fabrice Muller                                     *
 *                                                                          *
 * This file is useful for ESP32 Design course.                             *
 *                                                                          *
 ****************************************************************************/

/**
 * @file lab2-4_main.c
 * @author Fabrice Muller
 * @date 13 Oct. 2020
 * @brief File containing the lab2-4 of Part 3.
 *
 * @see https://github.com/fmuller-pns/esp32-vscode-project-template
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "esp_log.h"
#include "soc/rtc_wdt.h"
#include "driver/gpio.h"

#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "my_helper_fct.h"

static const char* TAG = "MsgQ";

/*isrCount global variable*/
int isrCount = 0;

// Push button for interrupt
static const gpio_num_t PIN_PUSH_BUTTON = 15;

/* Stack size for all tasks */
const uint32_t TASK_STACK_SIZE = 4096;

/*Queue global declaration*/
QueueHandle_t xQueue;

void vCounterTask(void *pvParameters) ;
static void IRAM_ATTR Push_button_isr_handler(void *args);

void app_main(void) {

	/* Config GPIO */
	gpio_config_t config_in ;
	config_in . intr_type = GPIO_INTR_NEGEDGE ; // falling edge interrupt
	config_in . mode = GPIO_MODE_INPUT ; // Input mode ( push button )
	config_in . pull_down_en = false ;
	config_in . pull_up_en = true ; // Pull -up resistor
	config_in . pin_bit_mask = (1ULL << PIN_PUSH_BUTTON ) ; // Pin number
	gpio_config (& config_in ) ;


	/* Create Message Queue and Check if created */
	/*Create the Queue*/
	xQueue = xQueueCreate(5, sizeof(uint32_t));
	/*if(!xQueue){
		return;
	}*/

	/* Create vCounterTask task */
	xTaskCreatePinnedToCore(vCounterTask,	/* Pointer to the function that implements the task. */
		"Task 1",				/* Text name for the task.  This is to facilitate debugging only. */
		TASK_STACK_SIZE,				/* Stack depth  */
		(void*)"Counter Task",		/* Pass the text to be printed in as the task parameter. */
		1,				/* Task priority */
		NULL,
		CORE_0);					/* We are not using the task handle. */					

	/* Install ISR */
	gpio_install_isr_service (0) ;
	gpio_isr_handler_add ( PIN_PUSH_BUTTON , Push_button_isr_handler , ( void
	*) PIN_PUSH_BUTTON ) ;

	/* to ensure its exit is clean */
	vTaskDelete(NULL);
}

void vCounterTask(void *pvParameters) {
	uint32_t lReceivedValue;
	uint32_t nbValues;
	for (;; ) {

		/* Wait for message with 5 sec. otherwise DISPLAY a message to push it */
		/* If pushed */
		if ( xQueueReceive (xQueue, &lReceivedValue, pdMS_TO_TICKS(5000)) ) {
			// DISPLAY "Button pushed" and the isrCount variable
			DISPLAY("Button pushed. isrCount value : %d", isrCount);
			// Get the number of items in the queue
			nbValues = uxQueueMessagesWaiting(xQueue);
			// DISPLAYI (Information display) number of items if greater than 1
			if(nbValues > 1){
				DISPLAYI(TAG, "Nobre d'items superieur a 1 : %d", nbValues);
				// Waiting for push button signal is 1 again (test every 20 ms)
				while(gpio_get_level(15) == 0){
				vTaskDelay ( pdMS_TO_TICKS (20) ) ;
			}
			}
		
			
			// DISPLAY "Button released"
			DISPLAY("Button released");
		}
		/* Else If Time out */
		else{
			// Display message "Please, push the button!"
			DISPLAYE( TAG, "Please push the button !");
		}
	}
}


static void IRAM_ATTR Push_button_isr_handler(void *args) {

	// Increment isrCount
	isrCount++;
	// Send message
	xQueueSendFromISR(xQueue, &isrCount, 0);
}
