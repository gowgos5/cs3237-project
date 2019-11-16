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
#include "ServiceNonInterrupt.h"
#include "Streaming.h"
#include "BMP280.h"
#include "PacketFifo.h"
#include "MAX30205.h"
#include "Stream.h"
#include "BMP280.h"
#include "PacketFifo.h"
#include "Peripherals.h"
#include "Device_Logging.h"

/// timer used for devices that do not have interrupt capability
static Timer nonInterruptTimer;

/// reference to logging object for the BMP280
extern Device_Logging *bmp280_Logging;
/// reference to logging object for the MAX30205 (instance 0)
extern Device_Logging *MAX30205_0_Logging;
/// reference to logging object for the MAX30205 (instance 1)
extern Device_Logging *MAX30205_1_Logging;

/**
* @brief Initialize the book keeping variables for non interrupt devices
*/
void ServiceNonInterrupt_Init(void) {
  // clear out the next time member in the device logging class
  //   this is so that the devices are sampled the very first go-around
  bmp280_Logging->setNextSampleTime(0);
  MAX30205_0_Logging->setNextSampleTime(0);
  MAX30205_1_Logging->setNextSampleTime(0);
}

/**
* @brief Stop the 1 second timer
*/
void ServiceNonInterrupt_StartTimer(void) {
  nonInterruptTimer.start();
  nonInterruptTimer.reset();
}

/**
* @brief Stop the 1 second timer
*/
void ServiceNonInterrupt_StopTimer(void) { nonInterruptTimer.stop(); }

/**
* @brief Get the current count of the timer
* @return timer count in seconds
*/
static int ServiceNonInterrupt_GetTimerCount(void) {
  return nonInterruptTimer.read();
}

/**
* @brief Log the BMP280 sensor value if it is time
* @param deviceLogging pointer to object that keeps track of logging for a
* device
*/
void ServiceNonInterrupt_BMP280(Device_Logging *deviceLogging) {
  int currentTime;
  uint32_t bmp280_data[2]; // index 0 - Temp (deg C), index 1 - Press (Pa).
                           // Divide the result by 10 to get the correct answer.
  float temp_C, press_P;
  int nextTime;

  if (deviceLogging->isLoggingEnabled() == 0)
    return;
  currentTime = ServiceNonInterrupt_GetTimerCount();
  nextTime = deviceLogging->getNextSampleTime();
  if ((nextTime == 0) || (currentTime >= nextTime)) {
    nextTime = currentTime + deviceLogging->getLoggingSampleRate();
    deviceLogging->setNextSampleTime(nextTime);
    Peripherals::bmp280()->ReadCompData(
        &temp_C, &press_P); // Read the Temp (index 0)  and Pressure (index 1)
    bmp280_data[0] = (int32_t)(temp_C * 10);
    bmp280_data[1] = (int32_t)(press_P * 10);
    PacketFifo_InsertPacket(PACKET_BMP280_PRESSURE, &bmp280_data[0],
                            2); // Read it and insert it into the FIFO
  }
}

/**
* @brief Log the BMP280 sensor value if it is time
* @param deviceLogging pointer to object that keeps track of logging for a
* device
* @param device pointer to the device instance (MAX30205 instance 0 or MAX30205
* instance 1)
* @param packetId packet id that is used when building a packet
*/
void ServiceNonInterrupt_MAX30205(Device_Logging *deviceLogging,
                                  MAX30205 *device, uint32_t packetId) {
  int currentTime;
  uint32_t data;
  uint16_t tempData;
  int nextTime;

  if (deviceLogging->isLoggingEnabled() == 0)
    return;
  currentTime = ServiceNonInterrupt_GetTimerCount();
  nextTime = deviceLogging->getNextSampleTime();
  if ((nextTime == 0) || (currentTime >= nextTime)) {
    nextTime = currentTime + deviceLogging->getLoggingSampleRate();
    deviceLogging->setNextSampleTime(nextTime);
    device->readTemperature(&tempData);
    // assemble this in the correct order
    data = (uint32_t)((tempData >> 8) + ((tempData & 0xFF) << 8));
    PacketFifo_InsertPacket(packetId, &data,
                            1); // Read it and insert it into the FIFO
  }
}
