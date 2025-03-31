//
// Created by Wangxuan on 2024/12/14.
//

#ifndef GONGXUN12_13_LED_H
#define GONGXUN12_13_LED_H

#include "FreeRTOS.h"
#include "task.h"

extern TaskHandle_t TaskLedHandler;

void Task_Led_function(void * arg);

#endif //GONGXUN12_13_LED_H
