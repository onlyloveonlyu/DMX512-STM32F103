#include "DMX512.h"
//solt 24.752ms  40HZ refresh
//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
uint16_t DMX_model; //IC number
uint8_t DMX_Chn;    //channel number 
//static uint16_t DMX_model;
//static uint8_t DMX_Chn;
static unsigned char dmxData[513];// Data pool
//static uint8_t fac_us = 0;					// DMX_Delay_us used
__root const char MenuStr[]={"Version :"__DATE__ __TIME__ __FILE__};

/* Clear DMX Buffer */
void clrDmxData()
{
	int i;
	for(i=1;i<513;i++)
	{
		dmxData[i] = 0;
	}
}
/* Set Tx_GPIO_Mode */
//iomode 0:OUTPUT_PP
//iomode 1:AF_PP
void GPIO_Tx_Config_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

      /*Configure GPIO pin : PtPin */
    GPIO_InitStruct.Pin = DMX_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DMX_TX_GPIO_Port, &GPIO_InitStruct);
}
void GPIO_Tx_Config_AF(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

      /*Configure GPIO pin : PtPin */
    GPIO_InitStruct.Pin = DMX_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DMX_TX_GPIO_Port, &GPIO_InitStruct);
}

/* Delay nus for break */
void DMX_Delay_us(uint32_t nus)
{
/*    Software Delay  */
    uint32_t i=8*nus;
    while(i--);
//      uint32_t delay_curr;
//      int32_t x=0,delay_goal,delay_start;
//      delay_start = SysTick->VAL;
//      //nus *= US_RATIO;
//      x = delay_start - nus;
//      if (x < 0)
//      {
//          delay_goal = x + (SystemCoreClock / (1000U / uwTickFreq));
//          do{
//                  delay_curr = SysTick->VAL;
//          }while(((delay_curr>delay_goal)||(delay_curr<delay_start);
//      }
//      else
//      {
//            delay_goal = x;
//            do{
//                    delay_curr = SysTick->VAL;
//            }while(((delay_curr>delay_goal)&&(delay_curr<delay_start)));
//      }

/*    Systick reload Delay  */
//    uint32_t temp;
////  temp=HAL_RCC_GetSysClockFreq();
//    //Get System Clock Frequency(in MHz)
//    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
//    fac_us = HAL_RCC_GetSysClockFreq() / 1000000 / 8;
//    SysTick->LOAD = nus*fac_us;          
//    SysTick->VAL = 0x00;
//    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;      
//    do
//    {
//        temp = SysTick->CTRL;
//    }while((temp&0x01) && !(temp&(1<<16)));   
//    //SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
//    SysTick->LOAD = 0x01193F;
//    SysTick->VAL = 0X00;     
}
/* Send Break sign and 00 Code */
void DMX_Break()
{
    GPIO_Tx_Config_OUT();     //Set UART TX pin mode to OUTPUT
    DMX_TX_Low;
    DMX_Delay_us(150);        //DMX512 1990's BREAK >88us
    DMX_TX_High;
    DMX_Delay_us(15);         //DMX512 1990's Mark after break MAB >8us
    GPIO_Tx_Config_AF();
    /* Send Start Code 00 */
    DMX_Send_9Data(0x00);
}
/* Send 9bit data and 9bit always set */
void DMX_Send_9Data(uint8_t tempdata)
{
    if(DMX_UART->SR & (1<<6))
    { 
        DMX_UART->DR = 0x0100 | tempdata;
    } 
    while((DMX_UART->SR&0X40)==0);//waiting for Send data over
}
/* Send packet data,tempnum must <512 */
void DMX_Send_Packet(uint16_t tempnum)
{
    uint16_t i=0;
    DMX_Break();        //Break and Start Code
    if(tempnum==0)      //set to defult
        tempnum=512;
    while(i < tempnum)  //1-512
    {
        DMX_Send_9Data(dmxData[i]);
        i++; 
    }
//    HAL_Delay(20);      //Mark time after Slot
}
/* Init DMX parameter */
void DMX_Init()
{
/* Init HardWare Info */
    DMX_model=DMX_MODEL_NUM;
    DMX_Chn=DMX_MODEL_CHN;
    if((DMX_model*DMX_Chn)>512) //data must <512
       printf("ERROR Data too long !!\r\n");
/* Init by HAL_Init() */
//    DMX_GPIO_Init();    //init RS485_DIR_Pin
//    DMX_UART_Init();    //init UART port and Pins
    clrDmxData();       //Clear Data
}
/* Write Address For IC,tempadd is First IC add */
void DMX_Write_Add(uint16_t tempadd)
{
    uint8_t i;
    uint16_t address;
    address=tempadd;
//    if(tempadd == 0)    //DMX IC start at 1
//        address=1;
    DMX_Reset();        //Send 2S Reset sign
    for(i = 0; i < DMX_model; ++i)
    {
         DMX_Send_ADD_Data(0x80,address); 
         address+=DMX_Chn;
    }
    address=tempadd;
    DMX_Reset();
    for(i = 0; i < DMX_model; ++i)
    {
         DMX_Send_ADD_Data(0xC0,address); 
         address+=DMX_Chn;
    }
}
/* Send change IC Address Order */
void DMX_Send_ADD_Data(uint8_t common,uint16_t tempdata)
{
//    uint8_t data_H,data_L,i,tempadd[4];
    uint8_t data_H,data_L,i,tempadd[8];
	data_H=(tempdata>>6)&63;     //Get High 8bit
	data_L=tempdata&63;		    //Get Low 8bit
	data_H=(data_H)|0x80;	    //set high 2bit =10
	data_L=(data_L)|0x40;	    //set high 2bit =01
    
	tempadd[0]=DMX_Transposition(0xAA);
    tempadd[1]=DMX_Transposition(0xF0);
    tempadd[2]=DMX_Transposition(0x34);
    tempadd[3]=DMX_Transposition(0x55); 
	tempadd[4]=DMX_Transposition(0xAC);
    tempadd[5]=DMX_Transposition(common);
    tempadd[6]=DMX_Transposition(data_H);
    tempadd[7]=DMX_Transposition(data_L);
    
//	tempadd[0]=DMX_Transposition(0xAC);
//    tempadd[1]=DMX_Transposition(common);
//    tempadd[2]=DMX_Transposition(data_H);
//    tempadd[3]=DMX_Transposition(data_L);
//    for(i = 0; i < 4; ++i)
    for(i = 0; i < 8; ++i)
    {
#ifdef UCS512C
      DMX_W();
#endif
      DMX_Send_9Data(tempadd[i]);
    }
#ifdef UCS512C
      DMX_W();
#endif
} 
/* Write CMD For IC,tempadd is E6/C6 */
void DMX_Write_CMD(uint16_t tempadd)
{
    uint8_t i;
    DMX_Reset();        //Send 2S Reset sign
    for(i = 0; i < DMX_model; ++i)
    {
         DMX_Send_CMD_Data(0x82,tempadd); 
    }
    DMX_Reset();
    for(i = 0; i < DMX_model; ++i)
    {
         DMX_Send_CMD_Data(0xC2,tempadd); 
    }
}
/* Send CMD Order*/
void DMX_Send_CMD_Data(uint8_t common,uint8_t tempcmd)
{
//    tempcmd ：xxp0 0110
//        1110 0110	P=1 E6  Keep the last frame
//        1100 0110	P=0 C6  Power on state
//    DEF_R: R Default value at Power on state
    uint8_t i,tempadd[11];
	tempadd[0]=DMX_Transposition(0xAA);
    tempadd[1]=DMX_Transposition(0xF0);
    tempadd[2]=DMX_Transposition(0x34);
    tempadd[3]=DMX_Transposition(0x54); //ADD =55
	tempadd[4]=DMX_Transposition(0xAC);
    tempadd[5]=DMX_Transposition(common); //ADD=common
    tempadd[6]=DMX_Transposition(tempcmd);
    tempadd[7]=DEF_R;
    tempadd[8]=DEF_G;
    tempadd[9]=DEF_B;
    tempadd[10]=DEF_W;
    
    for(i = 0; i < 11; ++i)
    {
#ifdef UCS512C
      DMX_W();
#endif
      DMX_Send_9Data(tempadd[i]);
    }
#ifdef UCS512C
      DMX_W();
#endif
} 
/* Send Reset sign and 00Code */
void DMX_Reset()
{
    GPIO_Tx_Config_OUT();     //Set UART TX pin mode to OUTPUT
    DMX_TX_Low;       
    HAL_Delay(2000);              //UCS512 RESET >2s
    DMX_TX_High;
    //HAL_Delay(1);                 
    DMX_Delay_us(150);           //UCS512 Mark after RESET MAB >50us
    GPIO_Tx_Config_AF();
    /* Send Start Code 00 */
    DMX_Send_9Data(0x00);
}
/* UCS512C Datasheet */
void DMX_W()
{
    GPIO_Tx_Config_OUT();     //Set UART TX pin mode to OUTPUT
    DMX_TX_High;               
    DMX_Delay_us(200);           //UCS512 Mark after RESET MAB >50us
    GPIO_Tx_Config_AF();
}
/* Transposition LSB to MSB */
uint8_t DMX_Transposition(uint8_t tempchar)
{
	uint8_t i=0;
	uint8_t tempnum=0;	
	for(i=0;i<8;i++)
	{
		tempnum=tempnum>>1; 
		tempnum=((tempchar<<i)&0x80)|tempnum; 
	}
	return tempnum;
}
/* Just for test */
void DMX_Demo_Init()
{  
    //TM512 灯条 每个IC8*3通道 BRG
    //UCS512 点光源 每个IC4通道 RGB_W W为白光LED
    clrDmxData();       //Clear data
    dmxData[0]=255;     //R      
    dmxData[1]=0;       //G
    dmxData[2]=0;       //B
    dmxData[3]=0;       //W
    
    dmxData[4]=0;       //R
    dmxData[5]=0;       //G
    dmxData[6]=0;       //B
    dmxData[7]=0;       //W
    
    dmxData[8]=0;       //R
    dmxData[9]=0;       //G
    dmxData[10]=0;      //B
    dmxData[11]=0;      //W
    
    dmxData[12]=0;      //R
    dmxData[13]=0;      //G
    dmxData[14]=0;      //B
    dmxData[15]=0;      //W
    
    dmxData[16]=0;      //R
    dmxData[17]=0;      //G
    dmxData[18]=0;      //B
    dmxData[19]=0;      //W

}
void DMX_Demo()
{  
    uint16_t num=(DMX_model*DMX_Chn);
    uint16_t m=1,i,t,j;

    DMX_Send_Packet(num);
    /* dmxData << m */
//    for (i = num - 1;i>m-1; i--)
//    {
//        t = dmxData[num-1];
//        for (j = num-1;j>0; j--)
//        {
//            dmxData[j] = dmxData[j-1];
//        }
//        dmxData[0] = t;
//    }
      /* dmxData >> m */
    for (i = 0;i<m; i++)
    {
        t = dmxData[num-1];
        for (j = num-1;j>0; j--)
        {
            dmxData[j] = dmxData[j-1];
        }
        dmxData[0] = t;
    }
}
