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
#include "Peripherals.h"

I2C *Peripherals::mI2c1 = NULL;
I2C *Peripherals::mI2c2 = NULL;
//USBSerial *Peripherals::mUSBSerial = NULL;
MAX30101 *Peripherals::mMAX30101 = NULL;
MAX30001 *Peripherals::mMAX30001 = NULL;
BMP280 *Peripherals::mBMP280 = NULL;
HspLed *Peripherals::mHspLed = NULL;
MAX30205 *Peripherals::mMAX30205_top = NULL;
MAX30205 *Peripherals::mMAX30205_bottom = NULL;
LIS2DH *Peripherals::mLIS2DH = NULL;
Timer *Peripherals::mTimestampTimer = NULL;
S25FS512 *Peripherals::mS25FS512 = NULL;
PushButton *Peripherals::mPushButton = NULL;
BLE *Peripherals::mBLE = NULL;
MAX14720 *Peripherals::mMAX14720 = NULL;
HspBLE *Peripherals::mhspBLE = NULL;
