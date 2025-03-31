//
// Created by Wangxuan on 2024/12/14.
//

#ifndef GONGXUN12_13_RASPI_H
#define GONGXUN12_13_RASPI_H

extern uint8_t recv_flag;

void USART_Init(void);
void USART1_SEND(void);
//void USART3_SEND(void);
void Data_transform(void);
void Data_transform_simple(void);
void Data_transform_test(void);
uint8_t Check_Sum(unsigned char Count_Number,unsigned char Mode);
void Recv_Process(void);
void Recv_Process_simple(void);
#endif //GONGXUN12_13_RASPI_H
