/** ******************************************************************
   @file CTS_NVM_drv.c
   @brief  CTS_NVM_drv Source
   @details

   This driver provides a default interface for storing and retreiving
   non-volatile configuration settings. It assumes the configuration
   structure is the only thing that is stored, thus no addressing is
   required apart from the base address definition.

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
#include "CTS_NVM_drv.h"
#include "hw_STM32F0_FLASH_drv.h"
#include "TERM_Lib.h"

//compiler includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>




//*******************************************************************
//Definitions

//*******************************************************************
//Variables

//*******************************************************************
//Helper Function Prototypes (declare static!)

/* @brief Check CTS frame for inconsistencies. */
static cts_nvm_ret_t CTS_NVM_ValidatePage(cts_nvm_frame_t *frame, uint32_t dataLength);
/* @brief Declare NVM to reside in "cts_config" section. This section is declared in the
 * linker script, and should be aligned with the start of the last flash page. The nocommon
 * attribute is used to force this variable to be allocated in this section*/
static const cts_nvm_frame_t ctsFrame __attribute__ ((section (".cts_config"), nocommon));

#define CTS_NVM_BASE_ADDRESS ((intptr_t)&ctsFrame)

//*******************************************************************
//Function Implementations

/** ******************************************************************
  @brief Perform NVM initialisation

  @retval CTS_NVM_HW_INIT_FAILED Hardware initialisation failed
  @retval CTS_NVM_OK initialisation successfull

******************************************************************* */
cts_nvm_ret_t CTS_NVM_Init()
{

  cts_nvm_ret_t result = CTS_NVM_OK;

  //initialise NVM hardware
  if(HW_NVM_INIT() != HW_NVM_OK)
  {
    result = CTS_NVM_HW_INIT_FAILED;
  }
  //return result
  return result;
}


/** ******************************************************************
  @brief Load CTS application configuration from NVM. Performs CRC check
  @details

  Attempts to load the CTS application configuration from NVM. If this
  configuration is invalid, it will state so in the return value.

  @param destBuff buffer to store the retreived structure in
  @param length size of the structure to read, used for validation

  @retval CTS_NVM_READ_FAILED Something went wrong reading from NVM
  @retval CTS_NVM_PAGE_INVALID Page retreived from NVM is invalid
  @retval CTS_NVM_OK Configuration was succesfully retreived

******************************************************************* */
cts_nvm_ret_t CTS_NVM_Load(uint8_t *destBuff, uint32_t length)
{

  cts_nvm_frame_t curFrame = ctsFrame;
  cts_nvm_ret_t result = CTS_NVM_OK;

  //read current frame into local copy
  if( HW_NVM_READ_BLOCK(  CTS_NVM_BASE_ADDRESS,
                          (uint8_t *)&curFrame,
                          sizeof(cts_nvm_frame_t)) != HW_NVM_OK )
  { //if read of current frame failed
    result = CTS_NVM_READ_FAILED;
  }
  else
  {
    //validate frame
    if( CTS_NVM_ValidatePage(&curFrame, length) == CTS_NVM_OK )
    { //if frame is valid, extract and return configuration
      memcpy((void *)destBuff,(void *)&curFrame.data,length);
    }
    else
    { //if frame invalid, indicate in result
      result = CTS_NVM_PAGE_INVALID;
    }

  }

  //return result
  return result;

}

