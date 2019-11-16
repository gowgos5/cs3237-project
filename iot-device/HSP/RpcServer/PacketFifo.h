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
#ifndef _PACKETFIFO_H_
#define _PACKETFIFO_H_

#include "mbed.h"

/**
* this will create a packet and insert it into an outbound fifo to be streamed out or saved in flash
* @param packetId number id to assign to this packet
* @param buffer a 32-bit buffer that contains data that will be used in the packet
* @param numberInBuffer the number of 32-bit elements to be copied from the buffer
*/
void PacketFifo_InsertPacket(uint32_t packetId, uint32_t *buffer, uint32_t numberInBuffer);

/**
* clears the packet outbound fifo
*/
void PacketFifo_Clear(void);

/**
* returns one if outbound fifo is empty, zero if not empty
*/
uint32_t PacketFifo_Empty(void);

/** 
* returns a uint32 from the fifo, this uint32 is destined to be streamed out USB or saved in flash
*/
uint32_t PacketFifo_GetUint32(void);

#endif /* _PACKETFIFO_H_ */
