#include "./serial.h"

float MOVE_X,MOVE_Y,MOVE_Z;
uint8_t SCRATCH_FLAG;
uint8_t TURN_MODE;
uint8_t X_REVERSAL_SINGLE;
uint8_t IsStoping = 0;
uint8_t Flag_Stop = 1;
/**************************************************************************
函数功能：串口类构造函数
入口参数：None
返回  值：None
**************************************************************************/
SerialClass::SerialClass(void)
{
    // 文件描述符的初始化
    serial_fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_SYNC);
    // 获取当前串口相关配置参数
    if(tcgetattr(serial_fd, &serial_config) != 0) {
        perror("Error from tcgetattr");
        close(serial_fd);
    }
    // 修改部分参数
    cfsetospeed(&serial_config, SERIAL_BAUD); // 波特率
    cfsetispeed(&serial_config, SERIAL_BAUD);

    serial_config.c_cflag &= ~PARENB; // 无校验位
    serial_config.c_cflag &= ~CSTOPB; // 1位停止位
    serial_config.c_cflag &= ~CSIZE;  // 清除数据位设置
    serial_config.c_cflag |= CS8;     // 8位数据位

    serial_config.c_cflag &= ~CRTSCTS;       // 禁用 RTS/CTS 硬件流控制
    serial_config.c_cc[VMIN]  = 0;           // 设置读取数据时的最小字符数为 0。
    serial_config.c_cc[VTIME] = 10;          // 设置超时等待时间为 1 秒（10 个 1/10 秒的时间单位，即 1 秒）

    cfmakeraw(&serial_config);               // 原始模式

    // 清空串口缓冲区
    tcflush(serial_fd, TCIFLUSH);
    // 应用串口配置
    if (tcsetattr(serial_fd, TCSANOW, &serial_config) != 0) {
        perror("Error from tcsetattr");
        close(serial_fd);
    }
}

/**************************************************************************
函数功能：串口类析构函数
入口参数：None
返回  值：None
**************************************************************************/
SerialClass::~SerialClass()
{
    close(serial_fd);
}

/**************************************************************************
函数功能：串口接收函数
入口参数：None
返回  值：接收数组
**************************************************************************/
unsigned char * SerialClass::Serial_Read(void)
{
    // 清空readfds
    FD_ZERO(&this->readfds);
    // 将串口文件描述符写入readfds
    FD_SET(this->serial_fd, &this->readfds);
    // 设置select最长等待时间位1s
    this->timeout.tv_sec = 1;
    this->timeout.tv_usec = 0;
    // 使用select监听标记的文件描述符
    int result = select(this->serial_fd + 1, &this->readfds, NULL, NULL, &this->timeout);
    if (result == -1) perror("select failed");

    if (FD_ISSET(this->serial_fd, &this->readfds)){
        this->buffer_bytes = read(this->serial_fd, this->recv_buf, sizeof(this->recv_buf));
    }
    if (this->buffer_bytes == RECEIVE_DATA_SIZE)
    {
        return this->recv_buf;
    }
    return nullptr;
}

/**************************************************************************
函数功能：串口接收处理函数
入口参数：buf:串口接收数组
返回  值：None
**************************************************************************/
void SerialClass::Serial_Process(unsigned char * buf)
{
    if (buf[0] == 0x7B && buf[4] == 0x7D)
    {
        // 将数据复制到Receive_Data.buffer
        std::memcpy(this->Receive_Data.buffer, buf, RECEIVE_DATA_SIZE);

        if(this->Receive_Data.buffer[3] == this->Check_Sum(3,0))
        {

            this->Receive_Data.Sensor_Str.Flag_Stop = this->Receive_Data.buffer[1];
            this->Receive_Data.Sensor_Str.Is_Stoping = this->Receive_Data.buffer[2];

            Flag_Stop = this->Receive_Data.Sensor_Str.Flag_Stop;
            IsStoping = this->Receive_Data.Sensor_Str.Is_Stoping;
            
            // 输出结果
            std::cout << "Is_Stoping: " << (int)(this->Receive_Data.Sensor_Str.Is_Stoping)
            << "    " << "Flag_Stop: " << (int)(this->Receive_Data.Sensor_Str.Flag_Stop) << std::endl;
        }
    }
    else {
        std::cerr << "Invalid frame: Incorrect start or end byte." << std::endl;
        std::cout << "buf" << this->Receive_Data.buffer << std::endl;
    }
}

// void SerialClass::Serial_Process_Simple(unsigned char * buf)
// {
//     if (buf[0] == 0x7B && buf[6] == 0x7D)
//     {
//         // 将数据复制到Receive_Data.buffer
//         std::memcpy(this->Receive_Data.buffer, buf, RECEIVE_DATA_SIZE);

//         // 提取帧头和帧尾
//         this->Receive_Data.Sensor_Str.Frame_Header = this->Receive_Data.buffer[0];
//         this->Receive_Data.Sensor_Str.Frame_Tail = this->Receive_Data.buffer[6];

//         if(this->Receive_Data.buffer[5] == this->Check_Sum(5,0))
//         {
//             // 计算X_speed, Y_speed, Z_speed和Power_Voltage
//             // 使用浮点型计算，确保精度
//             this->Receive_Data.Sensor_Str.move_Flag = (int)this->Receive_Data.buffer[1];
//             this->Receive_Data.Sensor_Str.scratch_Flag = (int)this->Receive_Data.buffer[2];

//             this->Receive_Data.Sensor_Str.Power_Voltage = (float)((this->Receive_Data.buffer[3] << 8) + this->Receive_Data.buffer[4]) / 100.0f;
            

