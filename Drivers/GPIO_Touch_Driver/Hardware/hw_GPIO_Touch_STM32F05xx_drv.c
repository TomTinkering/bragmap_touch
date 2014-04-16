/** ******************************************************************
   @file hw_GPIO_Touch_STM32F05xx_drv.c
   @brief  Application Layer for AC Hum based touch inputs
   @details

   Uses onboard DAC to generate a suitable BIAS voltage. This bias
   is added to a high impedance user touch pad, which should cause
   inputs to toggle @ approximately 50Hz. This toggling is decoded
   as being a touch action.

******************************************************************* */


//*******************************************************************
//Includes
#include "Global.h"
#include "hw_DAC_STM32F05xx_drv.h"
#include "hw_GPIO_Touch_STM32F05xx_drv.h"
#include "TERM_Lib.h"
#include "stm32f0xx.h"
#include "stm32f0xx_dac.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"

//*******************************************************************
//  Definitions
//*******************************************************************

//*******************************************************************
//  Typedefs
//*******************************************************************

//*******************************************************************
//External Variables
//*******************************************************************


//*******************************************************************
//Function Prototypes

/** @brief   Configures and activates the DAC peripheral. */
void hw_GPIO_TOUCH_Init(void)
{

    //initialize all enabled inputs
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Configure touch enabled pins as input */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

    //Configure desired pins for GPIOA
    GPIO_InitStructure.GPIO_Pin =  GPIO_A_TOUCH_PINS;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //Configure desired pins for GPIOB
    GPIO_InitStructure.GPIO_Pin =  GPIO_B_TOUCH_PINS;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //Configure desired pins for GPIOC
    GPIO_InitStructure.GPIO_Pin =  GPIO_C_TOUCH_PINS;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    //Configure desired pins for GPIOD
    GPIO_InitStructure.GPIO_Pin =  GPIO_D_TOUCH_PINS;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    //Configure desired pins for GPIOF
    GPIO_InitStructure.GPIO_Pin =  GPIO_F_TOUCH_PINS;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

//
}
//
//
//void hw_GPIO_TOUCH_SetInputs(uint16_t pins,uint8_t value,GPIO_TypeDef port)
//{
//  //GPIO structure
//  GPIO_InitTypeDef GPIO_InitStructure;
//  //reset all pins to input
//  hw_GPIO_TOUCH_Init();
//
//  //set desired pin to value
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//  GPIO_InitStructure.GPIO_Pin =  pins;
//  GPIO_Init(port, &GPIO_InitStructure);
//
//  if(value == 0)
//    {
//      GPIO_SetBits(port, pins);
//    }
//  else
//    {
//      GPIO_ResetBits(port, pins);
//    }
//
//
//}
//
//
//
//

