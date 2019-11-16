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
#include "Test_MAX30205.h"
#include "Test_Utilities.h"
#include "MAX30205.h"
#include "Peripherals.h"

//******************************************************************************
void test_MAX30205(MAX30205 *MAX30205, void (*outputString)(const char *)) {
  uint16_t value;
  char tempStr[32];
  int pass;

  outputString("Write Reg_THYST to 0x1234 @ 400kHz: ");
  // i2cSetSpeed(MAX30205_I2CM_INSTANCE, I2CM_SPEED_100kHz);
  MAX30205->reg_THYST_Write(0x1234);
  outputString("Read Reg_THYST @ 400kHz: ");
  // i2cSetSpeed(MAX30205_I2CM_INSTANCE, I2CM_SPEED_100kHz);
  MAX30205->reg_THYST_Read(&value);
  if (value == 0x1234)
    pass = 1;
  else
    pass = 0;
  //*totalPass &= *pass;
  sprintf(tempStr, " (%04X)|", value);
  outputString(tempStr);
  _printPassFail(pass, 0, outputString);
}
