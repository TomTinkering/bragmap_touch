/** ******************************************************************
   @file RS232_drv.c
   @brief  Generic RS232 driver, interrupt/buffer based
   @details

   Interrupt based RS232 implementation. This implementation uses a
   seperate RX and TX buffer, which are filles/emptied using interrupts.
   Relies on a lower level hardware implementation (hw_RS232_drv.h)

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
#include "RS232_drv.h"
#include "hw_RS232_drv.h"

//STM library depencies
#include "stm32f0xx_usart.h"
#include "stm32f0xx_rcc.h"

//*******************************************************************
//#Defines


//*******************************************************************
//Variables

/** @brief RX buffer memory allocation */
uint8_t RS232_RX_Buffer_Mem[RS232_RX_FIFO_BUFF_SIZE];
/** @brief TX buffer memory allocation */
uint8_t RS232_TX_Buffer_Mem[RS232_TX_FIFO_BUFF_SIZE];
/** @brief RX buffer administration */
rs232_ring_buff_t RS232_RX_Buffer;
/** @brief TX buffer memory allocation */
rs232_ring_buff_t RS232_TX_Buffer;

//*******************************************************************
//Functions

/** *******************************************************************
 * @brief   Configures and activates the UART peripheral.
 * @details
 *
 * Initialises the RX/TX buffers, UART Peripheral, and the RX/TX interrupts.
 * After all is set, the hardware is enabled.
 *
 * @retval  RS232_OK Init went ok, else, see ::rs232_err_t
 *
***********************************************************************/
rs232_err_t RS232_Init(void)
{
    //apply settings and store result
    rs232_err_t result;

    //Initialise RX buffer by setting head&tail to 0, linking buffer
    //memory and setting size and packet count.
    RS232_RX_Buffer.pBuffer = &RS232_RX_Buffer_Mem[0];
    RS232_RX_Buffer.headPos = 0;
    RS232_RX_Buffer.tailPos = 0;
    RS232_RX_Buffer.size    = RS232_RX_FIFO_BUFF_SIZE;
    RS232_RX_Buffer.packetCount = 0;

    //Initialise TX buffer by setting head&tail to 0, linking buffer
    //memory and setting size and packet count.
    RS232_TX_Buffer.pBuffer = &RS232_TX_Buffer_Mem[0];
    RS232_TX_Buffer.headPos = 0;
    RS232_TX_Buffer.tailPos = 0;
    RS232_TX_Buffer.size    = RS232_TX_FIFO_BUFF_SIZE;
    RS232_TX_Buffer.packetCount = 0;

    //Initialise hardware to default settings
    HW_RS232_INIT();

    //set default baudrate/databits/parity/stopbits
    result = RS232_Config(  9600, 8, 'N', 1 );

    //Setup UART interrrupts
    HW_RS232_SETUP_INTERRUPTS(&_RS232_RXInterrupt,&_RS232_TXInterrupt);

    //Enable UART
    HW_RS232_ENABLE();

    //return result
    return result;
}