//             // 输出结果
//             std::cout << "MOVE_SINGLE: " << Receive_Data.Sensor_Str.move_Flag 
//             << "    " << "SCRATCH_SINGLE: " << Receive_Data.Sensor_Str.scratch_Flag
//             << "    " << "Power_Voltage: " << Receive_Data.Sensor_Str.Power_Voltage << std::endl;

//         }
//         else {
//             std::cerr << "Invalid frame: Incorrect check sum." << std::endl;
//         }
//     }
//     else {
//         std::cerr << "Invalid frame: Incorrect start or end byte." << std::endl;
//     }
// }

/**************************************************************************
函数功能：串口发送数据传递
入口参数：None
返回  值：None
**************************************************************************/
void SerialClass::data_transition(void)
{
    Send_Data.Control_Str.Frame_Header = FRAME_HEADER; //Frame_header //帧头
    Send_Data.Control_Str.Frame_Tail = FRAME_TAIL;     //Frame_tail //帧尾
    Send_Data.Control_Str.X_speed = MOVE_X * 1000;
    Send_Data.Control_Str.Y_speed = MOVE_Y * 1000;
    Send_Data.Control_Str.Z_speed = MOVE_Z * 1000;
    Send_Data.Control_Str.scratch_Flag = SCRATCH_FLAG;
    Send_Data.Control_Str.Turn_Mode = TURN_MODE;
    Send_Data.Control_Str.x_reversal_single = X_REVERSAL_SINGLE;

    Send_Data.buffer[0]=Send_Data.Control_Str.Frame_Header; //Frame_heade //帧头

    //The three-axis speed of / / car is split into two eight digit Numbers
    //小车三轴速度,各轴都拆分为两个8位数据再发送
    Send_Data.buffer[1]=Send_Data.Control_Str.X_speed >> 8;
    Send_Data.buffer[2]=Send_Data.Control_Str.X_speed;
    Send_Data.buffer[3]=Send_Data.Control_Str.Y_speed >> 8;
    Send_Data.buffer[4]=Send_Data.Control_Str.Y_speed;
    Send_Data.buffer[5]=Send_Data.Control_Str.Z_speed >> 8;
    Send_Data.buffer[6]=Send_Data.Control_Str.Z_speed;

    //小车抓取标志位
    Send_Data.buffer[7]=Send_Data.Control_Str.scratch_Flag;

    Send_Data.buffer[8]=Send_Data.Control_Str.Turn_Mode;

    Send_Data.buffer[9]=Send_Data.Control_Str.x_reversal_single;

    //Data check digit calculation, Pattern 1 is a data check
    //数据校验位计算，模式1是发送数据校验
    Send_Data.buffer[10]=Check_Sum(10,1);

    Send_Data.buffer[11]=Send_Data.Control_Str.Frame_Tail; //Frame_tail //帧尾
}

// void SerialClass::data_transition_simple(void)
// {
//     Send_Data.Control_Str.move_Flag = MOVE_FLAG;
//     Send_Data.Control_Str.scratch_Flag = SCRATCH_FLAG;
//     Send_Data.Control_Str.Frame_Header = FRAME_HEADER; //Frame_header //帧头
//     Send_Data.Control_Str.Frame_Tail = FRAME_TAIL;     //Frame_tail //帧尾

//     Send_Data.buffer[0]=Send_Data.Control_Str.Frame_Header; //Frame_heade //帧头

//     Send_Data.buffer[1]=Send_Data.Control_Str.move_Flag;
//     //小车抓取标志位
//     Send_Data.buffer[2]=Send_Data.Control_Str.scratch_Flag;

//     //Data check digit calculation, Pattern 1 is a data check
//     //数据校验位计算，模式1是发送数据校验
//     Send_Data.buffer[3]=Check_Sum(3,1);

//     Send_Data.buffer[4]=Send_Data.Control_Str.Frame_Tail; //Frame_tail //帧尾
// }

void printByteArray(const unsigned char* buffer, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        std::cout << "Byte " << i << ": " << static_cast<int>(buffer[i]) << std::endl;
    }
}

/**************************************************************************
函数功能：串口发送数据
入口参数：None
返回  值：None
**************************************************************************/
unsigned char SerialClass::Serial_Write(void)
{
    // 清空readfds
    FD_ZERO(&this->writefds);
    // 将串口文件描述符写入readfds
    FD_SET(this->serial_fd, &this->writefds);
    // 设置select最长等待时间位1s
    this->timeout.tv_sec = 0;
    this->timeout.tv_usec = 100000;
    // 使用select监听标记的文件描述符
    int result = select(this->serial_fd + 1, NULL, &this->writefds, NULL, &this->timeout);
    if (result == -1) perror("select failed");

    if (FD_ISSET(this->serial_fd, &this->writefds)){
        this->write_bytes = write(this->serial_fd, this->Send_Data.buffer, sizeof(this->Send_Data.buffer));
    }
    if (this->write_bytes == SEND_DATA_SIZE)
    {
        return 1;
    }
    perror("write failed");
    return -1;
}

/**************************************************************************
函数功能：计算要发送/接收的数据校验结果
入口参数：Count_Number：校验的前几位数；Mode：0-对接收数据进行校验，1-对发送数据进行校验
返回  值：校验结果
**************************************************************************/
unsigned char SerialClass::Check_Sum(unsigned char Count_Number,unsigned char Mode)
{
    unsigned char check_sum=0,k;

    //Validate the data to be sent
    //对要发送的数据进行校验
    if(Mode==1)
        for(k=0;k<Count_Number;k++)
        {
            check_sum=check_sum^this->Send_Data.buffer[k];
        }

    //Verify the data received
    //对接收到的数据进行校验
    if(Mode==0)
        for(k=0;k<Count_Number;k++)
        {
            check_sum=check_sum^this->Receive_Data.buffer[k];
        }
    return check_sum;
}