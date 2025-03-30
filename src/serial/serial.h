#ifndef SERIALSELF_HPP
#define SERIALSELF_HPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/select.h>
#include <cstring>
#include <iostream>


#define SERIAL_PORT "/dev/ttyACM0"
#define SERIAL_BAUD B115200

#define FRAME_HEADER      0X7B //Frame_header //帧头
#define FRAME_TAIL        0X7D //Frame_tail   //帧尾
#define SEND_DATA_SIZE    12
#define RECEIVE_DATA_SIZE 5

/*******The structure of the serial port sending data************/
/*******串口发送数据的结构体*************************************/
typedef struct _RECEIVE_DATA_
{
    unsigned char buffer[RECEIVE_DATA_SIZE];
    struct _Sensor_Str_
    {
        unsigned char Frame_Header; //1个字节
        unsigned char Flag_Stop;
        unsigned char Is_Stoping;
        unsigned char Frame_Tail; //1个字节
    }Sensor_Str;
}RECEIVE_DATA;

typedef struct _SEND_DATA_
{
    unsigned char buffer[SEND_DATA_SIZE];
    struct _Control_Str_
    {
        unsigned char Frame_Header; //1 bytes //1个字节
        short X_speed; //2 bytes
        short Y_speed; //2 bytes
        short Z_speed; //2 bytes
        unsigned char x_reversal_single;
        unsigned char Turn_Mode;
        unsigned char scratch_Flag; //1 bytes
        unsigned char Frame_Tail;   //1 bytes //1个字节
    }Control_Str;
}SEND_DATA;

extern float MOVE_X,MOVE_Y,MOVE_Z;
extern uint8_t SCRATCH_FLAG;
extern uint8_t TURN_MODE;
extern uint8_t X_REVERSAL_SINGLE;
extern uint8_t IsStoping;
extern uint8_t Flag_Stop;

/*******The structure of the serial port sending data************/
/*******串口发送数据的类*************************************/
class SerialClass
{
private:
    /* data */
    int serial_fd; // 串口的文件描述符
    struct termios serial_config; // 串口的termios结构体，用来配置串口相关参数

    
public:
    SerialClass(void);
    ~SerialClass();
    SEND_DATA Send_Data;
    RECEIVE_DATA Receive_Data;
    

    unsigned char recv_buf[5]; // 串口接收缓存数组
    ssize_t buffer_bytes; // 串口接收缓存数组接收字节大小
    fd_set readfds; // io多路复用的readbuf
    timeval timeout; // io多路复用select的最大等待时间
    unsigned char * Serial_Read(void); // 串口接收函数

    void Serial_Process(unsigned char * buf); // 串口处理函数

    // void Serial_Process_Simple(unsigned char * buf); // 串口简单处理函数

    unsigned char write_bytes; // 串口发送标志位
    fd_set writefds; // io多路复用的writebuf

    void data_transition(void);

    // void data_transition_simple(void);

    unsigned char Serial_Write(void);

    unsigned char Check_Sum(unsigned char Count_Number,unsigned char Mode); // 校验
    
};

#endif