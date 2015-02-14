/** ******************************************************************
   @file Global.h
   @brief  Global Settings/Definitions file
   @details

   This file should be included by all the files in the project. It
   contains all the global settings, definitions, includes and API's.

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
  //Global Includes
  #include <stdint.h>
  #include "TERM_Lib.h"

#ifndef __GLOBALLIB_H
  #define __GLOBALLIB_H

  //*******************************************************************
  //Includes

  //*******************************************************************
  //#Definitions

  //DEBUG DEFINITIONS

  //Un-comment next line to enable global debugging facilities
  #define __DEBUG (0)

  //debug macro definitions
  #ifdef __DEBUG
    //call appropriate function to allow debugging
    #define DEBUG_STRING(STRING)    TERM_PutString(STRING)
    #define DEBUG_LINE(LINE)        TERM_PutLine(LINE)
    #define DEBUG_BIN(VAL,CNT)      TERM_PutBin(VAL,CNT)
    #define DEBUG_HEX(P_VAL,CNT)    TERM_PutHex(P_VAL,CNT)
    #define DEBUG_DECIMAL(VAL,LEN)  TERM_PutDecimal(VAL,LEN)
  #else
    //use empty definitions. This avoids having to write #ifdef etc..
    //every time a possible debug output is desireable
    //call appropriate function to allow debugging
    #define DEBUG_STRING(STRING)    do{}while(0)
    #define DEBUG_LINE(LINE)        do{}while(0)
    #define DEBUG_HEX(P_VAL,CNT)    do{}while(0)
    #define DEBUG_BIN(VAL,CNT)      do{}while(0)
    #define DEBUG_DECIMAL(VAL,LEN)  do{}while(0)
  #endif


  //set/reset value to toggle stm32f0 board led's
  #define BSRR_VAL        0x0300

  //*******************************************************************
  //Typedefs

  //*******************************************************************
  //External Variables

  //*******************************************************************
  //Function Prototypes

  void TERM_PutByte_Helper(uint8_t data);

#endif
