//
// Created by Wangxuan on 2024/12/14.
//

#ifndef GONGXUN12_13_DATA_H
#define GONGXUN12_13_DATA_H

#include "FreeRTOS.h"
#include "task.h"

extern TaskHandle_t TaskDataHandler;

void Task_Data_function(void * arg);

#endif //GONGXUN12_13_DATA_H
