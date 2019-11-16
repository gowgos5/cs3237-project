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

#include "mbed.h"
#include "LIS2DH.h"
#include "StringInOut.h"
#include "StringHelper.h"
#include "Peripherals.h"

#define LIS2DH_SLAVE_ADDRESS 0x32
#define LIS2DH_REG_PART_ID   0x0F

//******************************************************************************
int LIS2DH_ReadReg(char argStrs[32][32], char replyStrs[32][32]) {
  uint8_t args[1];
  uint8_t reply[1];
  ProcessArgs(argStrs, args, sizeof(args));
  LIS2DH *lis2dh = Peripherals::lis2dh();
  lis2dh->readReg((LIS2DH::LIS2DH_REG_map_t)args[0], (char *)reply);
  FormatReply(reply, sizeof(reply), replyStrs);
  return 0;
}

//******************************************************************************
int LIS2DH_WriteReg(char argStrs[32][32], char replyStrs[32][32]) {
  uint8_t args[2];
  uint8_t reply[1];
  ProcessArgs(argStrs, args, sizeof(args));
  LIS2DH *lis2dh = Peripherals::lis2dh();
  lis2dh->writeReg((LIS2DH::LIS2DH_REG_map_t)args[0], args[1]); ///< pass in the register address
  reply[0] = 0x80;
  FormatReply(reply, sizeof(reply), replyStrs);
  return 0;
}

extern int highDataRate;
//******************************************************************************
int LIS2DH_InitStart(char argStrs[32][32], char replyStrs[32][32]) {
  uint8_t args[2];
  uint8_t reply[1];
  ProcessArgs(argStrs, args, sizeof(args));
  LIS2DH *lis2dh = Peripherals::lis2dh();
  if (args[0] >= LIS2DH_DATARATE_200HZ) {
    highDataRate = 1;
  }
  lis2dh->initStart(args[0], args[1]);
  reply[0] = 0x80;
  FormatReply(reply, sizeof(reply), replyStrs);
  return 0;
}

//******************************************************************************
int LIS2DH_Stop(char argStrs[32][32], char replyStrs[32][32]) {
  uint8_t reply[1];
  LIS2DH *lis2dh = Peripherals::lis2dh();
  lis2dh->stop();
  reply[0] = 0x80;
  FormatReply(reply, sizeof(reply), replyStrs);
  return 0;
}
