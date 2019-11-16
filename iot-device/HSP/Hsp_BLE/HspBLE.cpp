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
#include "HspBLE.h"
#include "Peripherals.h"
#include "MAX30001_helper.h"

#define LOW_BYTE(x) ((uint8_t)((x)&0xFF))
#define HIGH_BYTE(x) ((uint8_t)(((x) >> 8) & 0xFF))

/// define all of the characteristic UUIDs
uint8_t HspBLE::temperatureTopCharUUID[] = {0x35,0x44,0x53,0x1b,0x00,0xc3,0x43,0x42,0x97,0x55,0xb5,0x6a,0xbe,0x8e,0x6c,0x67};
uint8_t HspBLE::temperatureBottomCharUUID[] = {0x35,0x44,0x53,0x1b,0x00,0xc3,0x43,0x42,0x97,0x55,0xb5,0x6a,0xbe,0x8e,0x6a,0x66};
uint8_t HspBLE::accelerometerCharUUID[] = {0xe6,0xc9,0xda,0x1a,0x80,0x96,0x48,0xbc,0x83,0xa4,0x3f,0xca,0x38,0x37,0x05,0xaf};
uint8_t HspBLE::heartrateCharUUID[] = {0x62,0x1a,0x00,0xe3,0xb0,0x93,0x46,0xbf,0xaa,0xdc,0xab,0xe4,0xc6,0x48,0xc5,0x69};
uint8_t HspBLE::pressureCharUUID[] = {0x1d,0x8a,0x19,0x32,0xda,0x49,0x49,0xad,0x91,0xd8,0x80,0x08,0x32,0xe7,0xe9,0x40};
uint8_t HspBLE::dataCharUUID[] = {0xaa,0x8a,0x19,0x32,0xda,0x49,0x49,0xad,0x91,0xd8,0x80,0x08,0x32,0xe7,0xe9,0x40};
uint8_t HspBLE::commandCharUUID[] = {0x36,0xe5,0x5e,0x37,0x6b,0x5b,0x42,0x0b,0x91,0x07,0x0d,0x34,0xa0,0xe8,0x67,0x5a};


/// define the BLE device name
uint8_t HspBLE::deviceName[] = "MAXREFDES100";
/// define the BLE serial number
uint8_t HspBLE::serialNumber[] = {0x77, 0x22, 0x33, 0x45, 0x67, 0x89};
/// define the BLE service UUID
uint8_t HspBLE::envServiceUUID[] = {0x5c,0x6e,0x40,0xe8,0x3b,0x7f,0x42,0x86,0xa5,0x2f,0xda,0xec,0x46,0xab,0xe8,0x51};

HspBLE *HspBLE::instance = NULL;

/**
* @brief Constructor that inits the BLE helper object
* @param ble Pointer to the mbed BLE object
*/
HspBLE::HspBLE(BLE *ble) {
  bluetoothLE = new BluetoothLE(ble, NUMBER_OF_CHARACTERISTICS);
  instance = this;
  notificationUpdateRoundRobin = 0;
}

/**
* @brief Constructor that deletes the bluetoothLE object
*/
HspBLE::~HspBLE(void) { delete bluetoothLE; }

/**
* @brief Initialize all of the HSP characteristics, initialize the ble service
* and attach callbacks
*/
void HspBLE::init(void) {
  uint8_t *serialNumberPtr;
  // uint8_t serialNumberBuffer[6];
  serialNumberPtr = MAX30001_Helper_getVersion();
  printf("MAX30001 Version = %02X:%02X:%02X:%02X:%02X:%02X...\n",
         serialNumberPtr[0], serialNumberPtr[1], serialNumberPtr[2],
         serialNumberPtr[3], serialNumberPtr[4], serialNumberPtr[5]);
  serialNumberPtr[3] = 0x00;
  serialNumberPtr[4] = 0x00;
  serialNumberPtr[5] = 0x03;
  printf("BLE DeviceID = %02X:%02X:%02X:%02X:%02X:%02X...\n",
         serialNumberPtr[0], serialNumberPtr[1], serialNumberPtr[2],
         serialNumberPtr[3], serialNumberPtr[4], serialNumberPtr[5]);

    bluetoothLE->addCharacteristic(new Characteristic(2 /* number of bytes */,temperatureTopCharUUID,GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY));
    bluetoothLE->addCharacteristic(new Characteristic(2 /* number of bytes */,temperatureBottomCharUUID,GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY));
    bluetoothLE->addCharacteristic(new Characteristic(6 /* number of bytes */,accelerometerCharUUID,GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY));
    bluetoothLE->addCharacteristic(new Characteristic(4 /* number of bytes */,heartrateCharUUID,GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY));
    bluetoothLE->addCharacteristic(new Characteristic(8 /* number of bytes */,pressureCharUUID,GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY));
    bluetoothLE->addCharacteristic(new Characteristic(32 /* number of bytes */,dataCharUUID,GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE));
    bluetoothLE->addCharacteristic(new Characteristic(1 /* number of bytes */,commandCharUUID,GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE));
    bluetoothLE->initService(serialNumberPtr, deviceName, sizeof(deviceName),envServiceUUID);

  bluetoothLE->onDataWritten(&HspBLE::_onDataWritten);
  ticker2.attach(callback(this, &HspBLE::tickerHandler), 1);
  ticker.attach(callback(this, &HspBLE::ppgHandler), 0.04);
}

void HspBLE::_onDataWritten(int index) {
  HspBLE::instance->onDataWritten(index);
}

