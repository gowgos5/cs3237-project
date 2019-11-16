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
#ifndef _PERIPHERALS_H_
#define _PERIPHERALS_H_

#include "mbed.h"
//#include "USBSerial.h"
#include "MAX30101.h"
#include "HspLed.h"
#include "MAX30205.h"
#include "LIS2DH.h"
#include "MAX30001.h"
#include "BMP280.h"
#include "S25FS512.h"
#include "PushButton.h"
#include "BLE.h"
#include "MAX14720.h"
#include "HspBLE.h"

/**
* This static class is used as a central locatoin for all devices on the HSP platform
* it gives (in-effect) a singleton interface for each device so that anywhere in code
* one can reference on of these devices
*/
class Peripherals {
public:
    //static USBSerial *setUSBSerial(USBSerial * device) { mUSBSerial = device; return device; }
    //static USBSerial *usbSerial(void) { return mUSBSerial; }

    static MAX30101 *max30101(void) { return mMAX30101; }
    static MAX30101 *setMAX30101(MAX30101 *device) { mMAX30101 = device; return device; }
    
    static MAX30205 *max30205_top(void) { return mMAX30205_top; }
    static MAX30205 *setMAX30205_top(MAX30205 *device) { mMAX30205_top = device; return device; }

    static MAX30205 *max30205_bottom(void) { return mMAX30205_bottom; }
    static MAX30205 *setMAX30205_bottom(MAX30205 *device) { mMAX30205_bottom = device; return device; }

    static HspLed *hspLed(void) { return mHspLed; }
    static HspLed *setHspLed(HspLed *device) { mHspLed = device; return device; }

    static LIS2DH *lis2dh(void) { return mLIS2DH; }
    static LIS2DH *setLIS2DH(LIS2DH *device) { mLIS2DH = device; return device; }

    static MAX30001 *max30001(void) { return mMAX30001; }
    static MAX30001 *setMAX30001(MAX30001 *device) { mMAX30001 = device; return device; }

    static BMP280 *bmp280(void) { return mBMP280; }
    static BMP280 *setBMP280(BMP280 *device) { mBMP280 = device; return device; }

    static Timer *timestampTimer(void) { return mTimestampTimer; }
    static Timer *setTimestampTimer(Timer *timer) { mTimestampTimer = timer; return timer; }

    static I2C *i2c1(void) { return mI2c1; }
    static I2C *setI2c1(I2C *i2cPort) { mI2c1 = i2cPort; return i2cPort; }

    static I2C *i2c2(void) { return mI2c2; }
    static I2C *setI2c2(I2C *i2cPort) { mI2c2 = i2cPort; return i2cPort; }

    static S25FS512 *s25FS512(void) { return mS25FS512; }
    static S25FS512 *setS25FS512(S25FS512 *s25FS512) { mS25FS512 = s25FS512; return s25FS512; }

    static PushButton *pushButton(void) { return mPushButton; }
    static PushButton *setPushButton(PushButton *pushButton) { mPushButton = pushButton; return pushButton; }

    static BLE *ble(void) { return mBLE; }
    static BLE *setBLE(BLE *_ble) { mBLE = _ble; return _ble; }

    static HspBLE *hspBLE(void) { return mhspBLE; }
    static HspBLE *setHspBLE(HspBLE *_hspBLE) { mhspBLE = _hspBLE; return _hspBLE; }

    static MAX14720 *max14720(void) { return mMAX14720; }
    static MAX14720 *setMAX14720(MAX14720 *_MAX14720) { mMAX14720 = _MAX14720; return _MAX14720; }

private:
    static I2C *mI2c1;
    static I2C *mI2c2;
    //static USBSerial *mUSBSerial;
    static MAX30101 *mMAX30101;
    static MAX30001 *mMAX30001;
    static BMP280 *mBMP280;
    static HspLed *mHspLed;
    static Serial *mSerial;
    static MAX30205 *mMAX30205_top;
    static MAX30205 *mMAX30205_bottom;
    static LIS2DH *mLIS2DH;
    static Timer *mTimestampTimer;
    static S25FS512 *mS25FS512;
    static PushButton *mPushButton;
    static BLE *mBLE;
    static MAX14720 *mMAX14720;
    static HspBLE *mhspBLE;
};

#endif // _PERIPHERALS_H_

