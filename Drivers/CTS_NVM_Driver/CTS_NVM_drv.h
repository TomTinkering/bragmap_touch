/** ******************************************************************
   @file CTS_NVM_drv.h
   @brief  CTS_NVM_drv Header
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
#include "HardwareConfig.h"

#ifndef CTS_NVM_DRV_H_
	#define CTS_NVM_DRV_H_

//*******************************************************************
//Definitions

//user suplied definitions
#define CTS_NVM_FRAME_BYTES   (512)  //choose for convenient NVM write
#define CTS_NVM_MAX_CYCLES    (0)    //Set maximum number of writes to NVM, 0 disables counting

//system calculated definitions to scale struct to flash frame size.
#define CTS_NVM_HEADER_BYTES  (sizeof(cts_nvm_head_t))
#define CTS_NVM_DATA_BYTES    (CTS_NVM_FRAME_BYTES - CTS_NVM_HEADER_BYTES)

//*******************************************************************
//Type Definitions

typedef enum cts_nvm_ret_t
{
  CTS_NVM_ERR,
  CTS_NVM_OK,
  CTS_NVM_WRITE_FAILED,
  CTS_NVM_READ_FAILED,
  CTS_NVM_CONFIG_INVALID,
  CTS_NVM_PAGE_INVALID,
  CTS_NVM_HW_INIT_FAILED,
  CTS_NVM_PAGE_EXPIRED
} cts_nvm_ret_t;

typedef struct cts_nvm_head_t
{
  uint32_t writeCount;      //number of writes to frame
  uint32_t headCRC;         //frame header CRC checksum
  uint32_t dataCRC;         //frame data CRC checksum
  uint32_t dataSize;        //number of data bytes
} cts_nvm_head_t;

/** @brief NVM frame structure, allows for convenient hardware implementation */
typedef struct cts_nvm_frame_t
{
  //frame header
  cts_nvm_head_t head;
  //data storage space (fills to end of frame size)
  uint8_t data[CTS_NVM_DATA_BYTES];
} cts_nvm_frame_t;

//*******************************************************************
//External Variables

//*******************************************************************
//Function Prototypes 
/** @brief Perform NVM initialisation */
cts_nvm_ret_t CTS_NVM_Init();
/** @brief Load CTS application configuration from NVM. Performs CRC check */
cts_nvm_ret_t CTS_NVM_Load(uint8_t *destBuff, uint32_t length);
/** @brief Store CTS application configuration to NVM. Performs CRC check */
cts_nvm_ret_t CTS_NVM_Store(uint8_t *srcBuff, uint32_t length);
/** @brief Clear NVM storage, and writes a default header. */
cts_nvm_ret_t CTS_NVM_ForceStore(uint8_t *dataBuff, uint32_t dataLength);
/** @brief Calculates the 8-Bit CRC for @p len bytes */
uint8_t CTS_NVM_GetCRC8(uint8_t *data, uint8_t len);


#endif //CTS_NVM_DRV_H_

//END OF CTS_NVM_drv.h
