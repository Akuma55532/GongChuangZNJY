//
// Created by Wangxuan on 2024/12/14.
//
#include "main.h"
#include "usart.h"
#include "Raspi.h"
#include <string.h>
#include "System.h"

uint8_t recv_flag = 0;

/**************************************************************************
函数功能：串口初始化
入口参数：无
返回  值：无
**************************************************************************/
void USART_Init(void)
{
    HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRx1Buffer, 1);
    HAL_UART_Receive_IT(&huart3, (uint8_t *)&aRx3Buffer, 1);
}

/**************************************************************************
函数功能：串口中断回调函数
入口参数：无
返回  值：无
**************************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    static uint8_t usart1_recv_flag,usart3_recv_flag;
    if(huart->Instance == USART1 && PS2_ON_Flag == 0) {
        if(aRx1Buffer == FRAME_HEADER)
        {
            usart1_recv_flag = 1;
        }
        if(usart1_recv_flag)
        {
            Rx1Buffer[Uart1_Rx_Cnt++] = aRx1Buffer;
            if ((Rx1Buffer[Uart1_Rx_Cnt - 1] == FRAME_TAIL) && (Uart1_Rx_Cnt == RECEIVE_DATA_SIZE)) //判断结束位
            {
                Uart1_Rx_Cnt = usart1_recv_flag = 0;
                memcpy(Receive_Data.buffer, Rx1Buffer, sizeof(Receive_Data.buffer));
                memset(Rx1Buffer, 0x00, sizeof(Rx1Buffer)); //清空数组
                Recv_Process();
            }
        }
        else
        {
            Uart1_Rx_Cnt = 0;
            memset(Rx1Buffer, 0x00, sizeof(Rx1Buffer));
        }
        HAL_UART_Receive_IT(&huart1, (uint8_t *) &aRx1Buffer, 1);   //再开启接收中断
    }
    else if (huart->Instance == USART3) {
        if(aRx3Buffer == FRAME_HEADER)
        {
            usart3_recv_flag = 1;
        }
        if(usart3_recv_flag)
        {
            Rx3Buffer[Uart3_Rx_Cnt++] = aRx3Buffer;
            if ((Rx3Buffer[Uart3_Rx_Cnt - 1] == FRAME_TAIL) && (Uart3_Rx_Cnt == RECEIVE_DATA_SIZE)) //判断结束位
            {
                Uart3_Rx_Cnt = usart3_recv_flag = 0;
                memcpy(Receive_Data.buffer, Rx3Buffer, sizeof(Receive_Data.buffer));
                memset(Rx3Buffer, 0x00, sizeof(Rx3Buffer)); //清空数组
            }
        }
        else
        {
            Uart3_Rx_Cnt = 0;
            memset(Rx3Buffer, 0x00, sizeof(Rx3Buffer));
        }
        HAL_UART_Receive_IT(&huart3, (uint8_t *) &aRx3Buffer, 1);   //再开启接收中断
    }
}

/**************************************************************************
函数功能：串口1发送数据
入口参数：无
返回  值：无
**************************************************************************/
void USART1_SEND(void)
{
    HAL_UART_Transmit(&huart1, Send_Data.buffer, SEND_DATA_SIZE, 50);
}

/**************************************************************************
函数功能：串口3发送数据
入口参数：无
返回  值：无
**************************************************************************/
void USART3_SEND(void)
{
    HAL_UART_Transmit(&huart3, Send_Data.buffer, SEND_DATA_SIZE, 50);
}


/**************************************************************************
函数功能：将预发送数据填充到发送数组中
入口参数：无
返回  值：无
**************************************************************************/
void Data_transform(void)
{
    Send_Data.Sensor_Str.Frame_Header = FRAME_HEADER; //Frame_header //帧头
    Send_Data.Sensor_Str.Frame_Tail = FRAME_TAIL;     //Frame_tail //帧尾

    Send_Data.Sensor_Str.Flag_Stop = Flag_Stop;
    Send_Data.Sensor_Str.Is_Stoping = IsStoping;

    Send_Data.buffer[0] = Send_Data.Sensor_Str.Frame_Header;
    Send_Data.buffer[1] = Send_Data.Sensor_Str.Flag_Stop;
    Send_Data.buffer[2] = Send_Data.Sensor_Str.Is_Stoping;
    Send_Data.buffer[3] = Check_Sum(3,1);
    Send_Data.buffer[4] = Send_Data.Sensor_Str.Frame_Tail;
}


//void Data_transform_simple(void)
//{
//    Send_Data.Sensor_Str.Frame_Header = FRAME_HEADER; //Frame_header //帧头
//    Send_Data.Sensor_Str.Frame_Tail = FRAME_TAIL;     //Frame_tail //帧尾
//    Send_Data.Sensor_Str.Power_Voltage = Volt;
//
//    Send_Data.buffer[0]=Send_Data.Sensor_Str.Frame_Header; //Frame_heade //帧头
//
//    //The three-axis speed of / / car is split into two eight digit Numbers
//    //小车三轴速度,各轴都拆分为两个8位数据再发送
//    Send_Data.buffer[1]=MOVE_SINGLE;
//
//    Send_Data.buffer[2]=SCRATCH_SINGLE;
//
//    Send_Data.buffer[3]=Send_Data.Sensor_Str.Power_Voltage >>8;
//    Send_Data.buffer[4]=Send_Data.Sensor_Str.Power_Voltage;
//
//    //Data check digit calculation, Pattern 1 is a data check
//    //数据校验位计算，模式1是发送数据校验
//    Send_Data.buffer[5]=Check_Sum(5,1);
//
//    Send_Data.buffer[6]=Send_Data.Sensor_Str.Frame_Tail; //Frame_tail //帧尾
//}

