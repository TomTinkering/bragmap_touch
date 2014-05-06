/** ******************************************************************
   @file Main.c
   @brief  Application Entry Point
   @details

   The main function in this file serves as the application entry point.
   It is responsible for doing hardware/application initialisation in
   the correct order, and for servicing tasks needed for the application.
   All tasks are called from here, a new task can only run if the previous
   task has return a value.

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

//configuration inclusions
#include "Global.h"
//#include "RS232_drv.h"
#include "TERM_Lib.h"
#include "AppConfig.h"
//#include "SYSTIME_drv.h"
#include <string.h>
//#include "CTS_NVM_drv.h"
#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

//*******************************************************************
//#Defines

//*******************************************************************
//Variables

//*******************************************************************
//Function Prototypes
void sign_of_life(void);

//*******************************************************************
//Main Program

/** *****************************************************************
 * @brief  application entry point
 * @retval 0 error, this function should not return!
 ***************************************************************** */
int main(void) {

//  //initialize uart and TERM lib
//  RS232_Init();
//  TERM_Lib_Init(&TERM_PutByte_Helper);
//
//  //initialize system time
//  SYSTIME_Init();
//  //start first timeout
//  SYSTIME_SetTimeout(0,40);
//
//  //signal life to user
//  DEBUG_LINE("Main :: STM32F0 Online");
//  sign_of_life();

  //enter program loop
  while(1)
  {

  }
  return 0;
  //End of Main
}


//*******************************************************************
//Helper Functions

void TERM_PutByte_Helper(uint8_t data)
{
  //RS232_PutByte(data);
}


void sign_of_life(void)
{
//  /*!< At this stage the microcontroller clock setting is already configured,
//       this is done through SystemInit() function which is called from startup
//       file (startup_stm32f0xx.s) before to branch to application main.
//       To reconfigure the default setting of SystemInit() function, refer to
//       system_stm32f0xx.c file
//     */
//	//  set led on to indicate life
//  int8_t i;
//  GPIO_InitTypeDef   GPIO_InitStructure;
//
//  /* GPIOC Periph clock enable */
//  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
//
//  /* Configure PC8 and PC9 in output pushpull mode */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//  GPIO_Init(GPIOC, &GPIO_InitStructure);
//
//  /* To achieve GPIO toggling maximum frequency, the following  sequence is mandatory.
//     You can monitor PC8 and PC9 on the scope to measure the output signal.
//     If you need to fine tune this frequency, you can add more GPIO set/reset
//     cycles to minimize more the infinite loop timing.
//     This code needs to be compiled with high speed optimization option.  */
//  for(i=0;i<5;i++)
//  {
//
//	/* Set PC8 and PC9 */
//    GPIOC->BSRR = BSRR_VAL;
//    SYSTIME_DelayMS(500);
//    /* Reset PC8 and PC9 */
//    GPIOC->BRR = BSRR_VAL;
//    SYSTIME_DelayMS(500);
//  }
}


