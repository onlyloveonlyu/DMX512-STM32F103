#ifndef __DMX_H
#define __DMX_H
#include "main.h"
#include "usart.h"
#include "gpio.h"

#define UCS512C


#define DMX_MODEL_NUM 5                        //IC num
#define DMX_MODEL_CHN 40                         //channel IC per UCS512=4*10 TM512=3*8
//#define DMX_UART &huart2                        //DMX UART PORT
#define DMX_UART USART2
#define DMX_TX_Pin GPIO_PIN_2			//DMX_UART TX PA2
#define DMX_TX_GPIO_Port GPIOA
#define DMX_RX_Pin GPIO_PIN_3			//DMX_UART RX PA3
#define DMX_RX_GPIO_Port GPIOA
#define DEF_R   255
#define DEF_G   0
#define DEF_B   0
#define DEF_W   0
#define DMX_TX_High  HAL_GPIO_WritePin(DMX_TX_GPIO_Port,DMX_TX_Pin,GPIO_PIN_SET)
#define DMX_TX_Low  HAL_GPIO_WritePin(DMX_TX_GPIO_Port,DMX_TX_Pin,GPIO_PIN_RESET)
#define LED1_ON  HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_RESET)
#define LED1_OFF HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_SET)
#define LED2_ON  HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET)
#define LED2_OFF HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET)
#define LED3_ON  HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,GPIO_PIN_RESET)
#define LED3_OFF HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,GPIO_PIN_SET)
#define RS485_DIR_TX  HAL_GPIO_WritePin(RS485_DIR_GPIO_Port,RS485_DIR_Pin,GPIO_PIN_SET)
#define RS485_DIR_RX  HAL_GPIO_WritePin(RS485_DIR_GPIO_Port,RS485_DIR_Pin,GPIO_PIN_RESET)
#define RS485_DIR_Pin GPIO_PIN_1
#define RS485_DIR_GPIO_Port GPIOA

void DMX_Reset();
uint8_t DMX_Transposition(uint8_t tempchar);
void clrDmxData(void);
void GPIO_Tx_Config_OUT(void);
void GPIO_Tx_Config_AF(void);
void DMX_Delay_us(uint32_t nus);
void DMX_Break();
void DMX_Send_9Data(uint8_t tempdata);
void DMX_Send_Packet(uint16_t tempnum);
void DMX_Init();
void DMX_Write_Add(uint16_t tempadd);
void DMX_Send_ADD_Data(uint8_t common,uint16_t tempdata);

void DMX_Demo_Init();
void DMX_Demo();
void DMX_Write_CMD(uint16_t tempadd);
void DMX_Send_CMD_Data(uint8_t common,uint8_t tempcmd);
void DMX_W();


#endif

