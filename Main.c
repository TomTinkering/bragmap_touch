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
#include "TERM_Lib.h"
#include "AppConfig.h"
#include "SYSTIME_drv.h"
#include <string.h>
//#include "CTS_NVM_drv.h"
#include "stm32f0xx.h"
#include "stm32f0xx_misc.h"
#include "stm32f0xx_usart.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"
#include "hw_DAC_STM32F05xx_drv.h"
#include "hw_GPIO_Touch_STM32F05xx_drv.h"

//*******************************************************************
//#Defines

#define GPIO_TEST_STEP_SIZE (32)
#define DAC_OUTPUT_HIGH_LEVEL (1056)
#define BSRR_VAL        0x0300

//*******************************************************************
//Variables

//*******************************************************************
//Function Prototypes
void sign_of_life(void);
void setDACLevel(uint16_t value);

//*******************************************************************
//Main Program

/** *****************************************************************
 * @brief  application entry point
 * @retval 0 error, this function should not return!
 ***************************************************************** */
int main(void) {

  uint16_t curDACLevel = 0x0000;
  uint16_t offset = GPIO_TEST_STEP_SIZE;
  uint16_t prevInputs = 0, curInputs = 0;

  //initialize uart and TERM lib
  RS232_Init();
  TERM_Lib_Init(&TERM_PutByte_Helper);

  //initialize system time
  SYSTIME_Init();
  //start first timeout
  SYSTIME_SetTimeout(0,40);

  //signal life to user
  DEBUG_LINE("Main :: STM32F0 Online");
  sign_of_life();

  //initialize DAC interface
  hw_DAC_Init();

  //initialize GPIO Touch inputs
  hw_GPIO_TOUCH_Init();

  //get current input state
  curInputs = (uint32_t)(GPIO_TOUCH_GET_INPUTS_GPIOC() & GPIO_C_TOUCH_PINS);
  prevInputs = curInputs;

  //enter promgram loop
  while(1)
  {

      //when top or bottom is reached, move in other direction
      curDACLevel += offset;
      if(curDACLevel >= 0x0FFF)
      {
        curDACLevel = 0x0FFF;
        offset = -GPIO_TEST_STEP_SIZE;
      }
      else if (curDACLevel < GPIO_TEST_STEP_SIZE)
      {
        curDACLevel = 0x0000;
        offset = GPIO_TEST_STEP_SIZE;
      }

      if(SYSTIME_GetTimeoutState(0) == SYSTIME_TIMEOUT_EXPIRED)
      {
          if(HW_DAC_GET_OUTPUT() == 0)
            {
              //set DAC output
              setDACLevel(DAC_OUTPUT_HIGH_LEVEL);
            }
          else
            {
              //set DAC output
              setDACLevel(0);
            }
          SYSTIME_SetTimeout(0,40);
      }

      if(SYSTIME_GetTimeoutState(1) == SYSTIME_TIMEOUT_EXPIRED)
      {
          /* Reset PC8 and PC9 */
          GPIOC->BRR = BSRR_VAL;
      }

      //set DAC output
      //setDACLevel(curDACLevel);

      //wait for a few ms to settle
      //SYSTIME_DelayMS(100);
      //output GPIOA state
//      DEBUG_STRING("GPIOA :: [");
//      DEBUG_BIN((uint32_t)(GPIO_TOUCH_GET_INPUTS_GPIOA() & GPIO_A_TOUCH_PINS),16);
//      DEBUG_LINE("]");
//      //output GPIOB state
//      DEBUG_STRING("GPIOB :: [");
//      DEBUG_BIN((uint32_t)(GPIO_TOUCH_GET_INPUTS_GPIOB() & GPIO_B_TOUCH_PINS),16);
//      DEBUG_LINE("]");
      //output GPIOC state
      //DEBUG_STRING("GPIOC :: [");
      curInputs = (GPIO_TOUCH_GET_INPUTS_GPIOC() & GPIO_C_TOUCH_PINS);
      if(prevInputs != curInputs)
        {

          /* Set PC8 and PC9 */
          GPIOC->BSRR = BSRR_VAL;
          SYSTIME_SetTimeout(1,100);

          DEBUG_BIN((uint32_t)(GPIO_TOUCH_GET_INPUTS_GPIOC() & GPIO_C_TOUCH_PINS),16);
          DEBUG_LINE("");
        }
      prevInputs = curInputs;
//      //output GPIOD state
//      DEBUG_STRING("GPIOD :: [");
//      DEBUG_BIN((uint32_t)(GPIO_TOUCH_GET_INPUTS_GPIOD() & GPIO_D_TOUCH_PINS),16);
//      DEBUG_LINE("]");
//      //output GPIOF state
//      DEBUG_STRING("GPIOF :: [");
//      DEBUG_BIN((uint32_t)(GPIO_TOUCH_GET_INPUTS_GPIOF() & GPIO_F_TOUCH_PINS),16);
//      DEBUG_LINE("]");

      //delay till next iteration
      //SYSTIME_DelayMS(500);

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

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */

void sign_of_life(void)
{
  /*!< At this stage the microcontroller clock setting is already configured,
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f0xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f0xx.c file
     */
	//  set led on to indicate life
  int8_t i;
  GPIO_InitTypeDef   GPIO_InitStructure;

  /* GPIOC Periph clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

  /* Configure PC8 and PC9 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* To achieve GPIO toggling maximum frequency, the following  sequence is mandatory.
     You can monitor PC8 and PC9 on the scope to measure the output signal.
     If you need to fine tune this frequency, you can add more GPIO set/reset
     cycles to minimize more the infinite loop timing.
     This code needs to be compiled with high speed optimization option.  */
  for(i=0;i<5;i++)
  {

	/* Set PC8 and PC9 */
    GPIOC->BSRR = BSRR_VAL;
    SYSTIME_DelayMS(500);
    /* Reset PC8 and PC9 */
    GPIOC->BRR = BSRR_VAL;
    SYSTIME_DelayMS(500);
  }
}

void setDACLevel(uint16_t value)
{
  //limit DAC output value
  if (value > 0xFFF) {value = 0xFFF;}

  //set DAC output level and output result over uart
  HW_DAC_SET_OUTPUT(value);
  //DEBUG_STRING("DAC :: Output");
  //DEBUG_DECIMAL(HW_DAC_GET_OUTPUT(),4);
  //DEBUG_STRING(" =");
  //DEBUG_DECIMAL((((int32_t)HW_DAC_GET_OUTPUT())*6000)/4096,4);
  //DEBUG_LINE(" mv");

}


