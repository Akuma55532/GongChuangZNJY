//
// Created by Wangxuan on 2024/12/13.
//

#ifndef GONGXUN12_13_PSTWO_H
#define GONGXUN12_13_PSTWO_H


typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;


//#define DI   PEin(15)     //Input pin //输入引脚
#define DI   HAL_GPIO_ReadPin(PS2IN_GPIO_Port,PS2IN_Pin)     //Input pin //输入引脚

//#define DO_H PEout(12)=1   //Command height //命令位高
//#define DO_L PEout(12)=0   //Command low //命令位低
#define DO_H HAL_GPIO_WritePin(PS2OUT3_GPIO_Port,PS2OUT3_Pin,GPIO_PIN_SET)   //Command height //命令位高
#define DO_L HAL_GPIO_WritePin(PS2OUT3_GPIO_Port,PS2OUT3_Pin,GPIO_PIN_RESET)   //Command low //命令位低

//#define CS_H PEout(10)=1  //Cs pull up //CS拉高
//#define CS_L PEout(10)=0  //Cs drawdown //CS拉低
#define CS_H HAL_GPIO_WritePin(PS2OUT2_GPIO_Port,PS2OUT2_Pin,GPIO_PIN_SET)  //Cs pull up //CS拉高
#define CS_L HAL_GPIO_WritePin(PS2OUT2_GPIO_Port,PS2OUT2_Pin,GPIO_PIN_RESET)  //Cs drawdown //CS拉低

//#define CLK_H PEout(8)=1 //Clock lift //时钟拉高
//#define CLK_L PEout(8)=0 //Clock down //时钟拉低
#define CLK_H HAL_GPIO_WritePin(PS2OUT1_GPIO_Port,PS2OUT1_Pin,GPIO_PIN_SET) //Clock lift //时钟拉高
#define CLK_L HAL_GPIO_WritePin(PS2OUT1_GPIO_Port,PS2OUT1_Pin,GPIO_PIN_RESET) //Clock down //时钟拉低


void PS2_Read(void);
void PS2_Init(void);
u8 PS2_RedLight(void);
void PS2_ReadData(void);
void PS2_Cmd(u8 CMD);
u8 PS2_DataKey(void);
u8 PS2_AnologData(u8 button);
void PS2_ClearData(void);
void PS2_Vibration(u8 motor1, u8 motor2);

void PS2_EnterConfing(void);
void PS2_TurnOnAnalogMode(void);
void PS2_VibrationMode(void);
void PS2_ExitConfing(void);
void PS2_SetInit(void);
void PS2_Receive (void);
void pstwo_task(void *pvParameters);

void PY_usDelayTest(void);
void PY_Delay_us_t(uint32_t Delay);
void PY_usDelayOptimize(void);
void PY_Delay_us(uint32_t Delay);

#endif //GONGXUN12_13_PSTWO_H
