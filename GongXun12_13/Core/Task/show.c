//
// Created by Wangxuan on 2024/12/14.
//
#include "main.h"
#include "show.h"
#include "System.h"
#include "oled.h"

#define Show_FREQUENCY 4

TaskHandle_t TaskShowHandler;

void Task_Show_function(void * arg)
{
    (void)arg; // 明确表示参数未被使用
    TickType_t xLastWakeTime = xTaskGetTickCount();
    OLED_Init();
    while (1)
    {
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000/Show_FREQUENCY));
        if (Flag_Stop == 0){
            OLED_ShowString(30,30,"Working");
        }
        else{
            OLED_ShowString(30,30,"Stoping");
        }
        OLED_ShowString(85,52,"V:");
        OLED_ShowNumber(100,52,Volt/100,2,12);
        OLED_ShowString(112,52,".");
        OLED_ShowNumber(120,52,Volt/10%10,1,12);
        OLED_Refresh_Gram();
    }
}