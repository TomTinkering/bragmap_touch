/** ******************************************************************
   @file hw_STM32F0_FLASH_drv.c
   @brief  hw_STM32F0_FLASH_drv Source
   @details

   ...

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

//TODO: THIS DRIVER RELIES ON THE FLASH PAGE OF THE BASE ADDRRESS USED TO BE
//      UNOCCUPIED BY PROGRAM CODE, SINCE IT HAS TO SUPPORT REWRITING THE FLASH
//      WITHOUT REPROGRAMMING THE DEVICE. A NEATER WAY TO DO THIS IS TO ENFORCE
//      THIS FREE SECTION OF FLASH IN THE LINKER....

//*******************************************************************
//Includes
#include "Global.h"
#include "stm32f0xx_flash.h"
#include "stm32f0xx_rcc.h"
#include <string.h>
#include "hw_STM32F0_FLASH_drv.h"

//*******************************************************************
//Definitions

//*******************************************************************
//Variables

//*******************************************************************
//Helper Function Prototypes (declare static!)

//*******************************************************************
//Function Implementations


/** ******************************************************************
  * @brief  Initialise FLASH access capability
  *
  * @note   This library requires HSI to be enabled, and will enable
  *         it in this INIT.
  *
  * @retval HW_NVM_OK Flash initialised succesfully
  * @retval HW_NVM_INIT_FAILED Flash initialisation failed
******************************************************************* */
hw_nvm_ret_t hw_STM32F0_FLASH_drv_Init(void)
{

  uint32_t i;
  //enable necessary hardware
  RCC_HSICmd(ENABLE);    // Enable HSI (high-speed internal oscillator)

  for(i=0;i<HW_NVM_INIT_RETRIES;i++){
    if(RCC_GetFlagStatus(RCC_CR_HSIRDY) != SET) break;
  }

  //return result
  return (i == HW_NVM_INIT_RETRIES) ? HW_NVM_INIT_FAILED : HW_NVM_OK ;
}


/** ******************************************************************
  * @brief  Read block of data from FLASH
  *
  * @note   Since flash config read should always succeed (we are in
  *         trouble anyway if it doesnt) no actual success check is done
  *
  * @param  Address specifies the address to start reading.
  * @param  dataBuff buffer to store the read data
  * @param  length the number of bytes to be read
  *
  * @retval HW_NVM_OK Flash Read Succesfully
 ******************************************************************* */
hw_nvm_ret_t hw_STM32F0_FLASH_ReadBlock(intptr_t Address, uint8_t *dataBuff, uint32_t length)
{

  //cpy from memory
  memcpy((void *)dataBuff, (void *)Address, length);
  //if reading from flash fails, we are in trouble anyway.
  return HW_NVM_OK;

}


/** ******************************************************************
  * @brief  Compares a byte array to a byte array at specified NVM address
  *
  * @param  Address specifies the address to start comparing.
  * @param  dataBuff data to compare NVM to
  * @param  length the number of bytes to be compared
  *
  * @retval HW_NVM_OK Flash Compared Succesfully
  * @retval HW_NVM_COMPARE_FAILED Flash Compare Failed
 ******************************************************************* */
hw_nvm_ret_t hw_STM32F0_FLASH_CompareBlock(intptr_t Address, uint8_t *Data, uint32_t length)
{

    uint32_t i;
    //convert address to pointer
    uint8_t *p_NVM = (uint8_t *)Address;
    hw_nvm_ret_t result = HW_NVM_OK;

    //compare data in memory
    for(i=0;i<length;i++)
    { //for each byte
      if(p_NVM[i] != Data[i])
      {
        result = HW_NVM_COMPARE_FAILED;
        break;
      }
    }

    //return result
    return result;

}


/** ******************************************************************
  * @brief  Programs a byte array to a specified address.
  *
  * @note   This function must be used when the device voltage range is from
  *         2.7V to 3.6V and an External Vpp is present.
  *
  * @param  Address specifies the address to be programmed.
  * @param  Data specifies the data to be programmed.
  * @param  length the number of bytes to be programmed
  *
  * @retval HW_NVM_OK Flash written succesfully
  * @retval HW_NVM_FLASH_NOT_RDY Flash was no ready for writing at start
  * @retval HW_NVM_FLASH_ERASE_FAILED Something went wrong while/after erasing flash
  * @retval HW_NVM_FLASH_WRITE_FAILED Something went wrong while/after writing to flash
 ******************************************************************* */
hw_nvm_ret_t hw_STM32F0_FLASH_WriteBlock(intptr_t Address, uint8_t *Data, uint32_t length)
{
  hw_nvm_ret_t result = HW_NVM_OK;
  uint32_t i;

  DEBUG_LINE("hwFlash :: waiting for flash..");

  /* Wait for last operation to be completed */
  if( FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT) != FLASH_COMPLETE)
  {
    result = HW_NVM_FLASH_NOT_RDY;
  }

  DEBUG_LINE("hwFlash :: unlocking flash..");

  /* Unlock the FLASH */
  FLASH_Unlock();

  DEBUG_STRING("hwFlash :: erasing page [");
  DEBUG_HEX((uint8_t *)&Address, 4);
  DEBUG_LINE("]");

  /* Erase FLASH and Wait for last operation to be completed */
  if( FLASH_ErasePage((uint32_t)Address)  != FLASH_COMPLETE )
  {
    result = HW_NVM_FLASH_ERASE_FAILED;
  }

  /* Clear all FLASH flags */
  FLASH_ClearFlag(  FLASH_FLAG_EOP    |
                    FLASH_FLAG_WRPERR |
                    FLASH_FLAG_BSY     );

  FLASH->CR |= FLASH_CR_PG;

  DEBUG_LINE("hwFlash :: writing to flash..");

  for(i=0;i<length;i+=2){
	//Copy data into flash.
	if(length-i == 1){ //if last (single) byte is to be copied
		((uint16_t *)Address)[i/2] = (uint16_t)Data[i] ;
	} else {
		((uint16_t *)Address)[i/2] = ((uint16_t *)Data)[i/2] ;
	}

    /* Wait for last operation to be completed */
    if( FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT) != FLASH_COMPLETE)
    {
      result = HW_NVM_FLASH_WRITE_FAILED;
    }
  }

  /* if the program operation is completed, disable the PG Bit */
  FLASH->CR &= (~FLASH_CR_PG);

  DEBUG_LINE("hwFlash :: locking flash..");

  /* Lock the FLASH */
  FLASH_Lock();

  /* Return the Program Status */
  return result;
}

//END OF hw_STM32F0_FLASH_drv.c
