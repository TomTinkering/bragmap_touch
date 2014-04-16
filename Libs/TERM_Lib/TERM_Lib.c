/** ******************************************************************
   @file TERM_Lib.c
   @brief  TERM_Lib Source
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
#include "TERM_Lib.h"

//*******************************************************************
//Definitions

#define TERM_MAX_U2D_LEN (9)

//*******************************************************************
//Variables

void(*TERM_PutByte)(uint8_t data);

//array of powers of 10, to avoid multiplications
uint32_t g_pwrTen[] = {  1,
						             10,
                         100,
                         1000,
                         10000,
                         100000,
                         1000000,
                         10000000,
                         100000000,
                         1000000000,
                       };

//*******************************************************************
//Helper Function Prototypes (declare static!)

static void TERM_U2D_Helper(uint32_t val, uint8_t length, char *strBuff);
static void TERM_PutHex_Helper(uint8_t hex);

//*******************************************************************
//Function Implementations


/** ******************************************************************
	@brief TERM_Lib initialiser. 
 	@details 

	TERM_Lib initialiser. Call this function to prepare this module
	for use

	@param TERM_Write_Callback Callback function used to register a
	       function that writes out a byte to the desired channel

******************************************************************* */
void TERM_Lib_Init(void(*TERM_Write_Callback)(uint8_t data))
{
  //register TERM_Write_Callback
  TERM_PutByte = TERM_Write_Callback;
}


/** ******************************************************************
  @brief Puts a string out on the TERM channel
  @param string The string to send. Must be 0 terminated!
******************************************************************* */
void TERM_PutString(char *string)
{
  uint8_t i=0;
  //for a maximum of TERM_MAX_STRING_LENGTH, write out string
  for(i=0;i<TERM_MAX_STRING_LENGTH;i++)
  {
    //break at end of string
    if(string[i] == 0){break;}
    //write string character to output terminal
    TERM_PutByte((uint8_t)string[i]);
  }
}

/** ******************************************************************
  @brief Puts a string out on the TERM channel, including newline
  @param string The string to send. Must be 0 terminated!
******************************************************************* */
void TERM_PutLine(char *string)
{
  TERM_PutString(string);
  TERM_PutString("\n\r");
}

/** ******************************************************************
  @brief Converts a single byte into its hex representation, and sends it out
  @param hex The byte to convert to hex representation format
******************************************************************* */
static void TERM_PutHex_Helper(uint8_t hex)
{
  const char HEX_LOOKUP[] = {
     '0', '1', '2', '3', '4', '5', '6', '7',
     '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

  TERM_PutByte((uint8_t)HEX_LOOKUP[hex >> 4]);
  TERM_PutByte((uint8_t)HEX_LOOKUP[hex & 0x0f]);
}

/** ******************************************************************
  @brief  Converts a binary number into its ascii binary representation
  @details

  Treats a integer as a list 0's and 1's, and outputs these over the
  output interface.

  @param src integer number to convert
  @param nrBits number of bit positions to convert, starting from 0

  Example Usage:
  @code
    uint32_t value = 0b111000111;
    TERM_PutBin(value, sizeof(value)); //this gives 0b0000000000000000000111000111
    TERM_PutBin(value, 1); //this gives 0b1
    TERM_PutBin(value, 4); //this gives 0b0111
  @endcode
******************************************************************* */
void TERM_PutBin(uint32_t src, uint8_t nrBits)
{
 uint8_t i=0;
 char curBit = '0';
 //for a maximum of TERM_MAX_STRING_LENGTH, write out string
 TERM_PutByte((uint8_t)'0');
 TERM_PutByte((uint8_t)'b');
 for(i=nrBits;i-->0;) //reverse loop i= NrBytes-1 -> 0
 {
     //determine character for this bit
     curBit = ((src & (1<<i)) != 0) ? '1' : '0';
     //output but over interface
     TERM_PutByte((uint8_t)curBit);
 }

}


/** ******************************************************************
  @brief  Converts a binary number into its hex representation,
          and sends it out
  @details

  Treats a number as an array of unsigned bytes, of which @p nrBytes
  LSBs are converted to HEX format, starting with the MSB of the LSBs.
  This is then sent out on the TERM channel

  @param src pointer to the first byte to convert
  @param nrBytes number of bytes to convert

  Example Usage:
  @code
    uint16_t value = 0x1234;
    TERM_PutHex((uint8_t *)&value, sizeof(value)); //this gives 0x1234
    TERM_PutHex((uint8_t *)&value, 1); //this gives 0x34
    TERM_PutHex((uint8_t *)&value, 3); //this gives 0x??1234
  @endcode
******************************************************************* */
void TERM_PutHex(uint8_t *src, uint8_t nrBytes)
{
 uint8_t i=0;
 //for a maximum of TERM_MAX_STRING_LENGTH, write out string
 TERM_PutByte((uint8_t)'0');
 TERM_PutByte((uint8_t)'x');
 for(i=nrBytes;i-->0;) //reverse loop i= NrBytes-1 -> 0
 {
   //write string character to output terminal
   TERM_PutHex_Helper(src[i]);
 }

}

/** ******************************************************************
  @brief  Converts a binary number into its decimal representation,
          and sends it out
  @details

  Converts a binary number into a @p length number of digits decimal
  representation. If the source value had more decimal digits, only
  the bottom @p length digits are given. After conversion the number
  is put onto the TERM channel.

  @param val binary number to convert into a decimal output
  @param length the number of decimals to show in the output
******************************************************************* */
void TERM_PutDecimal(int32_t val, uint8_t length)
{
  //allocate string buffer memory
  char strBuff[length+2];
  strBuff[length+1] = 0; //terminate string

  //determine sign, and string representation of value
  if(val<0){   //negative number
    strBuff[0]='-';
    TERM_U2D_Helper(-val,length,&strBuff[1]);
  }
  else         //positive number
  {
    strBuff[0]=' ';
    TERM_U2D_Helper( val,length,&strBuff[1]);
  }

  //put string out on TERM channel
  TERM_PutString(&strBuff[0]);
}

/** ******************************************************************
  @brief Converts a binary number into its decimal representation
  @details

  Converts a binary number into a @p length number of digits decimal
  representation. If the source value had more decimal digits, only
  the bottom @p length digits are given.

  @param val binary number to convert into a decimal output
  @param length the number of decimals to show in the output, max 9
  @param strBuff storage for the output string, should be >= @p length
******************************************************************* */
static void TERM_U2D_Helper(uint32_t val, uint8_t length, char *strBuff)
{

   uint8_t i;
   //cap at maximum decimal value to convert
   uint8_t len = (length <= TERM_MAX_U2D_LEN) ? length : TERM_MAX_U2D_LEN;
   uint32_t decVal = g_pwrTen[len-1];

   //for each disirable digit
   for(i=0; i<len; i++)
   {

     //get current digit value and string representation
     uint8_t dgt = val / decVal;
     dgt = (dgt < 10) ? dgt : 10;
     strBuff[i] = (char)(dgt + (uint8_t)'0');

     //prepare for next digit
     val = val % decVal;
     decVal = decVal / 10;

   }

}

//END OF TERM_Lib.c
