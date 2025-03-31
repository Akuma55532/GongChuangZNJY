//
// Created by Wangxuan on 2024/12/14.
//
#include "main.h"
#include "usart.h"
#include "show.h"
#include "anotc.h"
#include "System.h"
#include "Raspi.h"

#define Data_FREQUENCY 4

TaskHandle_t TaskDataHandler;

void Task_Data_function(void * arg)
{
    (void)arg; // 明确表示参数未被使用
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1)
    {
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000/Data_FREQUENCY)); // 50ms唤醒一次
//        if(Flag_Stop == 0) {
            Data_transform();
            USART1_SEND();
//        }
    }
}