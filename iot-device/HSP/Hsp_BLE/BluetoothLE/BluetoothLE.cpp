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
#include "BluetoothLE.h"
#include "Characteristic.h"

/**
 * @brief Constructor for class
 */
BluetoothLE::BluetoothLE(BLE *ble, int numberOfCharacteristics)
    : ble(ble), numberOfCharacteristics(numberOfCharacteristics), runningIndex(0)
       {
  characteristics = new Characteristic *[numberOfCharacteristics];
  gattCharacteristics = new GattCharacteristic *[numberOfCharacteristics];
}

/**
 * @brief Destructor for class
 */
BluetoothLE::~BluetoothLE(void) {
  for (int i = 0; i < numberOfCharacteristics; i++) delete characteristics[i];
  delete[] gattCharacteristics;
  delete[] characteristics;
}

/**
 * @brief Initialize the advertising, characteristics, service for this platform
 */
void BluetoothLE::addCharacteristic(Characteristic *characteristic) {
  characteristic->setBLE(ble);
  characteristic->setIndex(runningIndex);
  characteristics[runningIndex] = characteristic;
  gattCharacteristics[runningIndex] = characteristic->getGattCharacteristic();
  runningIndex++;
}

/**
 * @brief Initialize the advertising, characteristics, service for this platform
 */
void BluetoothLE::initService(uint8_t *serialNumber, uint8_t *deviceName,
                              int nameSize, uint8_t *serviceUUID) {
  ble->init();
  ble->onDisconnection(this, &BluetoothLE::disconnectionCallback);

  ble->gap().setAddress((BLEProtocol::AddressType_t)0, serialNumber);

  // Setup Advertising
  ble->accumulateAdvertisingPayload(
      GapAdvertisingData::BREDR_NOT_SUPPORTED |
      GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
  ble->accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME,
                                    (uint8_t *)deviceName, nameSize);
  ble->setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
  ble->setAdvertisingInterval(1600); // 1000ms; in multiples of 0.625ms.
  GattService envService(serviceUUID, gattCharacteristics,
                         numberOfCharacteristics);
  ble->gattServer().addService(envService);
  ble->onDataWritten(this, &BluetoothLE::onDataWritten);
  // Start Advertising
  ble->startAdvertising();
}

/**
 * @brief Called on BLE connection
 */
void BluetoothLE::connectionCallback(
    const Gap::ConnectionCallbackParams_t *params) {
}

/**
 * @brief Start advertising on a disconnect
 */
void BluetoothLE::disconnectionCallback(
    const Gap::DisconnectionCallbackParams_t *params) {
  _isConnected = false;
  ble->startAdvertising();
}

/**
 * @brief Called when the client writes to a writable characteristic
 * @param params Pointer to a structure that contains details on what was
 * written
 */
void BluetoothLE::onDataWritten(const GattWriteCallbackParams *params) {
  int i;
  int index = 0;
  // match the characteristic handle
  printf("BluetoothLE::onDataWritten ");
  for (i = 0; i < numberOfCharacteristics; i++) {
    if (params->handle == gattCharacteristics[i]->getValueHandle()) {
      characteristics[i]->copyDataWritten(params);
      index = i;
      break;
    }
  }
  (*_onDataWritten)(index);
}

/**
* @brief Update the characteristic notification
* @param index Index of the characteristic
* @param data Pointer to the byte data to update the charateristic payload
*/
void BluetoothLE::notifyCharacteristic(int index, uint8_t *data) {
  for (int i = 0; i < characteristics[index]->getPayloadLength(); i++) {
    characteristics[index]->getPayloadBytes()[i] = data[i];
  }
  characteristics[index]->update();
}

/**
* @brief Update the characteristic notification
* @param index Index of the characteristic
* @param data Pointer to the byte data to update the charateristic payload
*/
void BluetoothLE::notifyCharacteristicTest(int index) {
  for (int i = 0; i < characteristics[index]->getPayloadLength(); i++) {
    characteristics[index]->getPayloadBytes()[i]++;
  }
  characteristics[index]->update();
}

uint8_t *BluetoothLE::getDataWritten(int index, int *length) {
  return characteristics[index]->getDataWritten(length);
}

/**
* @brief Function to query if a BLE connection is active
* @return true if BLE connected, false if BLE is not connected
*/
bool BluetoothLE::isConnected(void) {
  return (ble->getGapState().connected == 1 ? true : false);
}
