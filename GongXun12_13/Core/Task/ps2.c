//
// Created by Wangxuan on 2024/12/14.
//
#include "main.h"
#include "ps2.h"
#include "pstwo.h"
#include "System.h"


#define PS2_FREQUENCY 50

TaskHandle_t TaskPs2Handler;

void Task_Ps2_function(void * arg)
{
    (void)arg; // 明确表示参数未被使用
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1)
    {
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000/PS2_FREQUENCY)); // 10ms唤醒一次
        if(Flag_Stop == 0){
            PS2_Read();
        }
    }
}