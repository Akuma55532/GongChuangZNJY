//
// Created by Wangxuan on 2024/12/14.
//

#ifndef GONGXUN12_13_SHOW_H
#define GONGXUN12_13_SHOW_H

#include "FreeRTOS.h"
#include "task.h"

extern TaskHandle_t TaskShowHandler;

void Task_Show_function(void * arg);

#endif //GONGXUN12_13_SHOW_H
