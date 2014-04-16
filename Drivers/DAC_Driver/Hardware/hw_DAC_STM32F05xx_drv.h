/** ******************************************************************
   @file hw_DAC_STM32F05xx_drv.h
   @brief  DAC hardware layer for onboard DAC stm32f05xx
   @details

   Minimalist DAC driver for the onboard DAC of the stm32f05xx. Allows
   initialization of the DAC peripheral, and setting and reading of the
   output setting. Advanced DMA features not supported

******************************************************************* */


//*******************************************************************
//Includes
#include "Global.h"
#include "HardwareConfig.h"
#include "stm32f0xx.h"
#include "stm32f0xx_dac.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"


#ifndef __DAC_DRV_STM32F05XX_H
  #define __DAC_DRV_STM32F05XX_H

  //*******************************************************************
  //  Definitions
  //*******************************************************************

  #define HW_DAC_INIT() (hw_DAC_Init())
  #define HW_DAC_SET_OUTPUT(VALUE) (DAC_SetChannel1Data(DAC_Align_12b_R, VALUE))
  #define HW_DAC_GET_OUTPUT() (DAC_GetDataOutputValue(DAC_Channel_1))

  //*******************************************************************
  //  Typedefs
  //*******************************************************************

  //*******************************************************************
  //External Variables
  //*******************************************************************


  //*******************************************************************
  //Function Prototypes

  /** @brief   Configures and activates the DAC peripheral. */
  void hw_DAC_Init(void);

#endif //end of guard








