//
// Created by Wangxuan on 2024/12/13.
//
#include "System.h"
#include "FreeRTOS.h"
#include "task.h"
#include "data.h"
#include "led.h"
#include "move.h"
#include "ps2.h"
#include "show.h"
#include "data.h"

/************ 小车尺寸相关变量 **************************/
//编码器精度
float Encoder_precision;
//轮子周长，单位：m
float Wheel_perimeter;
//主动轮轮距，单位：m
float Wheel_spacing;
//小车前后轴的轴距，单位：m
float Axle_spacing;
/************ 小车尺寸相关变量 **************************/


/************ 小车运动控制相关变量 **************************/
//停止标志位
uint8_t Flag_Stop = 1;
//小车三轴目标运动速度，单位：m/s
float Move_X, Move_Y, Move_Z;
//速度控制PID参数
float Velocity_KP=300,Velocity_KI=300,Velocity_KD=0;
//小车两个电机的结构体
Motor_parameter MOTOR_A,MOTOR_B;
//编码器原始数据
Encoder OriginalEncoder;
//平滑处理参数
Smooth_Control smooth_control;

uint8_t SCRATCH_SINGLE;
uint8_t TURN_MODE;
uint8_t X_REVERSAL_SINGLE;
/************ 小车运动控制相关变量 **************************/


/************ 小车PS2控制相关变量 **************************/
//按键值读取，零时存储
u16 Handkey;
//开始命令。请求数据
u8 Comd[2]={0x01,0x42};
//数据存储数组
u8 Data[9]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
//PS2手柄控制相关变量
float PS2_LX,PS2_LY,PS2_RX,PS2_RY,PS2_KEY;
//PS2控制标志位
u8 PS2_ON_Flag=0;
//PS2各个按键名和按键值
u16 MASK[]={
    PSB_SELECT,
    PSB_L3,
    PSB_R3 ,
    PSB_START,
    PSB_PAD_UP,
    PSB_PAD_RIGHT,
    PSB_PAD_DOWN,
    PSB_PAD_LEFT,
    PSB_L2,
    PSB_R2,
    PSB_L1,
    PSB_R1 ,
    PSB_GREEN,
    PSB_RED,
    PSB_BLUE,
    PSB_PINK
};
//PS2遥控的默认速度
float RC_Velocity = 700;
/************ 小车PS2控制相关变量 **************************/


/************ 小车串口发送相关变量 **************************/
SEND_DATA Send_Data;
RECEIVE_DATA Receive_Data;

//串口1的接受变量
uint8_t aRx1Buffer;
//串口1的接收缓冲计数
uint8_t Uart1_Rx_Cnt = 0;
//串口1的接收数组
char Rx1Buffer[RXBUFFERSIZE];

//串口3的接受变量
uint8_t aRx3Buffer;
//串口3的接收缓冲计数
uint8_t Uart3_Rx_Cnt = 0;
//串口3的接收数组
char Rx3Buffer[RXBUFFERSIZE];
/************ 小车串口发送相关变量 **************************/


uint16_t Volt = 0;

uint8_t IsStoping = 0;

void Task_Init(void)
{
    xTaskCreate(Task_Led_function,"Task_Led",128,NULL,3,&TaskLedHandler);
    xTaskCreate(Task_Move_function,"Task_Move",512,NULL,4,&TaskMoveHandler);
    xTaskCreate(Task_Data_function,"Task_Data",256,NULL,4,&TaskDataHandler);
    xTaskCreate(Task_Show_function,"Task_Show",256,NULL,3,&TaskShowHandler);
    xTaskCreate(Task_Ps2_function,"Task_Ps2",256,NULL,4,&TaskPs2Handler);

    vTaskStartScheduler();
}