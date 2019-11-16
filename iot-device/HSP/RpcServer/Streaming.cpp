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
#include "RpcServer.h"
#include "RpcFifo.h"
#include "Streaming.h"
#include "Peripherals.h"
#include "Test_MAX30101.h"
#include "Test_MAX30001.h"
#include "MAX30001.h"
#include "MAX30101.h"
#include "Peripherals.h"


bool streaming = FALSE;
bool dataLogging = FALSE;

/**
* @brief Encodes a 0x55 0xAA signature and a simple checksum to the id byte in
* the 32 bit field
* @param id Streaming ID
*/
uint32_t StreamIdChecksumCalculate(uint32_t id) {
  uint32_t sum;
  uint32_t calculated;
  sum = 0x55;
  sum += 0xAA;
  sum += id;
  sum &= 0xFF;
  sum = sum << 8;
  calculated = 0x55AA0000 + sum + id;
  return calculated;
}

/**
* @brief Creates a packet that will be streamed via USB or saved into flash
* datalog memory
* @brief the packet created will be inserted into a fifo to be streamed at a
* later time
* @param id Streaming ID
* @param buffer Pointer to a uint32 array that contains the data to include in
* the packet
* @param number Number of elements in the buffer
*/
void StreamPacketUint32(uint32_t id, uint32_t *buffer, uint32_t number) {
  uint32_t checksumId;
  
  if (streaming == TRUE || dataLogging == TRUE) {
    checksumId = StreamIdChecksumCalculate(id);
    StreamFifoId(checksumId);
    StreamFifoTimeStamp();
    StreamFifoLength(number);
    StreamFifoUint32Array(buffer, number);
  }
  if (testing_max30001 == 1) {
    if (id == MAX30001_DATA_ECG)
      testing_ecg_flags[TESTING_ECG_FLAG] = 1;
    if (id == MAX30001_DATA_BIOZ)
      testing_ecg_flags[TESTING_BIOZ_FLAG] = 1;
    if (id == MAX30001_DATA_PACE)
      testing_ecg_flags[TESTING_PACE_FLAG] = 1;
    if (id == MAX30001_DATA_RTOR)
      testing_ecg_flags[TESTING_RTOR_FLAG] = 1;
  }
  if (testing_max30101 == 1) {
    if (id == (MAX30101_OXIMETER_DATA + 1))
      testing_max30101_flags[TESTING_HR_FLAG] = 1;
    if (id == (MAX30101_OXIMETER_DATA + 2))
      testing_max30101_flags[TESTING_SPO2_FLAG] = 1;
    if (id == (MAX30101_OXIMETER_DATA + 3))
      testing_max30101_flags[TESTING_MULTI_FLAG] = 1;
  }
  
  if (id == MAX30101_OXIMETER_DATA + 1) {
    Peripherals::hspBLE()->sendHRData(buffer, number);
  } else if (id == PACKET_LIS2DH) {
    Peripherals::hspBLE()->sendAccData(buffer, number);
  }
}

/**
* @brief Insert a buffer into the out going fifo
* @param buffer Array of uint32 to send to the fifo
* @param len Length of the array
*/
int StreamFifoUint32Array(uint32_t buffer[], uint32_t len) {
  int status;
  uint32_t i;
  for (i = 0; i < len; i++) {
    status = fifo_put32(GetStreamOutFifo(), buffer[i]);
    if (status == -1) {
      printf("FIFO_OF!");
      fflush(stdout);
      while (1)
        ;
    }
  }
  return 0;
}

/**
* @brief Insert a timestamp into the out going fifo
*/
int StreamFifoTimeStamp(void) {
  int status;
  // uint32_t timer = timestamp_GetCurrent(); //RTC_GetVal();
  uint32_t timer = (uint32_t)Peripherals::timestampTimer()->read_us();
  status = fifo_put32(GetStreamOutFifo(), timer);
  if (status == -1) {
    printf("FIFO_OF!");
    fflush(stdout);
    while (1)
      ;
  }
  return 0;
}

/**
* @brief Insert a packet id into the out going fifo
* @param id The uint32 packet id
*/
int StreamFifoId(uint32_t id) {
  int status;
  status = fifo_put32(GetStreamOutFifo(), id);
  if (status == -1) {
    printf("FIFO_OF!");
    fflush(stdout);
    while (1)
      ;
  }
  return 0;
}

/**
* @brief Insert a length value into the out going fifo
* @param length A uint32 number representing a length
*/
int StreamFifoLength(uint32_t length) {
  int status;
  status = fifo_put32(GetStreamOutFifo(), length);
  if (status == -1) {
    printf("FIFO_OF!");
    fflush(stdout);
    while (1)
      ;
  }
  return 0;
}

/**
* @brief Return a value that indicates if the system is streaming data
* @returns Returns a one or zero value
*/
uint8_t IsStreaming(void) { return streaming; }

/**
* @brief Set a flag to indicate if streaming is enabled
* @param state A one or zero value
*/
void SetStreaming(uint8_t state) { streaming = state; }

/**
* @brief Set a flag to indicate if datalogging is enabled
* @param state A one or zero value
*/
void SetDataLoggingStream(uint8_t state) { dataLogging = state; }