/**
* @brief Callback for written characteristics
* @param index Index of whose characteristic is written to
*/
void HspBLE::onDataWritten(int index) {
  int length;
  uint8_t *data;
  printf("onDataWritten ");
  if (index == CHARACTERISTIC_CMD) {
    data = bluetoothLE->getDataWritten(index, &length);
    if (length >= 1) {
        	if (data[0] == 0x00) startDataLogging = false;
        	if (data[0] == 0x01) startDataLogging = true;
        	printf("onDataWritten index %d, data %02X, length %d ",index,data[0],length); fflush(stdout);

    }
  }
}

int index = 0;
void HspBLE::pollSensor(int sensorId, uint8_t *data) {

  switch (sensorId) {
  case CHARACTERISTIC_TEMP_TOP: {
    uint16_t uShort;
    Peripherals::max30205_top()->readTemperature(&uShort);
    data[0] = HIGH_BYTE(uShort);
    data[1] = LOW_BYTE(uShort);
  } break;
  case CHARACTERISTIC_TEMP_BOTTOM: {
    uint16_t uShort;
    Peripherals::max30205_bottom()->readTemperature(&uShort);
    data[0] = HIGH_BYTE(uShort);
    data[1] = LOW_BYTE(uShort);
  } break;
  case CHARACTERISTIC_ACCELEROMETER: {
    unsigned int i;
    uint8_t *bytePtr;
    int16_t acclPtr[3];
    Peripherals::lis2dh()->get_motion_fifo(&acclPtr[0], &acclPtr[1],
                                    	   &acclPtr[2]);
    bytePtr = reinterpret_cast<uint8_t *>(&acclPtr);
    for (i = 0; i < sizeof(acclPtr); i++)
      data[i] = bytePtr[i];
  } break;
  case CHARACTERISTIC_PRESSURE: {
    unsigned int i;
    uint8_t *bytePtr;
    float temperature;
    float pressure;
    Peripherals::bmp280()->ReadCompData(&temperature, &pressure);
    bytePtr = reinterpret_cast<uint8_t *>(&temperature);
    for (i = 0; i < sizeof(float); i++)
      data[i] = bytePtr[i];
    bytePtr = reinterpret_cast<uint8_t *>(&pressure);
    for (i = 0; i < sizeof(float); i++)
      data[i + sizeof(float)] = bytePtr[i];
  } break;
  case CHARACTERISTIC_HEARTRATE: {
    // TODO
    uint32_t adc_val = Peripherals::max30101()->max30101_buffer[index];
    
    data[0] = (adc_val & 0x000000FFUL);
    data[1] = (adc_val & 0x0000FF00UL) >> 8;
    data[2] = (adc_val & 0x00FF0000UL) >> 16;
    data[3] = (adc_val & 0xFF000000UL) >> 24;
    
    if (index++ == 16) {
    	index = 0;
    }
  } break;
  }
}

bool HspBLE::getStartDataLogging(void) { return startDataLogging; }

/**
* @brief Timer Callback that updates all sensor characteristic notifications
*/
void HspBLE::tickerHandler(void) {
  uint8_t data[8];
  
  if (bluetoothLE->isConnected()) {
    //pollSensor(CHARACTERISTIC_TEMP_TOP, data);
    //bluetoothLE->notifyCharacteristic(CHARACTERISTIC_TEMP_TOP, data);
    pollSensor(CHARACTERISTIC_TEMP_BOTTOM, data);
    bluetoothLE->notifyCharacteristic(CHARACTERISTIC_TEMP_BOTTOM, data);
    //pollSensor(CHARACTERISTIC_ACCELEROMETER, data);
    //bluetoothLE->notifyCharacteristic(CHARACTERISTIC_ACCELEROMETER, data);
    //pollSensor(CHARACTERISTIC_HEARTRATE, data);
    //bluetoothLE->notifyCharacteristic(CHARACTERISTIC_HEARTRATE, data);
    //pollSensor(CHARACTERISTIC_PRESSURE, data);
    //bluetoothLE->notifyCharacteristic(CHARACTERISTIC_PRESSURE, data);
  }
}

uint32_t ppg_buffer[17];
uint32_t ppg_len = 0;
uint32_t ppg_counter = 0;
bool ppg_flag = 0;

void HspBLE::ppgHandler(void) {
  uint8_t data[4];
  
  if (bluetoothLE->isConnected() && ppg_flag == 1) {
	data[3] = ppg_buffer[ppg_counter] >> 24;
	data[2] = ppg_buffer[ppg_counter] >> 16;
	data[1] = ppg_buffer[ppg_counter] >> 8;
	data[0] = ppg_buffer[ppg_counter];
	
	bluetoothLE->notifyCharacteristic(CHARACTERISTIC_HEARTRATE, data);
	
	if(ppg_counter == ppg_len) {
		ppg_flag = 0;
		ppg_counter = 0;
	}
	
	ppg_counter++;
  }
}

void HspBLE::sendHRData(uint32_t buffer[], uint32_t len) {
  uint32_t i;
  
  for (i = 0; i < len; i++) {
  	ppg_buffer[i] = buffer[i];
  }
  
  ppg_len = len;
  ppg_flag = 1;
  ppg_counter = 0;
}

void HspBLE::sendAccData(uint32_t buffer[], uint32_t len) {
	unsigned int i;
    uint8_t *bytePtr;
    int16_t acclPtr[3];
  	uint8_t data[6];
  	
    Peripherals::lis2dh()->get_motion_fifo(&acclPtr[0], &acclPtr[1],
                                    	   &acclPtr[2]);
    bytePtr = reinterpret_cast<uint8_t *>(&acclPtr);
    for (i = 0; i < sizeof(acclPtr); i++)
      data[i] = bytePtr[i];
	
	bluetoothLE->notifyCharacteristic(CHARACTERISTIC_ACCELEROMETER, data);
}
