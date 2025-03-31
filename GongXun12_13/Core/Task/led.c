//
// Created by Wangxuan on 2024/12/14.
//
#include "led.h"
#include "main.h"
#include "System.h"
#include "adc.h"
#include "anotc.h"
#include "Raspi.h"

#define LED_FREQUENCY 4

TaskHandle_t TaskLedHandler;

uint16_t ADC_Value;

void Task_Led_function(void * arg)
{
    (void)arg; // 明确表示参数未被使用
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1)
    {
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000/LED_FREQUENCY));
        if(Flag_Stop == 0) {
            HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        }
        if(HAL_GPIO_ReadPin(USER_KEY_GPIO_Port,USER_KEY_Pin) == GPIO_PIN_RESET)
        {
            Flag_Stop = 0;
        }
        HAL_ADC_Start(&hadc1);     //启动ADC转换
        HAL_ADC_PollForConversion(&hadc1, 50);   //等待转换完成，50为最大等待时间，单位为ms

        if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC)) {
            ADC_Value = HAL_ADC_GetValue(&hadc1);   //获取AD值
            Volt = ADC_Value * 0.92;
        }
    }
}
