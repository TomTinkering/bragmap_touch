/** ******************************************************************
   @file TERM_Lib.h
   @brief  TERM_Lib Header
   @details

   This library contains functions to allow for a simple terminal like
   communication with an embedded device. It can send character outputs
   over a arbritray channel, and provides a structure for command
   induced function calls

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

#ifndef TERM_LIB_H_
	#define TERM_LIB_H_

//*******************************************************************
//Definitions

/** @brief sets the maximum allowed characters per string write */
#define TERM_MAX_STRING_LENGTH  (32)

//*******************************************************************
//External Variables

//provide a function to write out a byte on the terminal here
extern void(*TERM_PutByte)(uint8_t data);

//*******************************************************************
//Function Prototypes 

/** @brief TERM_Lib initialiser. */
void TERM_Lib_Init(void(*TERM_Write_Callback)(uint8_t data));
/** @brief Puts a string out on the TERM channel */
void TERM_PutString(char *string);
/** @brief Puts a string out on the TERM channel, including newline */
void TERM_PutLine(char *string);
/** @brief  Converts a binary number into its hex representation, and sends it out */
void TERM_PutHex(uint8_t *src, uint8_t nrBytes);
/** @brief  Converts a binary number into its ascii representation, and sends it out */
void TERM_PutBin(uint32_t src, uint8_t nrBits);
/** @brief  Converts a binary number into its decimal representation, and sends it out */
void TERM_PutDecimal(int32_t val, uint8_t length);


#endif //TERM_LIB_H_

//END OF TERM_Lib.h