/** ******************************************************************
  @brief Check CTS frame for inconsistencies.

  @param frame pointer to the frame to check
  @param dataLength length of the data in the frame, in bytes.
                    this should be equal to the currently stored data
                    length

  @retval CTS_NVM_PAGE_INVALID Page pointed to is invalid
  @retval CTS_NVM_OK Page pointed to is valid

******************************************************************* */
static cts_nvm_ret_t CTS_NVM_ValidatePage(cts_nvm_frame_t *frame, uint32_t dataLength)
{

  cts_nvm_ret_t result = CTS_NVM_OK;
  uint32_t tmpCRC = frame->head.headCRC;
  //clear header CRC field (header CRC value cannot be included in CRC calc)
  frame->head.headCRC = 0;

  //TODO: this if statement if frkn ugly
  if( tmpCRC != CTS_NVM_GetCRC8((uint8_t *)& frame->head, sizeof(cts_nvm_head_t)) )
  { //if header is invalid, indicate in return
      result = CTS_NVM_PAGE_INVALID;
  }

  if( frame->head.dataSize != dataLength )
  { //if header is invalid, indicate in return
      result = CTS_NVM_PAGE_INVALID;
  }

  if( frame->head.dataCRC !=  CTS_NVM_GetCRC8(frame->data, frame->head.dataSize) )
  { //if header is invalid, indicate in return
    result = CTS_NVM_PAGE_INVALID;
  }

  //return result
  return result;

}

/** ******************************************************************
  @brief Clear current NVM storage, and store new page
  @details

  Writes a CTS NVM page to the NVM without retreiving and checking
  existing data in the NVM. Makes sure the page is stored valid so
  that a next CTS_NVM_Store operation succeeds, given that the NVM
  doesnt cause any errors.

  @param dataBuff   structure to store in default frame
  @param dataLength size of the structure to store on a
                    CTS_NVM_Store command, used for read validation.

  @retval CTS_NVM_WRITE_FAILED Something went wrong writing to NVM
  @retval CTS_NVM_PAGE_INVALID Default page readback failed.
  @retval CTS_NVM_OK Configuration was succesfully stored
******************************************************************* */
cts_nvm_ret_t CTS_NVM_ForceStore(uint8_t *dataBuff, uint32_t dataLength)
{
  cts_nvm_frame_t newFrame;
  cts_nvm_ret_t result = CTS_NVM_OK;

  //build default header
  newFrame.head.writeCount = 1;
  newFrame.head.dataSize = dataLength;
  newFrame.head.dataCRC = CTS_NVM_GetCRC8(dataBuff,dataLength);
  newFrame.head.headCRC = 0; //clear for CRC calculation
  newFrame.head.headCRC = CTS_NVM_GetCRC8((uint8_t *)&newFrame.head,sizeof(cts_nvm_head_t));

  //copy data for new frame
  memcpy((void *)&newFrame.data[0],(void *)dataBuff,dataLength);

  //copy to NVM
  if( HW_NVM_WRITE_BLOCK( CTS_NVM_BASE_ADDRESS,
                          (uint8_t *)&newFrame,
                          sizeof(cts_nvm_frame_t)) != HW_NVM_OK )
  { //if writing to NVM failed
    DEBUG_LINE("CTS:: NVM Write Failed");
    result = CTS_NVM_WRITE_FAILED;
  }
  else if( HW_NVM_COMPARE( CTS_NVM_BASE_ADDRESS,
                           (uint8_t *)&newFrame,
                           sizeof(cts_nvm_frame_t) ) != HW_NVM_OK )
  { //if data in memory is not equal to data that was written
    DEBUG_LINE("CTS:: NVM Page Invalid");
    result = CTS_NVM_PAGE_INVALID;
  }

  //return result
  return result;

}

