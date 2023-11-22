/****************************************************************************
 * Copyright (C) 2020 by Fabrice Muller                                     *
 *                                                                          *
 * This file is useful for ESP32 Design course.                             *
 *                                                                          *
 ****************************************************************************/

/**
 * @file lab3-1_main.c
 * @author Fabrice Muller
 * @date 20 Oct. 2020
 * @brief File containing the lab3-1 of Part 3.
 *
 * @see https://github.com/fmuller-pns/esp32-vscode-project-template
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include "esp_log.h"

/* FreeRTOS.org includes. */
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "my_helper_fct.h"

static const char* TAG = "SEM";

/* Application constants */
#define STACK_SIZE     4096
#define TABLE_SIZE     400

/* Task Priority */
const uint32_t TIMER_TASK_PRIORITY = 5;
const uint32_t INC_TABLE_TASK_PRIORITY = 3;
const uint32_t DEC_TABLE_TASK_PRIORITY = 4;
const uint32_t INSPEC_TABLE_TASK_PRIORITY = 4;

/* Communications */
SemaphoreHandle_t xSemInc;
SemaphoreHandle_t xSemDec;

/* Tasks */
void vTaskTimer(void *pvParameters);
void vTaskIncTable(void *pvParameters);
void vTaskDecTable(void *pvParameters);
void vTaskInspectorTable(void *pvParameters);


/* Datas */
int Table[TABLE_SIZE];

/* Main function */
void app_main(void) {

    /* Init Table */
    memset(Table, 0, TABLE_SIZE*sizeof(int));

    /* Create semaphore */
    xSemInc = xSemaphoreCreateBinary();
    xSemDec = xSemaphoreCreateBinary();

    /* Stop scheduler */
    vTaskSuspendAll();

    /* Create Tasks */
    xTaskCreatePinnedToCore(vTaskTimer, "Timer", STACK_SIZE, (void*)"Timer", TIMER_TASK_PRIORITY, NULL, CORE_0);
    xTaskCreatePinnedToCore(vTaskIncTable, "Inc", STACK_SIZE, (void*)"Inc", INC_TABLE_TASK_PRIORITY, NULL, CORE_0);
    xTaskCreatePinnedToCore(vTaskDecTable, "Dec", STACK_SIZE, (void*)"Dec", DEC_TABLE_TASK_PRIORITY, NULL, CORE_1);

    /* Continue scheduler */
    xTaskResumeAll();

    /* to ensure its exit is clean */
    vTaskDelete(NULL);
}
/*-----------------------------------------------------------*/

void vTaskTimer(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for(;;){
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(250));
        COMPUTE_IN_TIME_MS(20);
        DISPLAY("Task Timer : give sem");
        xSemaphoreGive(xSemInc);
        xSemaphoreGive(xSemDec);
    }
}

void vTaskIncTable(void *pvParameters) {
    static int ActivationNumber = 0;
    const int constNumber = (int *)pvParameters;
    for(;;){
        xSemaphoreTake(xSemInc, portMAX_DELAY);
        DISPLAY("Inc task running");
        if (ActivationNumber == 0){
            for (int i=0; i<TABLE_SIZE-1; i++){
                Table[i] = Table[i] + constNumber;
            }
            COMPUTE_IN_TIME_MS(50);
            ActivationNumber = 4;
        }
        else{
            ActivationNumber--;
        }
        DISPLAY("Inc task finished");
    }
}

void vTaskDecTable(void *pvParameters) {
    for(;;){
        xSemaphoreTake(xSemDec, portMAX_DELAY);
        DISPLAY("Dec task running");
        for(int i=0; i<TABLE_SIZE-1; i++){
            Table[i] = Table[i] - 1;
        }
        COMPUTE_IN_TIME_MS(50);
        DISPLAY("Dec task finished");
    }
}

void vTaskInspectorTable(void *pvParametres){
    for(;;){



    }

}