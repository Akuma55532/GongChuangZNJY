//
// Created by Wangxuan on 2024/12/14.
//

#ifndef GONGXUN12_13_PS2_H
#define GONGXUN12_13_PS2_H

#include "FreeRTOS.h"
#include "task.h"

extern TaskHandle_t TaskPs2Handler;

void Task_Ps2_function(void * arg);

#endif //GONGXUN12_13_PS2_H