/** *******************************************************************
 * @brief   Configures the UART peripheral settings
 * @details
 * Sets the baudrate, number of databits, type of parity and the number of
 * stopbits.
 *
 * @note
 * In case one of the inputs is invalid, the function returns with a
 * corresponding error. Some, or all of the other settings might be set depending
 * on which setting failed.
 *
 * @param BaudRate buad rate to be set in bits/s (2400,4800,9600.. up to 115200)
 * @param DataBits The number of bits per data packet (8 or 9)
 * @param Parity Parity type used. N=none, E=even, O=odd;
 * @param StopBits number of stopbits used (1 or 2)
 *
 * @retval  RS232_INVALID_DATA_SIZE Datasize not supported
 * @retval  RS232_INVALID_PARITY Given parity type not supported
 * @retval  RS232_INVALID_STOPBITS Number of stopbits not supported
 *
***********************************************************************/
rs232_err_t RS232_Config(uint32_t BaudRate, uint8_t DataBits, char Parity, uint8_t StopBits)
{

  //setup Baud-Rate
  HW_RS232_SET_BAUDRATE(0, BaudRate);

  //setup number of data bits
  switch(DataBits)
  {
    case 8: //8 databits per frame
      HW_RS232_SET_DATABITS_8();
      break;
    case 9: //9 databits per frame
      HW_RS232_SET_DATABITS_9();
      break;
    default: //return error
      return RS232_INVALID_DATA_SIZE;
  }

  //setup parity type
  switch(Parity)
  {
    case 'E': //Even parity
      HW_RS232_SET_PARITY_EVEN();
      break;
    case 'O': //Odd parity
      HW_RS232_SET_PARITY_ODD();
      break;
    case 'N': //No parity
      HW_RS232_SET_PARITY_NONE();
      break;
    default: //return error
      return RS232_INVALID_PARITY;
  }

  //setup number of stop bits
  switch(StopBits)
  {
    case 1: //1 stop bit
      HW_RS232_SET_STOPBITS_1();
      break;
    case 2: //2 stop bits
      HW_RS232_SET_STOPBITS_2();
      break;
    default: //return error
      return  RS232_INVALID_STOPBITS;
  }

  //return result
  return RS232_OK;
}


/** *******************************************************************
 * @brief   Get a byte from the RS232 receive buffer
 * @retval  The data taken from the receive buffer
***********************************************************************/
uint8_t RS232_GetByte(void)
{
  uint32_t nrPackets;
  hw_RS232_INT_t RX_Int_State = HW_GET_RX_INTERRUPT_STATE();
  uint8_t result = 0xFF;

  // stop the Rx'er and get packet count (no guarantee reading the
  // packet count is an atomic operation..)
  HW_SET_INTERRUPT_STATE(HW_RS232_RX_INTERRUPT,HW_INT_DISABLED);
  nrPackets = RS232_RX_Buffer.packetCount;
  HW_SET_INTERRUPT_STATE(HW_RS232_RX_INTERRUPT,RX_Int_State);

  if(nrPackets)
  {
    //get byte from buffer and increment tail position. Since tailpos
    //is not accessed in the interrupt, this can be safely done.
    result = RS232_RX_Buffer.pBuffer[RS232_RX_Buffer.tailPos];
    RS232_RX_Buffer.tailPos ++;
    RS232_RX_Buffer.tailPos &= RS232_RX_BUFF_MASK;

    //update packet count. This is a non atomic operation, so interrupts
    //must be disabled
    HW_SET_INTERRUPT_STATE(HW_RS232_RX_INTERRUPT,HW_INT_DISABLED);
    RS232_RX_Buffer.packetCount --;
    HW_SET_INTERRUPT_STATE(HW_RS232_RX_INTERRUPT,RX_Int_State);

  }

  //return result
  return result;
}


/** *******************************************************************
 * @brief   Write a byte to the RS232 TX buffer
 *
 * @param   data data to write to the RS232 TX buffer
 *
 * @retval  RS232_OK Byte was written to the buffer
 * @retval  RS232_BUFFER_FULL No more space was available in the buffer
 *
***********************************************************************/
rs232_err_t RS232_PutByte(uint8_t data)
{

  uint32_t remaining;
  rs232_err_t result = RS232_BUFFER_FULL;
  hw_RS232_INT_t TX_Int_State = HW_GET_TX_INTERRUPT_STATE();

  //Find amount of free space in the buffer, this is non atomic and volatile
  //so interrupts must be disabled
  HW_SET_INTERRUPT_STATE(HW_RS232_TX_INTERRUPT,HW_INT_DISABLED);
  remaining = RS232_TX_Buffer.size - RS232_TX_Buffer.packetCount;
  HW_SET_INTERRUPT_STATE(HW_RS232_TX_INTERRUPT,TX_Int_State);

  //if there is room in the buffer
  if(remaining)
  {
    //Store the data
    RS232_TX_Buffer.pBuffer[RS232_TX_Buffer.headPos] = data;
    //Update the head position, no need to disable interrupts
    RS232_TX_Buffer.headPos++;
    RS232_TX_Buffer.headPos &= RS232_TX_BUFF_MASK;

    //update packet count. This is a non atomic operation on a volatile
    //variable, so interrupts must be disabled. Since data is added to the
    //buffer, the TX interrupt can be enabled afterwards
    HW_SET_INTERRUPT_STATE(HW_RS232_TX_INTERRUPT,HW_INT_DISABLED);
    RS232_TX_Buffer.packetCount ++;
    HW_SET_INTERRUPT_STATE(HW_RS232_TX_INTERRUPT,HW_INT_ENABLED);

    //update result
    result = RS232_OK;
  }

  return result;
}


