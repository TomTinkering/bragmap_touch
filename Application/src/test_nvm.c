/*
 * test_nvm.c
 *
 *  Created on: 14/04/2014
 *      Author: tomv
 */

//=================================================================
// Includes
#include "Global.h"
#include "RS232_drv.h"
#include "TERM_Lib.h"
#include "AppConfig.h"
#include "SYSTIME_drv.h"
#include <string.h>
#include "CTS_NVM_drv.h"
#include "stm32f0xx.h"
#include "stm32f0xx_misc.h"
#include "stm32f0xx_usart.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"

#include "test_nvm.h"

/** *****************************************************************
 * @brief  a simple test app to confirm nvm is working
 * @retval 0 error, this function should not return!
 ***************************************************************** */
uint8_t test_nvm(void) {

  uint16_t packetCount = 0;
  app_config_t testConfig;
  app_config_t readConfig;
  cts_nvm_ret_t tmpReturn;
  memcpy(&testConfig.testString[0],"1234567890",11);

  //initialize uart and TERM lib
  RS232_Init();
  TERM_Lib_Init(&TERM_PutByte_Helper);

  //initialize system time
  SYSTIME_Init();

  //signal life to user
  DEBUG_LINE("Main :: STM32F0 Online");
  sign_of_life();

  //read from flash
  //Initialize NVM

  if( (tmpReturn = CTS_NVM_Init()) != CTS_NVM_OK)
  {
    DEBUG_LINE("Main:: NVM Init Failed");
  }
  else
  {
    DEBUG_LINE("Main:: NVM Init Success");
  }

  /*get configuration
   *    if fail, forcestore new configuration
   *      */
  if( (tmpReturn = CTS_NVM_Load((uint8_t *)&readConfig, sizeof(app_config_t)) ) != CTS_NVM_OK)
  { //if load fails
    DEBUG_LINE("Main:: NVM Load Failed");
    if( (tmpReturn = CTS_NVM_ForceStore((uint8_t *)&testConfig, sizeof(app_config_t)) ) != CTS_NVM_OK)
    {
      DEBUG_LINE("Main:: NVM Force-Store Failed");
    }
    else
    {
      DEBUG_LINE("Main:: NVM Default Loaded");
    }
  }
  else
  {
    DEBUG_STRING("Main:: Loaded ");
    DEBUG_LINE(readConfig.testString);
  }

  //program loop
  while (1){
    //USART_puts(USART1, "U"); // just send a message to indicate that it works
    packetCount = RS232_GetRXPacketCount();

    DEBUG_STRING("Main :: PacketCount [");
    DEBUG_DECIMAL(packetCount,4);
    DEBUG_LINE("]");

    delay(1000000);
    if(packetCount > 11){

      DEBUG_LINE("Main:: NVM Storing new Config");
      //get new configuration message
      RS232_GetByteBlock((uint8_t*)&testConfig.testString[0],11);
      testConfig.testString[10]=(char)0; //set string delimiter

      //disable interrupts
      USART_ITConfig(USART1, USART_IT_RXNE, DISABLE); // enable the USART1 receive interrupt
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE); // enable the USART1 transmission complete interrupt

      tmpReturn = CTS_NVM_Store((uint8_t *)&testConfig, sizeof(app_config_t));

      //enable interrupts
      USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // enable the USART1 receive interrupt
      USART_ITConfig(USART1, USART_IT_TXE, ENABLE); // enable the USART1 transmission complete interrupt

      if(tmpReturn  != CTS_NVM_OK)
      {
        DEBUG_LINE("Main:: NVM Store Failed");
      }
      else
      {
        DEBUG_LINE("Main:: NVM Store Succeeded");
      }

    }
  }

  return 0;
  //End of Main
}
