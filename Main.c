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
#include "RS232_drv.h"
//#include "TERM_Lib.h"
#include "AppConfig.h"
#include "TOUCH_drv.h"
#include "SYSTIME_drv.h"
#include <string.h>

//stm lib includes
#include "stm32f0xx.h"
#include "stm32f0xx_misc.h"
#include "stm32f0xx_usart.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"

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

  //initialize UART and TERM_lib
  RS232_Init();
  TERM_Lib_Init(&TERM_PutByte_Helper);

  //initialize system time
  SYSTIME_Init();
  //start first timeout
  SYSTIME_SetTimeout(0,40);

  //signal life to user
  DEBUG_LINE("Main :: STM32F0 Online");
  sign_of_life();

  //perform single measurement

  DEBUG_LINE("Main :: Initializing TOUCH_drv");
  TOUCH_Init();
  DEBUG_LINE("Main :: Measurement Starting");
  TOUCH_StartMeasurement(UP,SIDE_A,SIDE_B);
  for(int i=0;i<15;i++){
      DEBUG_DECIMAL(TIM2->CNT,8);
      DEBUG_LINE("");
      SYSTIME_DelayMS(50);
  }
  DEBUG_STRING("Main :: Performed ");
  DEBUG_DECIMAL(curMeas,2);
  DEBUG_LINE("Measurements");
  DEBUG_STRING("Main :: Result -> ");
  if(measStatus == TOUCH_MEAS_DONE){
      DEBUG_LINE("TOUCH_MEAS_DONE");
  } else {
      DEBUG_LINE("TOUCH_MEAS_ACTIVE");
  }
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
  RS232_PutByte(data);
}

//set led pins to output, and toggle them a few times to
//indicate uC operation has started
void sign_of_life(void)
{
  //  set led on to indicate life
  int8_t i;
  GPIO_InitTypeDef   GPIO_InitStructure;

  /* GPIOC Peripheral clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

  /* Configure PC8 and PC9 in output push-pull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  //toggle led's 5 times
  for(i=0;i<5;i++)
  {	/* Set PC8 and PC9 */
    GPIOC->BSRR = BSRR_VAL;
    SYSTIME_DelayMS(200);
    /* Reset PC8 and PC9 */
    GPIOC->BRR = BSRR_VAL;
    SYSTIME_DelayMS(200);
  }

}

