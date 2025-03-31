//
// Created by Wangxuan on 2024/12/13.
//
#include "main.h"
#include "Delay.h"



__IO float usDelayBase;

/**************************************************************************
函数功能：微秒延时初始化函数，在使用微秒前务必初始化
入口参数：无
返回  值：无
**************************************************************************/
void usDelayInit(void)
{
    usDelayTest();
    usDelayOptimize();
}

/**************************************************************************
函数功能：测试运行环境，获得微秒延时因子usDelayBase
入口参数：无
返回  值：无
**************************************************************************/
void usDelayTest(void)
{
    __IO uint32_t firstms, secondms;
    __IO uint32_t counter = 0;

    firstms = HAL_GetTick()+1;
    secondms = firstms+1;

    while(uwTick!=firstms) ;

    while(uwTick!=secondms) counter++;

    usDelayBase = ((float)counter)/1000;
}
/**************************************************************************
函数功能：延时函数，单位为微秒，适合长时间延时，延时时间最好不大于1000us
入口参数：微秒值
返回  值：无
**************************************************************************/
void Delay_us_t(uint32_t Delay)
{
    __IO uint32_t delayReg;
    __IO uint32_t usNum = (uint32_t)(Delay*usDelayBase);

    delayReg = 0;
    while(delayReg!=usNum) delayReg++;
}
/**************************************************************************
函数功能：以HAL_Delay为基准，优化微秒延时精度
入口参数：无
返回  值：无
**************************************************************************/
void usDelayOptimize(void)
{
    __IO uint32_t firstms, secondms;
    __IO float coe = 1.0;

    firstms = HAL_GetTick();
    Delay_us_t(1000000) ;
    secondms = HAL_GetTick();

    coe = ((float)1000)/(secondms-firstms);
    usDelayBase = coe*usDelayBase;
}
/**************************************************************************
函数功能：适合长时间延时，延时时间可大于1000us
入口参数：微秒值
返回  值：无
**************************************************************************/
void Delay_us(uint32_t Delay)
{
    __IO uint32_t delayReg;

    __IO uint32_t msNum = Delay/1000;
    __IO uint32_t usNum = (uint32_t)((Delay%1000)*usDelayBase);

    if(msNum>0) HAL_Delay(msNum);

    delayReg = 0;
    while(delayReg!=usNum) delayReg++;
}