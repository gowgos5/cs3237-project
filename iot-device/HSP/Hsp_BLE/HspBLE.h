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
#ifndef _HSPBLE_H_
#define _HSPBLE_H_

#include "mbed.h"
#include "BluetoothLE.h"

/// define the number of characteristics used on the HSP platform
#define NUMBER_OF_CHARACTERISTICS (7)

/// indicies of the characteristics used
#define CHARACTERISTIC_TEMP_TOP (0)
#define CHARACTERISTIC_TEMP_BOTTOM (1)
#define CHARACTERISTIC_ACCELEROMETER (2)
#define CHARACTERISTIC_HEARTRATE (3)
#define CHARACTERISTIC_PRESSURE (4)
#define CHARACTERISTIC_DATA (5)
#define CHARACTERISTIC_CMD (6)

class HspBLE {
public:
  HspBLE(BLE *ble);
  
  ~HspBLE(void);
  void init(void);
  static void _onDataWritten(int index);
  void onDataWritten(int index);
  void updateNotification(int index);
  void pollSensor(int sensorId, uint8_t *data);
  bool getStartDataLogging(void);
  static HspBLE *instance;
  void send(void);
  void sendHRData(uint32_t data[], uint32_t len);
  void sendAccData(uint32_t data[], uint32_t len);

private:
  void tickerHandler(void);
  void ppgHandler(void);
  void accHandler(void);
  void testPattern(uint8_t *data, int length, bool clear);
  /// ticker that is used to periodically update the characteristic
  /// notifications
  Ticker ticker;
  Ticker ticker2;

  BluetoothLE *bluetoothLE;
  /// value that controls the notifications to be sent in a round robin fashion
  int notificationUpdateRoundRobin;

  /// charateristic UUIDs statically defined
  static uint8_t temperatureTopCharUUID[];
  static uint8_t temperatureBottomCharUUID[];
  static uint8_t accelerometerCharUUID[];
  static uint8_t heartrateCharUUID[];
  static uint8_t pressureCharUUID[];
  static uint8_t dataCharUUID[];
  static uint8_t commandCharUUID[];
  static uint8_t envServiceUUID[];
  static uint8_t deviceName[];
  static uint8_t serialNumber[];

  /// application specific
  bool startDataLogging;
};

#endif // _HSPBLE_H_