/** *******************************************************************
 * @brief   Get a block of bytes form the RS232 RX buffer
 *
 * @param   BlockBuffer the memory to which the read byte from the RX buffer should
 *          be written to. Should have at least @p Count bytes of space!
 * @param   Count the number of bytes to get from the RX buffer
 *
 * @retval  RS232_NOT_ENOUGH_ELEMENTS Less than @p Count bytes available,
 *          nothing is read
 * @retval  RS232_OK Succes, @p Count bytes were read from @p BlockBuffer
 *
***********************************************************************/
rs232_err_t RS232_GetByteBlock(uint8_t* BlockBuffer, uint32_t Count)
{

  uint32_t nrPackets,i;
  hw_RS232_INT_t RX_Int_State = HW_GET_RX_INTERRUPT_STATE();

  // stop the Rx'er and get packet count (no guarantee reading the
  // packet count is an atomic operation..)
  HW_SET_INTERRUPT_STATE(HW_RS232_RX_INTERRUPT,HW_INT_DISABLED);
  nrPackets = RS232_RX_Buffer.packetCount;
  HW_SET_INTERRUPT_STATE(HW_RS232_RX_INTERRUPT,RX_Int_State);

  //check for the posibility of over-reading the buffer
  if(Count > nrPackets){
    return RS232_NOT_ENOUGH_ELEMENTS;
  }

  //for each byte in cnt, copy source to destination
  for (i=0; i<Count; i++){
    //copy data
    BlockBuffer[i] = RS232_RX_Buffer.pBuffer[RS232_RX_Buffer.tailPos];
    //update tail position, no need to disable interrupts
    RS232_RX_Buffer.tailPos ++;
    RS232_RX_Buffer.tailPos &= RS232_RX_BUFF_MASK;
    //update destination pointer
  }

  //update packet count
  RX_Int_State = HW_GET_RX_INTERRUPT_STATE();
  HW_SET_INTERRUPT_STATE(HW_RS232_RX_INTERRUPT,HW_INT_DISABLED);
  RS232_RX_Buffer.packetCount -= Count;
  HW_SET_INTERRUPT_STATE(HW_RS232_RX_INTERRUPT,RX_Int_State);

  //return result
  return RS232_OK;
}

