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
#include <stdint.h>
#include "StringHelper.h"
#include "I2C_RPC.h"
#include "Peripherals.h"

//******************************************************************************
// input
// [instance] [slaveAddress] [writeNumber] [](data to write) [readNumber]
// output data to read
int I2C_WriteRead(char argStrs[32][32], char replyStrs[32][32]) {
  uint8_t args[16];
  uint8_t reply[16];
  uint8_t writeNumber;
  uint8_t readNumber;
  uint8_t instance;
  uint8_t slaveAddress;
  int ret1;
  int ret2;
  I2C *i2c;
  // get the number of bytes to write
  ProcessArgs(argStrs, args, 3); // parse [instance] [slaveAddress] [writeNumber]
  instance = args[0];
  slaveAddress = args[1];
  writeNumber = args[2];
  // parse [instance] [slaveAddress] [writeNumber] [](data to write)
  // [readNumber]
  ProcessArgs(argStrs, args, writeNumber + 4);
  readNumber = args[writeNumber + 3];

  i2c = Peripherals::i2c1();
  if (instance == 2) {
    i2c = Peripherals::i2c2();
  }
  ret1 = 0;
  ret2 = 0;

  if (writeNumber != 0) {
    ret1 = i2c->write((int)slaveAddress, (char *)&args[3], (int)writeNumber);
  }
  if (readNumber != 0) {
    ret2 = i2c->read((int)slaveAddress, (char *)reply, (int)readNumber);
  }

	if (ret1 != 0) reply[0] = 0xFF;
	if (ret2 != 0) reply[0] = 0xFF;


  // reply[0] = 0x80;
  FormatReply(reply, readNumber, replyStrs);
  return 0;
}
