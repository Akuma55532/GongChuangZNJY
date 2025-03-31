//
// Created by Wangxuan on 2024/12/13.
//

#include "main.h"
#include "anotc.h"
#include "usart.h"

#define BYTE0(dwTemp) (*(char *)(&dwTemp))
#define BYTE1(dwTemp) (*((char *)(&dwTemp)+1))
#define BYTE2(dwTemp) (*((char *)(&dwTemp)+2))
#define BYTE3(dwTemp) (*((char *)(&dwTemp)+3))

uint8_t BUFF[100];
uint16_t flen;

/**************************************************************************
函数功能：发送4个uint16_t
入口参数：4个uint16_t
返回  值：无
**************************************************************************/
void send_4uint8(uint16_t A,uint16_t B,uint16_t C,uint16_t D)
{
    uint8_t _cnt = 0;
    BUFF[_cnt++]=0xAB;//帧头
    BUFF[_cnt++]=0xFF;//源地址
    BUFF[_cnt++]=0xFF;//目标地址
    BUFF[_cnt++]=0xF1;//功能码（ID）
    BUFF[_cnt++]=4;//数据长度
    BUFF[_cnt++]=0;//数据长度
    BUFF[_cnt++]=BYTE0(A);

    BUFF[_cnt++]=BYTE0(B);

    BUFF[_cnt++]=BYTE0(C);

    BUFF[_cnt++]=BYTE0(D);
    uint8_t sumcheck = 0;
    uint8_t addcheck = 0;
    uint16_t flen = BUFF[4] + BUFF[5] * 255;
    for(uint16_t i=0; i < (flen+6); i++)//校验计算
    {
        sumcheck += BUFF[i];
        addcheck += sumcheck;
    }

    BUFF[_cnt++] = sumcheck;//和校验
    BUFF[_cnt++] = addcheck;//附加校验
    HAL_UART_Transmit(&huart1,BUFF,_cnt,0xffff);//串口发送
}
/**************************************************************************
函数功能：发送4个uint16_t
入口参数：4个uint16_t
返回  值：无
**************************************************************************/
void send_4uint16(uint16_t A,uint16_t B,uint16_t C,uint16_t D)
{
    uint8_t _cnt = 0;
    BUFF[_cnt++]=0xAB;//帧头
    BUFF[_cnt++]=0xFF;//源地址
    BUFF[_cnt++]=0xFF;//目标地址
    BUFF[_cnt++]=0xF2;//功能码（ID）
    BUFF[_cnt++]=8;//数据长度
    BUFF[_cnt++]=0;//数据长度
    BUFF[_cnt++]=BYTE0(A);
    BUFF[_cnt++]=BYTE1(A);

    BUFF[_cnt++]=BYTE0(B);
    BUFF[_cnt++]=BYTE1(B);

    BUFF[_cnt++]=BYTE0(C);
    BUFF[_cnt++]=BYTE1(C);

    BUFF[_cnt++]=BYTE0(D);
    BUFF[_cnt++]=BYTE1(D);

    uint8_t sumcheck = 0;
    uint8_t addcheck = 0;
    uint16_t flen = BUFF[4] + BUFF[5] * 255;
    for(uint16_t i=0; i < (flen+6); i++)//校验计算
    {
        sumcheck += BUFF[i];
        addcheck += sumcheck;
    }

    BUFF[_cnt++] = sumcheck;//和校验
    BUFF[_cnt++] = addcheck;//附加校验
    HAL_UART_Transmit(&huart1,BUFF,_cnt,0xffff);//串口发送
}
/**************************************************************************
函数功能：发送4个uint32_t
入口参数：4个uint32_t
返回  值：无
**************************************************************************/
void sent_4uint32(uint32_t A,uint32_t B,uint32_t C,uint32_t D)
{
    uint8_t _cnt = 0;
    BUFF[_cnt++]=0xAB;//帧头
    BUFF[_cnt++]=0xFF;//源地址
    BUFF[_cnt++]=0xFF;//目标地址
    BUFF[_cnt++]=0xF3;//功能码（ID）
    BUFF[_cnt++]=16;//数据长度
    BUFF[_cnt++]=0;//数据长度
    BUFF[_cnt++]=BYTE0(A);
    BUFF[_cnt++]=BYTE1(A);
    BUFF[_cnt++]=BYTE2(A);
    BUFF[_cnt++]=BYTE3(A);

    BUFF[_cnt++]=BYTE0(B);
    BUFF[_cnt++]=BYTE1(B);
    BUFF[_cnt++]=BYTE2(B);
    BUFF[_cnt++]=BYTE3(B);

    BUFF[_cnt++]=BYTE0(C);
    BUFF[_cnt++]=BYTE1(C);
    BUFF[_cnt++]=BYTE2(C);
    BUFF[_cnt++]=BYTE3(C);

    BUFF[_cnt++]=BYTE0(D);
    BUFF[_cnt++]=BYTE1(D);
    BUFF[_cnt++]=BYTE2(D);
    BUFF[_cnt++]=BYTE3(D);

    uint8_t sumcheck = 0;
    uint8_t addcheck = 0;
    uint16_t flen = BUFF[4] + BUFF[5] * 255;
    for(uint16_t i=0; i < (flen+6); i++)//校验计算
    {
        sumcheck += BUFF[i];
        addcheck += sumcheck;
    }

    BUFF[_cnt++] = sumcheck;//和校验
    BUFF[_cnt++] = addcheck;//附加校验
    HAL_UART_Transmit(&huart1,BUFF,_cnt,0xffff);//串口发送
}
/**************************************************************************
函数功能：发送4个float
入口参数：4个float
返回  值：无
**************************************************************************/
void send_4float(float A,float B,float C,float D)
{
    uint8_t _cnt = 0;
    BUFF[_cnt++]=0xAB;//帧头
    BUFF[_cnt++]=0xFF;//源地址
    BUFF[_cnt++]=0xFF;//目标地址
    BUFF[_cnt++]=0xF4;//功能码（ID）
    BUFF[_cnt++]=16;//数据长度
    BUFF[_cnt++]=0;//数据长度
    BUFF[_cnt++]=BYTE0(A);
    BUFF[_cnt++]=BYTE1(A);
    BUFF[_cnt++]=BYTE2(A);
    BUFF[_cnt++]=BYTE3(A);

    BUFF[_cnt++]=BYTE0(B);
    BUFF[_cnt++]=BYTE1(B);
    BUFF[_cnt++]=BYTE2(B);
    BUFF[_cnt++]=BYTE3(B);

    BUFF[_cnt++]=BYTE0(C);
    BUFF[_cnt++]=BYTE1(C);
    BUFF[_cnt++]=BYTE2(C);
    BUFF[_cnt++]=BYTE3(C);

    BUFF[_cnt++]=BYTE0(D);
    BUFF[_cnt++]=BYTE1(D);
    BUFF[_cnt++]=BYTE2(D);
    BUFF[_cnt++]=BYTE3(D);

    uint8_t sumcheck = 0;
    uint8_t addcheck = 0;
    uint16_t flen = BUFF[4] + BUFF[5] * 255;
    for(uint16_t i=0; i < (flen+6); i++)//校验计算
    {
        sumcheck += BUFF[i];
        addcheck += sumcheck;
    }
    BUFF[_cnt++] = sumcheck;//和校验
    BUFF[_cnt++] = addcheck;//附加校验
    HAL_UART_Transmit(&huart1,BUFF,_cnt,0xffff);//串口发送
}