/** *******************************************************************
 * @brief   Write a block of bytes to the RS232 TX buffer
 *
 * @param   DataBlock the array of bytes to be added to the TX buffer. Should
 *          have at least @p Count elements!
 * @param   Count the number of bytes to write to the TX buffer
 *
 * @retval  RS232_NOT_ENOUGH_SPACE Less than @p Count bytes of space in TX buffer,
 *          nothing is written
 * @retval  RS232_OK Succes, @p Count bytes were written to the TX buffer.
 *
***********************************************************************/
rs232_err_t RS232_PutByteBlock(uint8_t* DataBlock, uint32_t Count)
{

  uint32_t i, remaining;
  hw_RS232_INT_t TX_Int_State = HW_GET_TX_INTERRUPT_STATE();

  //calculate space in buffer
  HW_SET_INTERRUPT_STATE(HW_RS232_TX_INTERRUPT,HW_INT_DISABLED);
  remaining = RS232_TX_Buffer.size - RS232_TX_Buffer.packetCount;
  HW_SET_INTERRUPT_STATE(HW_RS232_TX_INTERRUPT,TX_Int_State);

  if(remaining < Count){
    return RS232_NOT_ENOUGH_SPACE;
  }

  //for each byte in the source, store in TX buffer
  for(i=0;i<Count;i++)
  {
    //Store data in TX buffer
    RS232_TX_Buffer.pBuffer[RS232_TX_Buffer.headPos] = DataBlock[i];
    //Update head pointer. No need to disable interrupts
    RS232_TX_Buffer.headPos ++;
    RS232_TX_Buffer.headPos &= RS232_TX_BUFF_MASK;
  }

  //update packet count, and enable interrupt, since data was inserted
  HW_SET_INTERRUPT_STATE(HW_RS232_TX_INTERRUPT,HW_INT_DISABLED);
  RS232_TX_Buffer.packetCount += Count;
  HW_SET_INTERRUPT_STATE(HW_RS232_TX_INTERRUPT,HW_INT_ENABLED);

  return RS232_OK;
}


/** *******************************************************************
 * @brief   Write a string to the RS232 TX buffer
 *
 * @param   String the array of characters (string) to be added to the TX buffer.
 *          The string should '0' terminate!
 *
 * @retval  RS232_NOT_ENOUGH_SPACE Less bytes than string length in TX buffer,
 *          nothing is written
 * @retval  RS232_OK Succes, String was written to the TX buffer
 *
***********************************************************************/
rs232_err_t RS232_PutString(char *String)
{
  uint32_t i;
  rs232_err_t result = RS232_NOT_ENOUGH_SPACE;

  //find string length. Strings longer than the buffer will not be
  //sent in any case, so no sense in checking for them
  for(i=0;i<RS232_TX_Buffer.size;i++)
  {
    if(String[i] == 0) //if end of string
    {
      //send out data block with given string length
      result = RS232_PutByteBlock((uint8_t *)String, i);
      break;
    }
  }

  //return result
  return result;
}


/** *******************************************************************
 * @brief   Clear RS232 peripheral error state, use sparingly
 ***********************************************************************/
void RS232_ClearErrors()
{
   HW_RS232_CLEAR_ERRORS();
}

/** *******************************************************************
 * @brief   Get the number of waiting bytes in the RX buffer
 * @retval  Number of bytes waiting in the RX buffer
***********************************************************************/
uint32_t RS232_GetRXPacketCount()
{
  uint32_t result;
  hw_RS232_INT_t RX_Int_State = HW_GET_RX_INTERRUPT_STATE();
  //get number of packets in buffer, and restore interrupt state
  HW_SET_INTERRUPT_STATE(HW_RS232_RX_INTERRUPT,HW_INT_DISABLED);
  result = RS232_RX_Buffer.packetCount;
  HW_SET_INTERRUPT_STATE(HW_RS232_RX_INTERRUPT,RX_Int_State);

  //return result
  return result;
}


/** *******************************************************************
 * @brief   Get the number of waiting bytes in the TX buffer
 * @retval  Number of bytes waiting in the TX buffer
***********************************************************************/
uint32_t RS232_GetTXPacketCount()
{
  uint32_t result;
  hw_RS232_INT_t TX_Int_State = HW_GET_TX_INTERRUPT_STATE();
  //get number of packets in buffer, and restore interrupt state
  HW_SET_INTERRUPT_STATE(HW_RS232_TX_INTERRUPT,HW_INT_DISABLED);
  result = RS232_TX_Buffer.packetCount;
  HW_SET_INTERRUPT_STATE(HW_RS232_TX_INTERRUPT,TX_Int_State);

  //return result
  return result;
}

/** *******************************************************************
 * @brief   Get maximum size of the TX buffer
 * @retval  Maximum size of the TX buffer
***********************************************************************/
uint32_t RS232_GetTXBufferSize()
{
  return RS232_TX_Buffer.size;
}

