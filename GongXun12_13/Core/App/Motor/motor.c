//
// Created by Wangxuan on 2024/12/13.
//
#include "main.h"
#include "tim.h"
#include "motor.h"
#include "System.h"

#define CONTROL_FREQUENCY 50

/**************************************************************************
函数功能：初始化小车各项参数和定时器的PWM输出与编码器模式
入口参数：无
返回  值：无
**************************************************************************/
void Car_Init(void)
{
    Encoder_precision = 660.0;

    Wheel_perimeter = 0.4084;

    Wheel_spacing = 0.023;

    Axle_spacing = 0;

    HAL_TIM_PWM_Start(&htim10,TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim11,TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim9,TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim9,TIM_CHANNEL_2);
    HAL_TIM_Encoder_Start(&htim2,TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_ALL);
}

/**************************************************************************
函数功能：读取编码器计数
入口参数：定时器
返回  值：编码器数值(代表速度)
**************************************************************************/
int Read_Encoder(uint8_t TIMX)
{
    int Encoder_TIM;
    switch(TIMX)
    {
        case 2:
            Encoder_TIM = __HAL_TIM_GET_COUNTER(&htim2);
            __HAL_TIM_SET_COUNTER(&htim2,0);
            break;
        case 3:
            Encoder_TIM = __HAL_TIM_GET_COUNTER(&htim3);
            __HAL_TIM_SET_COUNTER(&htim3,0);
            break;
        default: Encoder_TIM=0;
    }
    return Encoder_TIM;
}
/**************************************************************************
函数功能：限幅函数
入口参数：幅值
返回  值：无
**************************************************************************/
float target_limit_float(float insert,float low,float high)
{
    if (insert < low)
        return low;
    else if (insert > high)
        return high;
    else
        return insert;
}
int target_limit_int(int insert,int low,int high)
{
    if (insert < low)
        return low;
    else if (insert > high)
        return high;
    else
        return insert;
}
/**************************************************************************
函数功能：限制PWM值
入口参数：幅值
返回  值：无
**************************************************************************/
void Limit_Pwm(int amplitude)
{
    MOTOR_A.Motor_Pwm=target_limit_float(MOTOR_A.Motor_Pwm,-amplitude,amplitude);
    MOTOR_B.Motor_Pwm=target_limit_float(MOTOR_B.Motor_Pwm,-amplitude,amplitude);
}

/**************************************************************************
函数功能：增量式PI控制器
入口参数：编码器测量值(实际速度)，目标速度
返回  值：电机PWM
根据增量式离散PID公式
pwm+=Kp[e（k）-e(k-1)]+Ki*e(k)+Kd[e(k)-2e(k-1)+e(k-2)]
e(k)代表本次偏差
e(k-1)代表上一次的偏差  以此类推
pwm代表增量输出
在我们的速度控制闭环系统里面，只使用PI控制
pwm+=Kp[e（k）-e(k-1)]+Ki*e(k)
**************************************************************************/
int Incremental_PID_A (float Encoder,float Target)
{
    static float Bias,Pwm,Last_bias,Last_Last_bias;
    Bias=Target-Encoder; //Calculate the deviation //计算偏差
    Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias+Velocity_KD*(Bias-2*Last_bias+Last_Last_bias);
    if(Pwm>6500)Pwm=6500;
    if(Pwm<-6500)Pwm=-6500;
    if(Pwm<100 && Pwm>-100)Pwm=0;
    Last_Last_bias=Last_bias;
    Last_bias=Bias; //Save the last deviation //保存上一次偏差
    return Pwm;
}
int Incremental_PID_B (float Encoder,float Target)
{
    static float Bias,Pwm,Last_bias,Last_Last_bias;
    Bias=Target-Encoder; //Calculate the deviation //计算偏差
    Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias+Velocity_KD*(Bias-2*Last_bias+Last_Last_bias);
    if(Pwm>6500)Pwm=6500;
    if(Pwm<-6500)Pwm=-6500;
    if(Pwm<100 && Pwm>-100)Pwm=0;
    Last_Last_bias=Last_bias;
    Last_bias=Bias; //Save the last deviation //保存上一次偏差
    return Pwm;
}
/**************************************************************************
函数功能：赋值给PWM寄存器，控制车轮转速与方向
入口参数：PWM
返回  值：无
**************************************************************************/
void Set_Pwm(int motor_a,int motor_b)
{
    int PWMA1,PWMA2,PWMB1,PWMB2;
    //Forward and reverse control of motor
    //电机正反转控制
    if(motor_a<0)			PWMA1=10000,PWMA2=10000+motor_a;
    else 	            PWMA2=10000,PWMA1=10000-motor_a;

    //Forward and reverse control of motor
    //电机正反转控制
    if(motor_b<0)			PWMB1=10000,PWMB2=10000+motor_b;
    else 	            PWMB2=10000,PWMB1=10000-motor_b;

    __HAL_TIM_SET_COMPARE(&htim10,TIM_CHANNEL_1,PWMA2);
    __HAL_TIM_SET_COMPARE(&htim11,TIM_CHANNEL_1,PWMA1);
    __HAL_TIM_SET_COMPARE(&htim9,TIM_CHANNEL_1,PWMB2);
    __HAL_TIM_SET_COMPARE(&htim9,TIM_CHANNEL_2,PWMB1);
}

/**************************************************************************
Function: Read the encoder value and calculate the wheel speed, unit m/s
Input   : none
Output  : none
函数功能：读取编码器数值并计算车轮速度，单位m/s
入口参数：无
返回  值：无
**************************************************************************/
void Get_Velocity_Form_Encoder(void)
{
    float Encoder_A_pr,Encoder_B_pr;
    OriginalEncoder.A=Read_Encoder(2);
    OriginalEncoder.B=Read_Encoder(3);

    //Decide the encoder numerical polarity according to different car models
    //根据差速小车编码器数值
    Encoder_A_pr= OriginalEncoder.A;
    Encoder_B_pr= -OriginalEncoder.B;

    //The encoder converts the raw data to wheel speed in m/s
    //编码器原始数据转换为车轮速度，单位m/s
    MOTOR_A.Encoder= -Encoder_A_pr*CONTROL_FREQUENCY*Wheel_perimeter/Encoder_precision;
    MOTOR_B.Encoder= -Encoder_B_pr*CONTROL_FREQUENCY*Wheel_perimeter/Encoder_precision;
}

/**************************************************************************
函数功能：运动学逆解，根据三轴目标速度计算各车轮目标转速
入口参数：X和Y、Z轴方向的目标运动速度
返回  值：无
**************************************************************************/
void Drive_Motor(float Vx,float Vy,float Vz)
{
    (void)Vy;
    float amplitude=1.5; //Wheel target speed limit //车轮目标速度限幅
    //Inverse kinematics //运动学逆解
    MOTOR_A.Target  = (Vx - Vz * Wheel_spacing / 2.0f); //计算出左轮的目标速度
    MOTOR_B.Target =  (Vx + Vz * Wheel_spacing / 2.0f); //计算出右轮的目标速度

    //Wheel (motor) target speed limit //车轮(电机)目标速度限幅
    MOTOR_A.Target=target_limit_float( MOTOR_A.Target,-amplitude,amplitude);
    MOTOR_B.Target=target_limit_float( MOTOR_B.Target,-amplitude,amplitude);
}