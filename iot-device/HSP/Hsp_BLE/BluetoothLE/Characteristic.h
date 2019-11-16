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
#ifndef _CHARACTERISTIC_H_
#define _CHARACTERISTIC_H_

#include "mbed.h"
#include "BLE.h"
#include "GattCharacteristic.h"

class Characteristic {
public:
  /**
   * @brief ticker handler static method
   */
  Characteristic(uint16_t length, const UUID &uuid,
                 uint8_t additionalProperties =
                     GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NONE,
                 GattAttribute *descriptors[] = NULL,
                 unsigned numDescriptors = 0);

  void setIndex(int index) { this->index = index; }
  /**
  * @brief Set a pointer reference to the mbed BLE framework
  */
  void setBLE(BLE *ble) { this->ble = ble; }
  /**
  * @brief Get the pointer to the byte payload
  */
  uint8_t *getPayloadPtr(void) { return payload.getPointer(); }
  /**
   * @brief Get the handle of the characteristic
   */
  GattAttribute::Handle_t getValueHandle();
  /**
   * @brief Update the characteristic
   */
  void update(void);
  /**
   * @brief Get the GattCharacteristic
   */
  GattCharacteristic *getGattCharacteristic();
  /**
  * @brief Get the length of the payload
  */
  int getPayloadLength(void);

  uint8_t *getPayloadBytes(void) { return payload.bytes; }

  void copyDataWritten(const GattWriteCallbackParams *params) {
    unsigned int i = 0;
    while (i < params->len && i < sizeof(dataWritten)) {
      dataWritten[i] = params->data[i];
      i++;
    }
    dataWrittenLength = params->len;
  }

  uint8_t *getDataWritten(int *length) {
    *length = dataWrittenLength;
    return dataWritten;
  }

private:
  int payloadLength;
  struct PayloadStruct {
    uint8_t bytes[32];
    uint8_t *getPointer(void) { return bytes; }
  };
  /// local copy of data written to this characteristic
  uint8_t dataWritten[64];
  int dataWrittenLength;
  /// payload structure for this characteristic
  PayloadStruct payload;
  /// gatt characteristic
  GattCharacteristic gattCharacteristic;
  /// mbed BLE framework pointer
  BLE *ble;
  /// index of this characteristic
  int index;
};

#endif /* _CHARACTERISTIC_H_ */
