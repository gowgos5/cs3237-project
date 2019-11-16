/*******************************************************************************
 * Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *******************************************************************************
 */
#ifndef _STRINGINOUT_H_
#define _STRINGINOUT_H_

#include "mbed.h"

/// indicates that a string up to a CRLF is being accumulated
#define GETLINE_WAITING 1
/// indicates that a string is being processes
#define GETLINE_PROCESSING 2
/// indicates that a CRLF string has been buffered and can be processed
#define GETLINE_DONE 3

/**
* @brief Clear the incoming USB read fifo
*/
void clearOutReadFifo(void);
/**
* @brief Block until a character can be read from the USB
* @return the character read
*/
char getch(void);
/**
* @brief Place incoming USB characters into a fifo
* @param lineBuffer buffer to place the incoming characters
* @param bufferLength length of buffer
* @return GETLINE_WAITING if still waiting for a CRLF, GETLINE_DONE
*/
int getLine(char *lineBuffer, int bufferLength);
/**
* @brief Block until a fixed number of characters has been accumulated from the
* incoming USB
* @param lineBuffer buffer to place the incoming characters
* @param maxLength length of buffer
*/
void getStringFixedLength(uint8_t *lineBuffer, int maxLength);
/**
* @brief Output a string out the USB serial port
* @param str output this str the USB channel
*/
int putStr(const char *str);
/**
* @brief Place incoming USB characters into a fifo
* @param data_IN buffer of characters
* @param len length of data
*/
int fifoIncomingChars(uint8_t data_IN[], unsigned int len);
/**
* @brief Outut an array of bytes out the USB serial port
* @param data buffer to output
* @param length length of buffer
*/
int putBytes(uint8_t *data, uint32_t length);
/**
* @brief Outut 256 byte blocks out the USB serial using writeBlock bulk
* transfers
* @param data buffer of blocks to output
* @param length length of 256-byte blocks
*/
//int putBytes256Block(uint8_t *data, int numberBlocks);

#endif // _STRINGINOUT_H_
