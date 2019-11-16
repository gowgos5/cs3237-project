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
#include "RpcFifo.h"
#include "Streaming.h"
#include "RpcServer.h"

// this will create a packet and insert it on the "jFifo" to be streamed out or
// saved in flash
void PacketFifo_InsertPacket(uint32_t packetId, uint32_t *buffer,
                             uint32_t numberInBuffer) {
  StreamPacketUint32(packetId, buffer, numberInBuffer);
}

// clears the packet fifo "jFifo"
void PacketFifo_Clear(void) { fifo_clear(GetStreamOutFifo()); }

// returns one if fifo is empty, zero if not empty
int PacketFifo_Empty(void) { return fifo_empty(GetStreamOutFifo()); }

// returns a uint32 from the fifo, this uint32 is destined to be streamed out
// USB or saved in flash
uint32_t PacketFifo_GetUint32(void) {
  uint32_t val;
  fifo_get32(GetStreamOutFifo(), &val);
  return val;
}