/** *******************************************************************
 * @brief   Get maximum size of the RX buffer
 * @retval  Maximum size of the RX buffer
***********************************************************************/
uint32_t RS232_GetRXBufferSize()
{
  return RS232_RX_Buffer.size;
}

/** *******************************************************************
 * @brief   Get the number of free slots in the TX buffer
 * @retval  Number of free slots in the TX buffer
***********************************************************************/
uint32_t RS232_GetTXBufferSpace()
{
  uint32_t result;
  hw_RS232_INT_t TX_Int_State = HW_GET_TX_INTERRUPT_STATE();
  //get number of packets in buffer, and restore interrupt state
  HW_SET_INTERRUPT_STATE(HW_RS232_TX_INTERRUPT,HW_INT_DISABLED);
  result = RS232_TX_Buffer.size - RS232_TX_Buffer.packetCount;
  HW_SET_INTERRUPT_STATE(HW_RS232_TX_INTERRUPT,TX_Int_State);

  return result;
}

/** *******************************************************************
 * @brief   Get the number of free slots in the TX buffer
 * @retval  Number of free slots in the TX buffer
***********************************************************************/
uint32_t RS232_GetRXBufferSpace()
{
  uint32_t result;
  hw_RS232_INT_t RX_Int_State = HW_GET_RX_INTERRUPT_STATE();
  //get number of packets in buffer, and restore interrupt state
  HW_SET_INTERRUPT_STATE(HW_RS232_RX_INTERRUPT,HW_INT_DISABLED);
  result = RS232_RX_Buffer.size - RS232_RX_Buffer.packetCount;
  HW_SET_INTERRUPT_STATE(HW_RS232_RX_INTERRUPT,RX_Int_State);

  return result;
}


/** *******************************************************************
 * @brief  RX interrupt callback function
 * @details
 *
 * This function is called by a lower layer interrupt handler. It takes care
 * of filling the RS232 RX buffer upon receiving a new byte. If the buffer is
 * full, the data is discarded
***********************************************************************/
void _RS232_RXInterrupt(uint8_t TheData)
{

  //if there is room in the buffer, store the data. The Buffer cannot
  //be fully filled since that would cause the tail and head positions
  //to overlap. This is seen as an empty buffer.
  if(RS232_RX_Buffer.packetCount < RS232_RX_FIFO_BUFF_SIZE)
  {
    //Store Data
    RS232_RX_Buffer.pBuffer[RS232_RX_Buffer.headPos] = TheData;
    //Update head position
    RS232_RX_Buffer.headPos ++;
    RS232_RX_Buffer.headPos &= RS232_RX_BUFF_MASK;
    //Increment packet count
    RS232_RX_Buffer.packetCount ++;
    // Update system RX counter status information
    //PBPStatusInformation.UART_Rx_Counter[UPT_RS232_Port1]++;
  }

}

/** *******************************************************************
 * @brief  TX interrupt callback function
 * @details
 *
 * This function is called by a lower layer interrupt handler. It takes care
 * of emtying the RS232 TX buffer when the peripheral is available. If the
 * buffer is empty, the interrupt is disabled.
***********************************************************************/
void _RS232_TXInterrupt(void)
{

  // Any bytes waiting to be sent ?
  if(RS232_TX_Buffer.packetCount)
  {

    //send the byte over the UART
    HW_RS232_SEND_BYTE(RS232_TX_Buffer.pBuffer[RS232_TX_Buffer.tailPos]);
    //Update buffer tail position
    RS232_TX_Buffer.tailPos ++;
    RS232_TX_Buffer.tailPos &= RS232_TX_BUFF_MASK;
    //Decrement packet count
    RS232_TX_Buffer.packetCount --;

    //PBPStatusInformation.UART_Tx_Counter[UPT_RS232_Port2]++;
  }
  else
  {
    // Disable the TX interrupt if we are done so that we don't
    //keep entering this ISR
    HW_SET_INTERRUPT_STATE(HW_RS232_TX_INTERRUPT,HW_INT_DISABLED);
  }

}
