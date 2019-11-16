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
#ifndef _STREAMING_H_
#define _STREAMING_H_

#include "mbed.h"

#define PACKET_LIS2DH 0x20

#define PACKET_MAX30205_TEMP_TOP 0x40
#define PACKET_MAX30205_TEMP_BOTTOM 0x50
#define PACKET_BMP280_PRESSURE 0x60
#define PACKET_LSM6DS3_ACCEL 0x70
#define PACKET_MAX30205_TEMP 0x80
#define PACKET_NOP 0x90

/**
* @brief Creates a packet that will be streamed via USB or saved into flash
* datalog memory
* @brief the packet created will be inserted into a fifo to be streamed at a
* later time
* @param id Streaming ID
* @param buffer Pointer to a uint32 array that contains the data to include in
* the packet
* @param number Number of elements in the buffer
*/
void StreamPacketUint32(uint32_t id, uint32_t *buffer, uint32_t number);
/**
* @brief Insert a buffer into the out going fifo
* @param buffer Array of uint32 to send to the fifo
* @param len Length of the array
*/
int StreamFifoUint32Array(uint32_t buffer[], uint32_t len);
/**
* @brief Insert a timestamp into the out going fifo
*/
int StreamFifoTimeStamp(void);
/**
* @brief Insert a packet id into the out going fifo
* @param id The uint32 packet id
*/
int StreamFifoId(uint32_t id);
/**
* @brief Return a value that indicates if the system is streaming data
* @returns Returns a one or zero value
*/
uint8_t IsStreaming(void);
/**
* @brief Set a flag to indicate if streaming is enabled
* @param state A one or zero value
*/
void SetStreaming(uint8_t state);
/**
* @brief Set a flag to indicate if datalogging is enabled
* @param state A one or zero value
*/
void SetDataLoggingStream(uint8_t state);
/**
* @brief Insert a length value into the out going fifo
* @param length A uint32 number representing a length
*/
int StreamFifoLength(uint32_t length);

#endif // _STREAMING_H_ 