/** ******************************************************************
  @brief Store CTS application configuration to NVM. Performs CRC check
  @details

  Attempts to store the CTS application configuration from NVM. If this
  fails (checked by readback), it will state so in the return value.

  @param srcBuff buffer containing the structure to be stored
  @param dataLength size of the structure to store, used for read validation

  @retval CTS_NVM_STORE_FAILED Something went wrong writing to NVM
  @retval CTS_NVM_PAGE_EXPIRED NVM frame has been written to many times
  @retval CTS_NVM_PAGE_INVALID Page retreived from NVM is invalid
  @retval CTS_NVM_OK Configuration was succesfully stored

******************************************************************* */
cts_nvm_ret_t CTS_NVM_Store(uint8_t *srcBuff, uint32_t dataLength)
{
    cts_nvm_frame_t curFrame;
    cts_nvm_ret_t result = CTS_NVM_OK;

    //read current frame into local copy
    if( HW_NVM_READ_BLOCK(  CTS_NVM_BASE_ADDRESS,
                            (uint8_t *)&curFrame,
                            sizeof(cts_nvm_frame_t)) != HW_NVM_OK )
    { //if read of current frame failed
      result = CTS_NVM_READ_FAILED;
    }
    else if (CTS_NVM_ValidatePage(&curFrame, dataLength) != CTS_NVM_OK)
    {
      result = CTS_NVM_PAGE_INVALID;
    }
    else if( (curFrame.head.writeCount < CTS_NVM_MAX_CYCLES) || (CTS_NVM_MAX_CYCLES == 0) )
    { //if page has not been written too many times

      //build new header
      curFrame.head.writeCount++;
      curFrame.head.dataSize = dataLength;
      curFrame.head.dataCRC = CTS_NVM_GetCRC8(srcBuff,dataLength);
      curFrame.head.headCRC = 0; //clear for CRC calculation
      curFrame.head.headCRC = CTS_NVM_GetCRC8((uint8_t *)&curFrame.head,sizeof(cts_nvm_head_t));

      //copy data for new frame
      memcpy((void *)&curFrame.data[0],(void *)srcBuff,dataLength);

      //copy to NVM
      if( HW_NVM_WRITE_BLOCK( CTS_NVM_BASE_ADDRESS,
                              (uint8_t *)&curFrame,
                              sizeof(cts_nvm_frame_t)) != HW_NVM_OK )
      { //if writing to NVM failed
        result = CTS_NVM_WRITE_FAILED;
      }
      else if( HW_NVM_COMPARE( CTS_NVM_BASE_ADDRESS,
                               (uint8_t *)&curFrame,
                               sizeof(cts_nvm_frame_t) ) != HW_NVM_OK )
      { //if data in memory is not equal to data that was written
        result = CTS_NVM_PAGE_INVALID;
      }

    } else {  //page has expired
      result = CTS_NVM_PAGE_EXPIRED;
    }

    //return result
    return result;
}

/** ******************************************************************
  @brief 8-Bit CRC calculator for 1 byte.
  @details

  Calculates the 8-Bit CRC for 1 byte, using @p crc as the seed to
  allow for use for longer data structures. CRC polynominal is chosen
  for HD2 @ 2048+ bits datalength.

  @param crc current CRC state when calculating multiple byte CRC
  @param data data to perform CRC calculation with

  @return CRC value calculated
******************************************************************* */
static uint8_t CTS_NVM_GetByteCRC8(uint8_t crc, uint8_t data)
{
    uint8_t i;
    //divisor polynominal, HD2 for 2048+ bits
    const uint8_t poly = 0xA6;

    //calculate CRC
    crc = crc ^ data;
    for (i = 0; i < 8; i++) //for each bit position
    {
        if (crc & 0x01) //if LSB = 1
        {  //shift and XOR with polynomial
            crc = (crc >> 1) ^ poly;
        }
        else
        {   //just shift
            crc >>= 1;
        }
    }

    //return result
    return crc;
}

/** ******************************************************************
  @brief Calculates the 8-Bit CRC for @p len bytes

  @param data byte array to perform CRC calculation for/on
  @param len the number of bytes to include in the calculation

  @retval 8 bit CRC value calculated
******************************************************************* */
uint8_t CTS_NVM_GetCRC8(uint8_t *data, uint8_t len)
{
  uint8_t crc = 0;
  uint8_t i;

  //calculate CRC bytewise using helper function
  for(i = 0; i<len ; i++) //for each data byte
  {
    crc = CTS_NVM_GetByteCRC8(crc, *data++);
  }

  //return result
  return crc;
}

//END OF CTS_NVM_drv.c
