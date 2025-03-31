//
// Created by Wangxuan on 2024/12/14.
//

#include "main.h"
#include "move.h"
#include "motor.h"
#include "System.h"
#include "servo.h"

#define CONTROL_FREQUENCY 50

TaskHandle_t TaskMoveHandler;

void Task_Move_function(void * arg)
{
    (void)arg; // 明确表示参数未被使用
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1)
    {
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000/CONTROL_FREQUENCY)); // 20ms唤醒一次

        if(Flag_Stop == 0) {
            Get_Velocity_Form_Encoder();

            if(MOTOR_A.Encoder < 0.05 && MOTOR_A.Encoder > -0.05 && MOTOR_B.Encoder < 0.05 && MOTOR_B.Encoder > -0.05)
                IsStoping = 1;
            else
                IsStoping = 0;

            if (PS2_ON_Flag) {
                PS2_control();
            }
            else {
                if (TURN_MODE) {
                    if (TURN_MODE == 1)
                        turn_right();
                    else if (TURN_MODE == 2)
                        turn_left();
                } else {
                    Drive_Motor(Move_X, Move_Y, Move_Z);
                }
            }
            //Speed closed-loop control to calculate the PWM value of each motor,
            //PWM represents the actual wheel speed
            //速度闭环控制计算各电机PWM值，PWM代表车轮实际转速
            if(!TURN_MODE)
            {
//                if (HAL_GPIO_ReadPin(Trigger_Left_GPIO_Port,Trigger_Left_Pin) == GPIO_PIN_RESET)
//                {
//                    if(MOTOR_B.Target > 0)
//                        MOTOR_B.Target = 0;
//                    IsRightTrigger = 1;
//                }
//                else {
//                    IsRightTrigger = 0;
//                }
//                if (HAL_GPIO_ReadPin(Trigger_Right_GPIO_Port,Trigger_Right_Pin) == GPIO_PIN_RESET)
//                {
//                    if(MOTOR_A.Target > 0)
//                        MOTOR_A.Target = 0;
//                    IsLeftTrigger = 1;
//                }
//                else {
//                    IsLeftTrigger = 0;
//                }
                MOTOR_A.Motor_Pwm = Incremental_PID_A(MOTOR_A.Encoder, MOTOR_A.Target);
                MOTOR_B.Motor_Pwm = Incremental_PID_B(MOTOR_B.Encoder, MOTOR_B.Target);

                Limit_Pwm(6500);

                Set_Pwm(MOTOR_A.Motor_Pwm, MOTOR_B.Motor_Pwm);
            }
            switch(SCRATCH_SINGLE)
            {
                case 0:
                    Servo_up();
                    break;
                case 1:
                    Servo_down();
                    break;
                default:
                    break;
            }
        }
    }
}

/**************************************************************************
函数功能：对PS2手柄控制命令进行处理
入口参数：无
返回  值：无
**************************************************************************/
void PS2_control(void)
{
    int LX,RY;
    int Threshold=20; //Threshold to ignore small movements of the joystick //阈值，忽略摇杆小幅度动作

    //128 is the median.The definition of X and Y in the PS2 coordinate system is different from that in the ROS coordinate system
    //128为中值。PS2坐标系与ROS坐标系对X、Y的定义不一样
    LX=-(PS2_LY-128);
    RY=-(PS2_RX-128);

    //Ignore small movements of the joystick //忽略摇杆小幅度动作
    if(LX>-Threshold&&LX<Threshold)LX=0;
    if(RY>-Threshold&&RY<Threshold)RY=0;

    if (PS2_KEY==11)		RC_Velocity+=5;  //To accelerate//加速
    else if(PS2_KEY==9)	RC_Velocity-=5;  //To slow down //减速

    if (PS2_KEY==12) SCRATCH_SINGLE = 0;
    else if(PS2_KEY==10) SCRATCH_SINGLE = 1;

    if(RC_Velocity<0)   RC_Velocity=0;

    //Handle PS2 controller control commands
    //对PS2手柄控制命令进行处理
    Move_X=LX*RC_Velocity/128;
    Move_Z=RY*(PI/2)/128;

    //Z-axis data conversion //Z轴数据转化

    if(Move_X<0) Move_Z=-Move_Z; //The differential control principle series requires this treatment //差速控制原理系列需要此处理
    Move_Z=Move_Z*RC_Velocity/80;

    //Unit conversion, mm/s -> m/s
    //单位转换，mm/s -> m/s
    Move_X=Move_X/1000;
    Move_Z=Move_Z;

    //Control target value is obtained and kinematics analysis is performed
    //得到控制目标值，进行运动学分析
    Drive_Motor(Move_X,Move_Y,Move_Z);
}

void stop_move(void)
{
    Drive_Motor(0, 0, 0);
}
void move_forward(void)
{
    Drive_Motor(0.5, 0, 0);
}

void turn_right(void)
{
    Set_Pwm(2100, -2100);
}

void turn_left(void)
{
    Set_Pwm(-2100, 2100);
}
