/** ******************************************************************
   @file hw_GPIO_Touch_STM32F05xx_drv.h
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
#include "HardwareConfig.h"
#include "stm32f0xx.h"
#include "stm32f0xx_dac.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"

#ifndef __HW_GPIO_TOUCH_STM32F05XX_DRV_H_
  #define __HW_GPIO_TOUCH_STM32F05XX_DRV_H_


  //*******************************************************************
  //  Definitions
  //*******************************************************************

  //define the pins used for this touch driver for each GPIO port
  #define GPIO_A_TOUCH_PINS (GPIO_Pin_0)
  #define GPIO_B_TOUCH_PINS (0x0000)
  #define GPIO_C_TOUCH_PINS ( GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3 | \
                              GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 )
  #define GPIO_D_TOUCH_PINS (0x0000)
  #define GPIO_F_TOUCH_PINS (GPIO_Pin_0 | GPIO_Pin_1)

  #define GPIO_TOUCH_GET_INPUTS_GPIOA() (GPIO_ReadInputData(GPIOA))
  #define GPIO_TOUCH_GET_INPUTS_GPIOB() (GPIO_ReadInputData(GPIOB))
  #define GPIO_TOUCH_GET_INPUTS_GPIOC() (GPIO_ReadInputData(GPIOC))
  #define GPIO_TOUCH_GET_INPUTS_GPIOD() (GPIO_ReadInputData(GPIOD))
  #define GPIO_TOUCH_GET_INPUTS_GPIOF() (GPIO_ReadInputData(GPIOF))
//
//  #define GPIO_TOUCH_SET_INPUTS_GPIOA(INPUTS,VAL) (hw_GPIO_TOUCH_SetInputs(INPUTS,VAL,GPIOA))
//  #define GPIO_TOUCH_SET_INPUTS_GPIOB(INPUTS,VAL) (hw_GPIO_TOUCH_SetInputs(INPUTS,VAL,GPIOB))
//  #define GPIO_TOUCH_SET_INPUTS_GPIOC(INPUTS,VAL) (hw_GPIO_TOUCH_SetInputs(INPUTS,VAL,GPIOC))
//  #define GPIO_TOUCH_SET_INPUTS_GPIOD(INPUTS,VAL) (hw_GPIO_TOUCH_SetInputs(INPUTS,VAL,GPIOD))
//  #define GPIO_TOUCH_SET_INPUTS_GPIOF(INPUTS,VAL) (hw_GPIO_TOUCH_SetInputs(INPUTS,VAL,GPIOF))

  //*******************************************************************
  //  Typedefs
  //*******************************************************************

  //*******************************************************************
  //External Variables
  //*******************************************************************


  //*******************************************************************
  //Function Prototypes

  /** @brief   Configures and activates the correct GPIO pins as input */
  void hw_GPIO_TOUCH_Init(void);
  /** @brief   Blocking, display pin hi/lo trigger levels */
  void hw_GPIO_TOUCH_TestTriggerLevels(void);
//
//  void hw_GPIO_TOUCH_SetInputs(uint16_t pins,uint8_t value,GPIO_TypeDef port);

#endif //end of guard
