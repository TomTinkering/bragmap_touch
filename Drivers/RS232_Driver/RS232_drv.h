/** ******************************************************************
   @file RS232_drv.h
   @brief  Generic RS232 driver, interrupt/buffer based
   @details

   Interrupt based RS232 implementation. This implementation uses a
   seperate RX and TX buffer, which are filles/emptied using interrupts.
   Relies on a lower level hardware implementation (hw_RS232_drv.h)

******************************************************************* */


//*******************************************************************
//Includes
#include "Global.h"
#include "HardwareConfig.h"

#ifndef __RS232_DRV_H
  #define __RS232_DRV_H

  //*******************************************************************
  //#Definitions
  //*******************************************************************
  #define RS232_RX_FIFO_BUFF_SIZE (UART_RX_BUFFER_LENGTH)
  #define RS232_TX_FIFO_BUFF_SIZE (UART_TX_BUFFER_LENGTH)
  //buffer sizes should be a power of 2 in order for this to work!
  #define RS232_RX_BUFF_MASK      (RS232_RX_FIFO_BUFF_SIZE-1)
  #define RS232_TX_BUFF_MASK      (RS232_TX_FIFO_BUFF_SIZE-1)

  //*******************************************************************
  //Typedefs
  //*******************************************************************

  //buffer type definition, volatile because members are accessed in
  //an interrupt, some compilers don't support volatile members
  typedef volatile struct
  {
    uint8_t* pBuffer;     //pointer to start of buffer memory (DONT TOUCH)
    uint32_t headPos;     //position of buffer head
    uint32_t tailPos;     //position of buffer tail
    uint32_t size;        //max size of buffer
    uint32_t packetCount; //number of packets in buffer
  } rs232_ring_buff_t;

  typedef struct
  {
    uint8_t Enabled;
    uint32_t BaudRate;
    uint32_t DataBits;
    char UParity;
    uint32_t StopBits;
  } rs232_config_t;

  //RS232 standard return type
  typedef enum
  {
    RS232_ERR,
    RS232_OK,
    RS232_INVALID_PARITY,
    RS232_INVALID_DATA_SIZE,
    RS232_INVALID_STOPBITS,
    RS232_BUFFER_OVERRUN,
    RS232_BUFFER_FULL,
    RS232_NOT_ENOUGH_ELEMENTS,
    RS232_NOT_ENOUGH_SPACE
  } rs232_err_t;

  //*******************************************************************
  //External Variables
  //*******************************************************************


  //*******************************************************************
  //Function Prototypes

  /** @brief   Configures and activates the UART peripheral. */
  rs232_err_t RS232_Init(void);
  /** @brief   Configures the UART peripheral settings */
  rs232_err_t RS232_Config(uint32_t BaudRate, uint8_t DataBits, char Parity, uint8_t StopBits);

  /** @brief   Get a byte from the RS232 receive buffer */
  uint8_t RS232_GetByte(void);
  /** @brief   Write a byte to the RS232 TX buffer */
  rs232_err_t RS232_PutByte(uint8_t data);
  /** @brief   Get a block of bytes form the RS232 RX buffer */
  rs232_err_t RS232_GetByteBlock(uint8_t* BlockBuffer, uint32_t Count);
  /** @brief   Write a block of bytes to the RS232 TX buffer */
  rs232_err_t RS232_PutByteBlock(uint8_t* DataBlock, uint32_t Count);
  /** @brief   Write a string to the RS232 TX buffer */
  rs232_err_t RS232_PutString(char *String);

  /** @brief   Clear RS232 peripheral error state, use sparingly */
  void RS232_ClearErrors();

  /** @brief   Get the number of waiting bytes in the RX buffer */
  uint32_t RS232_GetRXPacketCount();
  /** @brief   Get the number of waiting bytes in the TX buffer */
  uint32_t RS232_GetTXPacketCount(void);
  /** @brief   Get maximum size of the TX buffer */
  uint32_t RS232_GetTXBufferSize(void);
  /** @brief   Get maximum size of the RX buffer */
  uint32_t RS232_GetRXBufferSize(void);
  /** @brief   Get the number of free slots in the TX buffer */
  uint32_t RS232_GetTXBufferSpace(void);
  /** @brief   Get the number of free slots in the TX buffer */
  uint32_t RS232_GetRXBufferSpace(void);
  /** @brief  RX interrupt callback function */
  void _RS232_RXInterrupt(uint8_t TheData);
  /** @brief  TX interrupt callback function*/
  void _RS232_TXInterrupt(void);

#endif








