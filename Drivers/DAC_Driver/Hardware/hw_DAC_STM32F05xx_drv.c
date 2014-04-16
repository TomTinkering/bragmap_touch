/** ******************************************************************
   @file hw_DAC_STM32F05xx_drv.c
   @brief  DAC hardware layer for onboard DAC stm32f05xx
   @details

   Minimalist DAC driver for the onboard DAC of the stm32f05xx. Allows
   initialization of the DAC peripheral, and setting and reading of the
   output setting. Advanced DMA features not supported

******************************************************************* */


//*******************************************************************
//Includes
#include "Global.h"
#include "hw_DAC_STM32F05xx_drv.h"
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
void hw_DAC_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;
    DAC_InitTypeDef  DAC_InitStructure;

    /* DAC Periph clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    /* GPIOA clock enable */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

    /* Configure PA.04 (DAC_OUT1) as analog */
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* DAC channel1 Configuration */
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;

    /* DAC Channel1 Init */
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);

    /* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is
       automatically connected to the DAC converter. */
    DAC_Cmd(DAC_Channel_1, ENABLE);

}









