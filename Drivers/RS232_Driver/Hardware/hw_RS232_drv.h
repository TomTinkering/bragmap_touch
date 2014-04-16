/** ******************************************************************
   @file hw_RS232_drv.h
   @brief  STM32 RS232 driver hardware layer
   @details

   Hardware level calls required by the RS232 Generic driver RS232_drv.c
   This layer is mostly macro based although some more complex functions
   have an implementation here.

   @copyright

   Copyright(C) 2013 PowerbyProxi Ltd and PowerbyProxi, Inc.
   All rights reserved

   The content of this file  contains  intellectual property including
   copyright and confidential  information  owned  by PowerbyProxi Ltd
   and PowerbyProxi, Inc.  and is  only to  be used in accordance with
   the terms of the licence. The content of  this  file and/or its use
   may be covered  by  New Zealand,  US and/or  other foreign patents.
   The content of this  file remains  the property of PowerbyProxi Ltd
   and PowerbyProxi, Inc. Dissemination or reproduction of the content
   of this file or part  hereof is  strictly prohibited unless written
   permission is obtained  from PowerbyProxi Ltd or PowerbyProxi, Inc.

******************************************************************* */

//*******************************************************************
//Includes
#include "Global.h"
#include "HardwareConfig.h"
#include "stm32f0xx.h"

#ifndef HW_RS232_DRV_H_
	#define HW_RS232_DRV_H_

//*******************************************************************
//#Defines

//library aliases
#define USART_PORT_DEF USART1


//Set BaudRate Macro
#define HW_RS232_SET_BAUDRATE(CLOCK,RATE)   ( hw_RS232_Set_Baudrate(RATE) )
//macros to change uart nr of data bits settings
#define HW_RS232_SET_DATABITS_8()           ( p_Usart_Port->CR1 &= ~(USART_CR1_M) ) //clear wordlength bit = 8bits
#define HW_RS232_SET_DATABITS_9()           ( p_Usart_Port->CR1 |=  (USART_CR1_M) )  //set   wordlength bit = 9bits
//macros to change uart parity settings
#define HW_RS232_SET_PARITY_ODD()           do{   p_Usart_Port->CR1 |= (USART_CR1_PCE);    \
                                                  p_Usart_Port->CR1 |= (USART_CR1_PS);      \
                                            }while(0) //enable parity, and set to odd

#define HW_RS232_SET_PARITY_EVEN()          do{   p_Usart_Port->CR1 |= (USART_CR1_PCE);    \
                                                  p_Usart_Port->CR1 &= ~(USART_CR1_PS);     \
                                            }while(0) //enable parity, and set to even

#define HW_RS232_SET_PARITY_NONE()          ( p_Usart_Port->CR1 &= ~(USART_CR1_PCE) ) //disable parity

//macro to change uart stop bits settings
#define HW_RS232_SET_STOPBITS_1()           ( p_Usart_Port->CR2 &= ~(USART_CR2_STOP_0) & ~(USART_CR2_STOP_1) )

#define HW_RS232_SET_STOPBITS_2()           do{   p_Usart_Port->CR2 &= ~(USART_CR2_STOP_0);    \
                                                  p_Usart_Port->CR2 |= (USART_CR2_STOP_1);     \
                                            }while(0) //set to 2 stop bits

//PIC32 UART hardware init and enable macros
#define HW_RS232_INIT()                     (hw_RS232_Init())
//Set peripheral enable bit, Tx enable bit, Rx enable bit
#define HW_RS232_ENABLE()                   (p_Usart_Port->CR1 |= (USART_CR1_RE) | \
                                                                  (USART_CR1_TE) | \
                                                                  (USART_CR1_UE)    )

//STM32 interrupt macros/definitions
#define HW_INT_ENABLED          ENABLE
#define HW_INT_DISABLED         DISABLE
#define HW_RS232_RX_INTERRUPT   USART_IT_RXNE
#define HW_RS232_TX_INTERRUPT   USART_IT_TXE

#define HW_RS232_SETUP_INTERRUPTS(RXI,TXI)  ( hw_RS232_Setup_Interrupts(RXI,TXI) )
#define HW_SET_INTERRUPT_STATE(CHAN,STATE)  ( USART_ITConfig(USART_PORT_DEF, CHAN, STATE) )
#define HW_GET_RX_INTERRUPT_STATE()         ( ( (p_Usart_Port->CR1 & USART_CR1_RXNEIE) != 0 ) ? HW_INT_ENABLED : HW_INT_DISABLED )
#define HW_GET_TX_INTERRUPT_STATE()         ( ( (p_Usart_Port->CR1 & USART_CR1_TCIE) != 0 ) ? HW_INT_ENABLED : HW_INT_DISABLED )

//PIC32 Send Byte macro
#define HW_RS232_SEND_BYTE(DATA)            ( p_Usart_Port->TDR = (DATA & (uint16_t)0x01FF) )
//Clear UART error register macro
#define HW_RS232_CLEAR_ERRORS()             ( p_Usart_Port->ISR &= ~(USART_ISR_PE | USART_ISR_FE | USART_ISR_ABRE | USART_ISR_ORE) ) //set first 9 bits to zero


//*******************************************************************
//Typedefs

typedef FunctionalState hw_RS232_INT_t;

//*******************************************************************
//External Variables

extern volatile USART_TypeDef *p_Usart_Port;

//*******************************************************************
//Function Prototypes

void hw_RS232_Init(void);
void hw_RS232_Setup_Interrupts(void (*RX_Interrupt_Handler)(uint8_t),void (*TX_Interrupt_Handler)(void));
void hw_RS232_Set_Baudrate(uint32_t);

#endif //HW_RS232_DRV_H_

//END OF hw_RS232_drv.h
