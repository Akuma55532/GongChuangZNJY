//
// Created by Wangxuan on 2025/3/11.
//
#include "servo.h"
#include "tim.h"
#include "System.h"

void Servo_init(void)
{
    HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_2);
    __HAL_TIM_SET_COMPARE(&htim8,TIM_CHANNEL_1,700);
    __HAL_TIM_SET_COMPARE(&htim8,TIM_CHANNEL_2,2400);
}

void Servo_up(void)
{
    __HAL_TIM_SET_COMPARE(&htim8,TIM_CHANNEL_1,2500);
    __HAL_TIM_SET_COMPARE(&htim8,TIM_CHANNEL_2,600);
}

void Servo_down(void)
{
    __HAL_TIM_SET_COMPARE(&htim8,TIM_CHANNEL_1,700);
    __HAL_TIM_SET_COMPARE(&htim8,TIM_CHANNEL_2,2400);
}