/**************************************************************************
函数功能：将预发送数据填充到发送数组中(测试树莓派和stm32通信版本)
入口参数：无
返回  值：无
**************************************************************************/
//void Data_transform_test(void)
//{
//    Send_Data.Sensor_Str.Frame_Header = FRAME_HEADER; //Frame_header //帧头
//    Send_Data.Sensor_Str.Frame_Tail = FRAME_TAIL;     //Frame_tail //帧尾
//    Send_Data.Sensor_Str.X_speed = 1200;
//    Send_Data.Sensor_Str.Y_speed = 1300;
//    Send_Data.Sensor_Str.Z_speed = 2300;
//    Send_Data.Sensor_Str.Power_Voltage = Volt;
//
//    Send_Data.buffer[0]=Send_Data.Sensor_Str.Frame_Header; //Frame_heade //帧头
//    Send_Data.buffer[1]=Flag_Stop; //Car software loss marker //小车软件失能标志位
//
//    //The three-axis speed of / / car is split into two eight digit Numbers
//    //小车三轴速度,各轴都拆分为两个8位数据再发送
//    Send_Data.buffer[2]=Send_Data.Sensor_Str.X_speed >> 8;
//    Send_Data.buffer[3]=Send_Data.Sensor_Str.X_speed;
//    Send_Data.buffer[4]=Send_Data.Sensor_Str.Y_speed >> 8;
//    Send_Data.buffer[5]=Send_Data.Sensor_Str.Y_speed;
//    Send_Data.buffer[6]=Send_Data.Sensor_Str.Z_speed >> 8;
//    Send_Data.buffer[7]=Send_Data.Sensor_Str.Z_speed;
//
//    Send_Data.buffer[8]=Send_Data.Sensor_Str.Power_Voltage >>8;
//    Send_Data.buffer[9]=Send_Data.Sensor_Str.Power_Voltage;
//
//    //Data check digit calculation, Pattern 1 is a data check
//    //数据校验位计算，模式1是发送数据校验
//    Send_Data.buffer[10]=Check_Sum(10,1);
//
//    Send_Data.buffer[11]=Send_Data.Sensor_Str.Frame_Tail; //Frame_tail //帧尾
//}


/**************************************************************************
函数功能：计算要发送/接收的数据校验结果
入口参数：Count_Number：校验的前几位数；Mode：0-对接收数据进行校验，1-对发送数据进行校验
返回  值：校验结果
**************************************************************************/
uint8_t Check_Sum(unsigned char Count_Number,unsigned char Mode)
{
    unsigned char check_sum=0,k;

    //Validate the data to be sent
    //对要发送的数据进行校验
    if(Mode==1)
        for(k=0;k<Count_Number;k++)
        {
            check_sum=check_sum^Send_Data.buffer[k];
        }

    //Verify the data received
    //对接收到的数据进行校验
    if(Mode==0)
        for(k=0;k<Count_Number;k++)
        {
            check_sum=check_sum^Receive_Data.buffer[k];
        }
    return check_sum;
}

void Recv_Process(void)
{
    short move_x,move_y,move_z;

    if(Receive_Data.buffer[0]==FRAME_HEADER && Receive_Data.buffer[11]==FRAME_TAIL && PS2_ON_Flag == 0)
    {
        if(Check_Sum(10,0) == Receive_Data.buffer[10])
        {
            move_x = (Receive_Data.buffer[1]<<8)+Receive_Data.buffer[2];
            move_y = (Receive_Data.buffer[3]<<8)+Receive_Data.buffer[4];
            move_z = (Receive_Data.buffer[5]<<8)+Receive_Data.buffer[6];

            SCRATCH_SINGLE = Receive_Data.buffer[7];

            TURN_MODE = Receive_Data.buffer[8];

            X_REVERSAL_SINGLE = Receive_Data.buffer[9];


            Move_X = move_x / 1000 + move_x % 1000 * 0.001;
            Move_Y = move_y / 1000 + move_y % 1000 * 0.001;
            Move_Z = move_z / 1000 + move_z % 1000 * 0.001;

            if(X_REVERSAL_SINGLE)
            {
                Move_X = -Move_X;
            }
        }
    }
}

//void Recv_Process_simple(void)
//{
//    if(Receive_Data.buffer[0]==FRAME_HEADER && Receive_Data.buffer[4]==FRAME_TAIL)
//    {
//        if(Check_Sum(3,0) == Receive_Data.buffer[3])
//        {
//            MOVE_SINGLE = Receive_Data.buffer[1];
//            SCRATCH_SINGLE = Receive_Data.buffer[2];
//        }
//    }
//}