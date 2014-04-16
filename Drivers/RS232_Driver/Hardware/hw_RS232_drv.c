/** ******************************************************************
   @file hw_RS232_drv.c
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
#include "hw_RS232_drv.h"
//STM32 Lib dependencies
#include "stm32f0xx.h"
#include "stm32f0xx_misc.h"
#include "stm32f0xx_usart.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"
//*******************************************************************
//#Defines

//*******************************************************************
//Variables

/** @brief pointer to STM32 USART peripheral to be used */
volatile USART_TypeDef *p_Usart_Port = USART_PORT_DEF;

/** @brief pointer to higher level RX interrupt handler callback function */
void (*hw_RX_Interrupt_Callback)(uint8_t);
/** @brief pointer to higher level TX interrupt handler callback function */
void (*hw_TX_Interrupt_Callback)();

//*******************************************************************
//Functions
//*******************************************************************


/** *******************************************************************
 * @brief   Configures and activates the UART peripheral hardware.
 * @details
 *
 * Setup GPIO clock, direction, function for use with USART. Also
 * set USART peripheral to default settings (9600,8,N,1).
 *
***********************************************************************/
void hw_RS232_Init(void)
{

  // Setup UART hardware using STM32 Library structures
  GPIO_InitTypeDef GPIO_InitStruct;
  USART_InitTypeDef USART_InitStruct;

  // enable APB2 peripheral clock for USART1
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

  // enable the peripheral clock for the pins used by USART1
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  //setup the GPIO initialization structure
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  //call the init function for this structure
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Connect the RX and TX pins to the USART function
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1); //
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);

  //Setup the USART initialisation structure
  USART_InitStruct.USART_BaudRate = 9600;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  //call the init function for this structure
  USART_Init(USART1, &USART_InitStruct);

}

/** *******************************************************************
 * @brief   Calculates and sets the USART baud rate settings
 * @param BaudRate baud rate to be set (2400,4800,9600... up to 115200)
***********************************************************************/
void hw_RS232_Set_Baudrate(uint32_t BaudRate)
{

    uint32_t tmpreg = 0x00, apbclock = 0x00;
    uint32_t integerdivider = 0x00;
    uint32_t fractionaldivider = 0x00;
    RCC_ClocksTypeDef RCC_ClocksStatus;

    /*---------------------------- USART BRR Configuration -----------------------*/
    /* Configure the USART Baud Rate */
    RCC_GetClocksFreq(&RCC_ClocksStatus);

    apbclock = RCC_ClocksStatus.PCLK_Frequency;

    /* Determine the integer part */
    if ((p_Usart_Port->CR1 & USART_CR1_OVER8) != 0)
    {
      /* Integer part computing in case Oversampling mode is 8 Samples */
      integerdivider = ((25 * apbclock) / (2 * (BaudRate)));
    }
    else /* if ((USARTx->CR1 & USART_CR1_OVER8) == 0) */
    {
      /* Integer part computing in case Oversampling mode is 16 Samples */
      integerdivider = ((25 * apbclock) / (4 * (BaudRate)));
    }

    tmpreg = (integerdivider / 100) << 4;
    /* Determine the fractional part */
    fractionaldivider = integerdivider - (100 * (tmpreg >> 4));

    /* Implement the fractional part in the register */
    if ((p_Usart_Port->CR1 & USART_CR1_OVER8) != 0)
    {
      tmpreg |= ((((fractionaldivider * 8) + 50) / 100)) & ((uint8_t)0x07);
    }
    else /* if ((USARTx->CR1 & USART_CR1_OVER8) == 0) */
    {
      tmpreg |= ((((fractionaldivider * 16) + 50) / 100)) & ((uint8_t)0x0F);
    }

}

/** *******************************************************************
 * @brief   Set up the STM32 interrupt controller and USART interrupts
 * @details
 *
 * This functions stores the higher level interrupt callback function
 * pointers. Next the RX empty en TX empty interrupt are enabled which
 * call the callback functions.
 *
 * @param RX_Interrupt_Handler pointer to higher level RX IRQ callback
 * @param TX_Interrupt_Handler pointer to higher level TX IRQ callback
 *
***********************************************************************/
void hw_RS232_Setup_Interrupts(void (*RX_Interrupt_Handler)(uint8_t),void (*TX_Interrupt_Handler)(void))
{

  // this is used to configure the NVIC (nested vector interrupt controller)
  NVIC_InitTypeDef NVIC_InitStructure;

  //store higher level RX interrupt callback function
  hw_RX_Interrupt_Callback = RX_Interrupt_Handler;
  //store higher level TX interrupt callback function
  hw_TX_Interrupt_Callback = TX_Interrupt_Handler;

  // Configure Interrupts
  /* Here the USART1 receive interrupt is enabled
     * and the interrupt controller is configured
     * to jump to the USART1_IRQHandler() function
     * if the USART1 receive interrupt occurs
     */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // enable the USART1 receive interrupt
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE); // enable the USART1 transmission complete interrupt

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;    // we want to configure the USART1 interrupts
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0; // this sets the priority group of the USART1 interrupts
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;      // the USART1 interrupts are globally enabled
    NVIC_Init(&NVIC_InitStructure);              // the properties are passed to the NVIC_Init function which takes care of the low level stuff

}


/** *******************************************************************
 * @brief   STM32 USART interrupt. Calls IRQ callbacks and resets flag.
***********************************************************************/
void USART1_IRQHandler(void) // UART6 Interrupt handler
{
  // Is this an Rx interrupt?
  if( USART_GetITStatus(USART1, USART_IT_RXNE) )
  {
    //call higher level interrupt handler with new data
    hw_RX_Interrupt_Callback(  (uint8_t)(p_Usart_Port->RDR & (uint16_t)0x00FF) );
    //flag is cleared autmatically on read
  }

  // Is this an Tx interrupt?
  if( USART_GetITStatus(USART1, USART_IT_TXE) )
  {
    //clear interrupt flag before retransmission
    p_Usart_Port->ISR &= ~(USART_ISR_TC) & ~(USART_ISR_TXE);
    //call higher level interrupt handler
    hw_TX_Interrupt_Callback();
  }

}

//END OF hw_RS232_drv.c
