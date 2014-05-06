/** ******************************************************************
   @file hw_STM32F0_FLASH_drv.h
   @brief  hw_STM32F0_FLASH_drv Header
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

//*******************************************************************
//Includes
#include "Global.h"
#include "TERM_Lib.h"
#include <string.h>
#include "stm32f10x_flash.h"

#ifndef HW_STM32F0_FLASH_DRV_H_
	#define HW_STM32F0_FLASH_DRV_H_

//*******************************************************************
//Definitions

#define HW_NVM_FLASH_PAGE     (63)
#define HW_NVM_INIT_RETRIES   (50)

#define HW_NVM_INIT()                             ( hw_STM32F0_FLASH_drv_Init() )
#define HW_NVM_WRITE_BLOCK( ADDRESS, DATA, LEN )  ( hw_STM32F0_FLASH_WriteBlock(ADDRESS,DATA,LEN) )
#define HW_NVM_READ_BLOCK( ADDRESS, DEST, LEN )   ( hw_STM32F0_FLASH_ReadBlock(ADDRESS,DEST,LEN) )
#define HW_NVM_COMPARE( ADDRESS, DATA, LEN)       ( hw_STM32F0_FLASH_CompareBlock(ADDRESS,DATA,LEN) )

//*******************************************************************
//Type Definitions

typedef enum hw_nvm_ret_t
{ HW_NVM_ERR,
  HW_NVM_OK,
  HW_NVM_WRITE_FAILED,
  HW_NVM_COMPARE_FAILED,
  HW_NVM_READ_ERROR,
  HW_NVM_WRITE_ERROR,
  HW_NVM_FLASH_NOT_RDY,
  HW_NVM_FLASH_ERASE_FAILED,
  HW_NVM_FLASH_WRITE_FAILED,
  HW_NVM_INIT_FAILED
}hw_nvm_ret_t;

//*******************************************************************
//External Variables

//*******************************************************************
//Function Prototypes 

/*! @brief hw_STM32F0_FLASH_drv initializer */
hw_nvm_ret_t hw_STM32F0_FLASH_drv_Init(void);
/** @brief  Programs a byte array to a specified NVM address, clears NVM! */
hw_nvm_ret_t hw_STM32F0_FLASH_WriteBlock(intptr_t Address, uint8_t *Data, uint32_t length);
/** @brief  Reads a byte array from a specified NVM address */
hw_nvm_ret_t hw_STM32F0_FLASH_ReadBlock(intptr_t Address, uint8_t *dataBuff, uint32_t length);
/** @brief  Compares a byte array to a byte array at specified NVM address */
hw_nvm_ret_t hw_STM32F0_FLASH_CompareBlock(intptr_t Address, uint8_t *Data, uint32_t length);

#endif //HW_STM32F0_FLASH_DRV_H_

//END OF hw_STM32F0_FLASH_drv.h
