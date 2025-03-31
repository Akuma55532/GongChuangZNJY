//
// Created by Wangxuan on 2024/12/13.
//

#ifndef GONGXUN12_13_SYSTEM_H
#define GONGXUN12_13_SYSTEM_H

#include "main.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

#define PI 3.14159

//Motor speed control related parameters of the structure
//电机速度控制相关参数结构体
typedef struct
{
    float Encoder;     //Read the real time speed of the motor by encoder //编码器数值，读取电机实时速度
    float Motor_Pwm;   //Motor PWM value, control the real-time speed of the motor //电机PWM数值，控制电机实时速度
    float Target;      //Control the target speed of the motor //电机目标速度值，控制电机目标速度
    float Velocity_KP; //Speed control PID parameters //速度控制PID参数
    float	Velocity_KI; //Speed control PID parameters //速度控制PID参数
}Motor_parameter;

typedef struct
{
    int16_t A;
    int16_t B;
}Encoder;

//Smoothed the speed of the three axes
//平滑处理后的三轴速度
typedef struct
{
    float VX;
    float VY;
    float VZ;
}Smooth_Control;

extern uint8_t Flag_Stop;
extern float Move_X, Move_Y, Move_Z;
extern Smooth_Control smooth_control;
extern Motor_parameter MOTOR_A, MOTOR_B;
extern float Encoder_A_pr,Encoder_B_pr;
extern float Velocity_KP, Velocity_KI,Velocity_KD;
extern Encoder OriginalEncoder; //Encoder raw data //编码器原始数据
extern float Encoder_precision;
extern float Wheel_perimeter;
extern float Wheel_spacing;
extern float Axle_spacing;
extern uint8_t TURN_MODE;
extern uint8_t SCRATCH_SINGLE;
extern uint8_t X_REVERSAL_SINGLE;

//These are our button constants
#define PSB_SELECT      1
#define PSB_L3          2
#define PSB_R3          3
#define PSB_START       4
#define PSB_PAD_UP      5
#define PSB_PAD_RIGHT   6
#define PSB_PAD_DOWN    7
#define PSB_PAD_LEFT    8
#define PSB_L2          9
#define PSB_R2          10
#define PSB_L1          11
#define PSB_R1          12
#define PSB_GREEN       13
#define PSB_RED         14
#define PSB_BLUE        15
#define PSB_PINK        16

#define PSB_TRIANGLE    13
#define PSB_CIRCLE      14
#define PSB_CROSS       15
#define PSB_SQUARE      16

#define PSS_RX 5   //Right rocker x shaft data   //右摇杆X轴数据
#define PSS_RY 6   //The left rocker y axis data //左摇杆Y轴数据
#define PSS_LX 7   //Right rocker x axis data    //右摇杆X轴数据
#define PSS_LY 8   //The left rocker y axis data //左摇杆Y轴数据

extern float PS2_LX,PS2_LY,PS2_RX,PS2_RY,PS2_KEY;
extern u8 PS2_ON_Flag;

extern u8 Data[9];
extern u8 Comd[2];
extern u16 MASK[16];
extern u16 Handkey;
extern float RC_Velocity;
void Task_Init(void);




#define FRAME_HEADER      0X7B //Frame_header //帧头
#define FRAME_TAIL        0X7D //Frame_tail   //帧尾
#define SEND_DATA_SIZE    5
#define RECEIVE_DATA_SIZE 12

/*******The structure of the serial port sending data************/
/*******串口发送数据的结构体*************************************/
typedef struct _SEND_DATA_
{
    unsigned char buffer[SEND_DATA_SIZE];
    struct _Sensor_Str_
    {
        unsigned char Frame_Header; //1个字节
        unsigned char Flag_Stop;
        unsigned char Is_Stoping;
        unsigned char Frame_Tail;   //1 bytes //1个字节
    }Sensor_Str;
}SEND_DATA;

typedef struct _RECEIVE_DATA_
{
    unsigned char buffer[RECEIVE_DATA_SIZE];
    struct _Control_Str_
    {
        unsigned char Frame_Header; //1 bytes //1个字节
        float X_speed;	            //4 bytes //4个字节
        float Y_speed;              //4 bytes //4个字节
        float Z_speed;              //4 bytes //4个字节
        unsigned char Turn_Mode;
        unsigned char Frame_Tail;   //1 bytes //1个字节
    }Control_Str;
}RECEIVE_DATA;

//最大接收字节为256
#define RXBUFFERSIZE  256

extern SEND_DATA Send_Data;
extern RECEIVE_DATA Receive_Data;

extern uint8_t aRx1Buffer;
extern uint8_t Uart1_Rx_Cnt;
extern char Rx1Buffer[RXBUFFERSIZE];

extern uint8_t aRx3Buffer;
extern uint8_t Uart3_Rx_Cnt;
extern char Rx3Buffer[RXBUFFERSIZE];

extern uint16_t Volt;

extern uint8_t IsStoping;

#endif //GONGXUN12_13_SYSTEM_H
