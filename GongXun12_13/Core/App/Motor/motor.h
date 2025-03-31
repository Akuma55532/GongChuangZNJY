//
// Created by Wangxuan on 2024/12/13.
//

#ifndef GONGXUN12_13_MOTOR_H
#define GONGXUN12_13_MOTOR_H

void Car_Init(void);
int Read_Encoder(uint8_t TIMX);
float target_limit_float(float insert,float low,float high);
int target_limit_int(int insert,int low,int high);
int Incremental_PID_A (float Encoder,float Target);
int Incremental_PID_B (float Encoder,float Target);
void Limit_Pwm(int amplitude);
void Set_Pwm(int motor_a,int motor_b);
void Get_Velocity_Form_Encoder(void);
void Drive_Motor(float Vx,float Vy,float Vz);

#endif //GONGXUN12_13_MOTOR_H
