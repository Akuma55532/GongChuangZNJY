//
// Created by Wangxuan on 2024/12/14.
//

#ifndef GONGXUN12_13_MOVE_H
#define GONGXUN12_13_MOVE_H

#include "FreeRTOS.h"
#include "task.h"

extern TaskHandle_t TaskMoveHandler;

void Task_Move_function(void * arg);
void PS2_control(void);
void stop_move(void);
void move_forward(void);
void turn_right(void);
void turn_left(void);
void MOVE_SIMPLE(void);
#endif //GONGXUN12_13_MOVE_H
