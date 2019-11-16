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
#include "mbed.h"
#include "USBSerial.h"
#include "Logging.h"
#include "Streaming.h"
#include "RpcServer.h"
#include "S25FS512.h"
#include "BMP280.h"
#include "PacketFifo.h"
#include "DataLoggingService.h"
#include "ServiceNonInterrupt.h"
#include "HspLed.h"
#include "MAX30001_helper.h"
#include "MAX30101_helper.h"
#include "StringInOut.h"
#include "StringHelper.h"
#include "Peripherals.h"
#include "Device_Logging.h"

/// BMP280 logging object reference
extern Device_Logging *bmp280_Logging;
/// MAX14720 instance 0 logging object reference
extern Device_Logging *MAX30205_0_Logging;
/// MAX14720 instance 1 logging object reference
extern Device_Logging *MAX30205_1_Logging;

#define PING_PONG_BUFFER_SIZE 512
#define HALF_OF_PING_PONG_BUFFER_SIZE PING_PONG_BUFFER_SIZE / 2
#define MISSION_DEFINITION_SIZE 4096

eLoggingTrigger loggingTrigger;

/// buffer where mission strings are stored
char loggingMissionCmds[MISSION_DEFINITION_SIZE];
/// This houses two 256 byte ram concatenated to act as a ping-pong
uint8_t PingPong_SRAM[PING_PONG_BUFFER_SIZE];
uint32_t buttonTrigger = 0;

eLoggingOutput loggingOutput;
// extern int bleStartCommand;
bool volatile globalFlag;
extern int highDataRate;
static uint32_t currentPage;
static uint32_t sramIndex;
/// flag to indicate that sram buffer 0 is dirty and will need to be flushed
static uint32_t sram_buffer_0_dirty;
/// flag to indicate that sram buffer 1 is dirty and will need to be flushed
static uint32_t sram_buffer_1_dirty;
/// usb byte buffer for sending out a bulk transfer
static uint8_t usb_block[64];
/// running index used to accumulate bytes to send as a block via bulk transfer
static uint16_t usb_block_index = 0;

typedef enum {
  eStartEvent_NULL,
  eStartEvent_BLE,
  eStartEvent_BUTTON,
  eStartEvent_RPC_TO_USB,
  eStartEvent_RPC_TO_FLASH
} eStartEvent;
static eStartEvent startEvent;

extern USBSerial *usbSerialPtr;

/**
* @brief Sets a flag to start USB logging (streaming)
*/
void LoggingService_StartLoggingUsb(void) {
  loggingTrigger = eTriggerLog_RPC_USB;
}

/**
* @brief Sets a flag to start flash logging
*/
void LoggingService_StartLoggingFlash(void) {
  loggingTrigger = eTriggerLog_RPC_FLASH;
}

/**
* @brief Checks the various logging start condition
* @return 1 if a start condition is true, 0 if there is no start condition
*/
static bool _LoggingService_CheckStartCondition(void) {
  bool buttonPressed;
  buttonPressed = Peripherals::pushButton()->GetButtonFallState();

  // default not logging USB or flash
  loggingOutput = eLogToNothing;
  startEvent = eStartEvent_NULL;
  if (buttonPressed) {
    Peripherals::pushButton()->clearButtonFallState();
    // a falling state has been detected... wait for a fraction of a second and
    // re-read the pin
    //   only start datalogging if the pin was released within this wait time
    wait(0.75f);
    int buttonRead = Peripherals::pushButton()->Read();
    // if after a period of time the button is still pressed then get out
    if (buttonRead == 0)
      return 0;
    buttonTrigger = 0;

    loggingTrigger = eTriggerLog_BUTTON;
    loggingOutput = eLogToFlash;
    startEvent = eStartEvent_BUTTON;
    return true;
  }
  if (loggingTrigger == eTriggerLog_RPC_FLASH) {
    loggingOutput = eLogToFlash;
    startEvent = eStartEvent_RPC_TO_FLASH;
    return true;
  }
  if (Peripherals::hspBLE()->getStartDataLogging()) {
    loggingTrigger = eTriggerLog_BLE;
    loggingOutput = eLogToFlash;
    startEvent = eStartEvent_BLE;
    return true;
  }
  // check if start is from RPC call for USB streaming
  if (loggingTrigger == eTriggerLog_RPC_USB) {
    loggingOutput = eLogtoUsb;
    startEvent = eStartEvent_RPC_TO_USB;
    return true;
  }
  return false;
}

/**
* @brief Read the mission string from flash into a buffer
* @return false if a mission was not defined, true if mission was read and
* buffered
*/
static bool _LoggingService_ReadMissionFromFlash(void) {
  // get mission from flash
  Logging_ReadMissionFromFlash((uint8_t *)loggingMissionCmds);
  if (Logging_IsMissionDefined((uint8_t *)loggingMissionCmds) == 0) {
    return false;
  }
  printf(loggingMissionCmds);
  fflush(stdout);
  RPC_ProcessCmds(loggingMissionCmds);
  return true;
}

/**
* @brief Process a RPC command that is pointed to.
* @param cmd RPC string to process
*/
void ProcessCmd(const char *cmd) {
  char cmd_[256];
  char reply[512];
  strcpy(cmd_, cmd);
  RPC_call(cmd_, reply);
}

/**
* @brief Buffer sensor fifo data in ram buffers, when a ram buffer is full (a
* flash page worth of data is accumulated) then flash that buffer.
*        A buffer ping pong method is used so that one buffer can be flashing as
* the other buffer fills with sensor fifo data.
* @param fifoData Sensor data taken from the fifo to be stored into flash
*/
static void _LoggingServer_OutputToFlash(uint32_t fifoData) {
  uint32_t index;
  char str[128];
  uint8_t *ptr;
  //
  // Log To Flash
  //
  // i.e. there is data, read one 32-bit size data at a time.
  // put the fifo data into the ping-pong SRAM
  PingPong_SRAM[sramIndex++] = fifoData & 0xFF; // LSByte goes into index N
  PingPong_SRAM[sramIndex++] = (fifoData >> 8) & 0xFF;
  PingPong_SRAM[sramIndex++] = (fifoData >> 16) & 0xFF;
  PingPong_SRAM[sramIndex++] = (fifoData >> 24) & 0xFF; // MSByte goes into index N+3

  // flag this buffer as dirty
  if (sramIndex <= 256)
    sram_buffer_0_dirty = 1;
  else
    sram_buffer_1_dirty = 1;

  if (sramIndex == 256 ||
      sramIndex == 512) // Either Ping SRAM or Pong SRAM location is full
  {                     // therefore write to Flash

    index = sramIndex - 256;
    ptr = &PingPong_SRAM[index];
    sprintf(str, "currentPage=%lu", currentPage);
    Peripherals::s25FS512()->writePage_Helper(currentPage, ptr, 0);

    // this page is no longer dirty
    if (index == 0)
      sram_buffer_0_dirty = 0;
    if (index == 256)
      sram_buffer_1_dirty = 0;

    currentPage++;
  }
  sramIndex = sramIndex % 512; // Wrap around the index
}

/**
* @brief If flash ram buffers are flagged as dirty, flush to flash
*/
static void _LoggingServer_WriteDirtySramBufferToFlash(void) {
  uint8_t *ptr = PingPong_SRAM;
  if (sram_buffer_0_dirty == 0 && sram_buffer_1_dirty == 0)
    return;
  if (sram_buffer_0_dirty == 1) {
    ptr += 0;
  }
  if (sram_buffer_1_dirty == 1) {
    ptr += 256;
  }
  printf("_LoggingServer_WriteDirtySramBufferToFlash:%lu,%lu\n",
         sram_buffer_0_dirty, sram_buffer_1_dirty);
  fflush(stdout);
  // s25fs512_WritePage_Helper(currentPage, ptr, 0);
  Peripherals::s25FS512()->writePage_Helper(currentPage, ptr, 0);
}

/**
* @brief Initialize the USB block running index
* @param fifoData Sensor data taken from the fifo to be sent out USB
*/
static void _LoggingServer_OutputToCdcAcm(uint32_t fifoData) {
  uint8_t *ptr;
  uint8_t str[16];
  sprintf((char *)str, "%X ", (unsigned int)fifoData);
  ptr = str;
  usb_block_index = 0;
  while (*ptr != 0) {
    usb_block[usb_block_index] = *ptr;
    ptr++;
    usb_block_index++;
  }
  usbSerialPtr->writeBlock(usb_block, usb_block_index);
}

/**
* @brief Initialize the USB block running index
*/
static void _LoggingServer_OutputToCdcAcm_Start(void) { usb_block_index = 0; }

/**
* @brief Buffer up fifoData from sensors, do a USB block transfer if buffer is
* full
* @param fifoData Sensor data taken from the fifo to be send out USB within a
* bulk block transfer
* @return Return the success status of the writeblock operation
*/
static bool _LoggingServer_OutputToCdcAcm_Block(uint32_t fifoData) {
  uint8_t str[64];
  uint8_t *ptr;
  bool result;
  //
  // Log to CDCACM
  //
  result = true;
  sprintf((char *)str, "%X ", (unsigned int)fifoData);
  ptr = str;
  while (*ptr != 0) {
    usb_block[usb_block_index] = *ptr;
    ptr++;
    usb_block_index++;
    if (usb_block_index >= 64) {
      result = usbSerialPtr->writeBlock(usb_block, 64);
      usb_block_index = 0;
    }
  }
  return result;
}

/**
* @brief Output a full USB block via bulk transfer
*/
static void _LoggingServer_OutputToCdcAcm_End(void) {
  if (usb_block_index == 0)
    return;
  usbSerialPtr->writeBlock(usb_block, usb_block_index - 1);
}

/**
* @brief Blink LED pattern that indicates that the flash end boundary has been
* reached
*/
static void BlinkEndOfDatalogging(void) {
  // blink to signal end of logging
  Peripherals::hspLed()->pattern(0x55555555, 20);
  wait(2);
}

/**
* @brief Reads the first data page of flash, if all FF's then the page is empty
* @return 1 if the flash is empty as indicated by the first data page of the
* flash, 0 if not
*/
int isFlashEmpty(void) {
  int i;
  uint8_t data[256];
  int firstDataPage = Logging_GetLoggingStartPage();
  Peripherals::s25FS512()->readPages_Helper(firstDataPage, firstDataPage, data, 0);
  for (i = 0; i < 256; i++) {
    if (data[i] != 0xFF)
      return 0;
  }
  return 1;
}

/**
* @brief Blink LED pattern that indicates that the flash is not empty and a new
* flash logging session can not occur
*/
void BlinkFlashNotEmpty(void) {
  Peripherals::hspLed()->pattern(0x55555555, 20);
  wait(1);
}

void ExecuteDefaultMission(void) {
  ProcessCmd("/MAX30001/CAL_InitStart 01 01 01 03 7FF 00");
  ProcessCmd("/MAX30001/ECG_InitStart 01 01 01 00 02 03 1F 0 00 00 01");
  ProcessCmd("/MAX30001/RtoR_InitStart 01 03 0F 00 03 01 00 00 01");
  ProcessCmd("/MAX30001/Rbias_FMSTR_Init 01 02 01 01 00");
  ProcessCmd("/LIS2DH/InitStart 02 01");
}

void LoggingService_Init(void) { loggingTrigger = eTriggerLog_NULL; }

/**
* @brief This routine checks to see if a USB or flash logging action needs to be taken
*           The routine checks for a start condition via button press, USB command, or BLE command 
*           Once one of these start conditions is present, the logging begins until stopped or memory is full
* @return 1 if successful, 0 if error or logging was aborted and no logging occurred
*/
uint8_t LoggingService_ServiceRoutine(void) {
  uint32_t fifoData;
  uint32_t endPage;
  //USBSerial *usbSerial = Peripherals::usbSerial();
  // BMP280 *bmp280 = Peripherals::bmp280();
  bool buttonPressed;
  int packetBurstCount = 0;
  HspLed *hspLed = Peripherals::hspLed();

  sramIndex = 0;
  // only start logging if conditions exist

	if (_LoggingService_CheckStartCondition() == false) return 0;
	printf("Begin Logging...");
	if (startEvent == eStartEvent_NULL) printf("eStartEvent_NULL..."); 
	if (startEvent == eStartEvent_BLE) printf("eStartEvent_BLE..."); 
	if (startEvent == eStartEvent_BUTTON) printf("eStartEvent_BUTTON..."); 
	if (startEvent == eStartEvent_RPC_TO_USB) printf("eStartEvent_RPC_TO_USB..."); 
	if (startEvent == eStartEvent_RPC_TO_FLASH) printf("eStartEvent_RPC_TO_FLASH..."); 
	fflush(stdout);

  // start logging stuttered blink pattern
  hspLed->pattern(0xA0F3813, 20);

  if (startEvent == eStartEvent_RPC_TO_FLASH ||
      startEvent == eStartEvent_BUTTON) {
    // check to see if datalog already in flash... abort and force user to erase
    // flash if needed
    if (loggingOutput == eLogToFlash) {
      if (isFlashEmpty() == 0) {
        Logging_SetStart(false);
        // bleStartCommand = 0x00;
        BlinkFlashNotEmpty();
        hspLed->blink(1000);
        printf("Abort Logging, flash log exists. ");
        fflush(stdout);
        return 0;
      }
    }
  }

  if (startEvent == eStartEvent_BLE) {
    // check for mission in flash
    if (_LoggingService_ReadMissionFromFlash() == false) {
      // if there is no mission in flash then do a default mission for the sake
      // of ble Android app working "out-of-the-box" and stream RtoR and Accel
      printf("No Mission in Flash...ExecuteDefaultMission...");
      fflush(stdout);
      ExecuteDefaultMission();
      // do not log this data
      loggingOutput = eLogToNothing;
    } else {
      // there is a mission in flash check if there is already logged data
      if (isFlashEmpty() == 0) {
        // just do default mission
        printf("Logged Data Detected...ExecuteDefaultMission...");
        fflush(stdout);
        ExecuteDefaultMission();
        // do not log this data
        loggingOutput = eLogToNothing;
      } else {
        // flag that we are logging to flash
        loggingOutput = eLogToFlash;
      }
    }
  }

  // if we are logging to flash then read mission in flash
  if (loggingOutput == eLogToFlash) {
    if (_LoggingService_ReadMissionFromFlash() ==
        false) { // if there is no mission in flash then get out
      Logging_SetStart(false);
      Peripherals::hspLed()->pattern(0xC3C3C3C3, 20);
      wait(2);
      printf("Abort Logging, Mission does not exist. ");
      fflush(stdout);
      return 0;
    }
    currentPage = Logging_GetLoggingStartPage();
    endPage = Logging_GetLoggingEndPage();
  }

  MAX30001_Helper_SetupInterrupts();
  if (MAX30001_AnyStreamingSet() == 1) {
    MAX30001_Helper_StartSync();
  }

  SetDataLoggingStream(TRUE);
  ServiceNonInterrupt_Init();
  ServiceNonInterrupt_StartTimer();

  while (usbSerialPtr->readable()) {
    usbSerialPtr->_getc();
  }
  fifo_clear(GetUSBIncomingFifo()); // clear USB serial incoming fifo
  fifo_clear(GetStreamOutFifo());

  sram_buffer_0_dirty = 0;
  sram_buffer_1_dirty = 0;


  if (loggingOutput == eLogToNothing) { printf("eLogToNothing..."); fflush(stdout); }
	if (loggingOutput == eLogToFlash) { printf("eLogToFlash..."); fflush(stdout); }
	if (loggingOutput == eLogtoUsb) { printf("eLogtoUsb..."); fflush(stdout); }
	printf("highDataRate=%u...",(unsigned int)highDataRate); fflush(stdout);


  Peripherals::timestampTimer()->reset();
  Peripherals::timestampTimer()->start();

  _LoggingServer_OutputToCdcAcm_Start();
  while (1) {
    if (loggingOutput == eLogToFlash) {
      // check if we are at the end of flash
      endPage = Logging_GetLoggingEndPage();
      if (currentPage >= endPage) {
        BlinkEndOfDatalogging(); // blink for 3 seconds to signal end of logging
        break;
      }
    }

    if (startEvent == eStartEvent_BUTTON) {
      buttonPressed = Peripherals::pushButton()->GetButtonFallState();
      if (buttonPressed) {
        Peripherals::pushButton()->clearButtonFallState();
        // if there is a dirty sram buffer... flush it to flash
        _LoggingServer_WriteDirtySramBufferToFlash();
        BlinkEndOfDatalogging(); // blink for 3 seconds to signal end of logging
        break;
      }
    }

    if (loggingTrigger == eTriggerLog_BLE) {
      if (Peripherals::hspBLE()->getStartDataLogging() == false) {
        // if there is a dirty sram buffer... flush it to flash
        _LoggingServer_WriteDirtySramBufferToFlash();
        BlinkEndOfDatalogging(); // blink for 3 seconds to signal end of logging
        break;
      }
    }

    if (startEvent == eStartEvent_RPC_TO_USB ||
        startEvent == eStartEvent_RPC_TO_FLASH) {
      if (usbSerialPtr->available()) {
        if (loggingOutput == eLogToFlash) {
          _LoggingServer_WriteDirtySramBufferToFlash();
        }
        wait(0.2f);
        while (usbSerialPtr->available()) {
          usbSerialPtr->_getc();
        }
        fifo_clear(GetUSBIncomingFifo()); // clear USB serial incoming fifo
        fifo_clear(GetStreamOutFifo());
        break;
      }
    }

    // check to see if data is available
    packetBurstCount = 0;
    while (PacketFifo_Empty() == 0) {
      if (packetBurstCount >= 100)
        break;
      fifoData = PacketFifo_GetUint32();
      if (loggingOutput == eLogToFlash) {
        _LoggingServer_OutputToFlash(fifoData);
      }
      if (loggingOutput == eLogtoUsb) {
        if (highDataRate == 0)
          _LoggingServer_OutputToCdcAcm(fifoData);
        else
          _LoggingServer_OutputToCdcAcm_Block(fifoData);
      }
      packetBurstCount++;
    }

    if (PacketFifo_Empty() != 0) {
      Peripherals::ble()->waitForEvent();
    }
    ServiceNonInterrupt_BMP280(bmp280_Logging);
    ServiceNonInterrupt_MAX30205(MAX30205_0_Logging,
                                 Peripherals::max30205_top(),
                                 PACKET_MAX30205_TEMP_TOP);
    ServiceNonInterrupt_MAX30205(MAX30205_1_Logging,
                                 Peripherals::max30205_bottom(),
                                 PACKET_MAX30205_TEMP_BOTTOM);
  }
  _LoggingServer_OutputToCdcAcm_End();
  printf("End Logging.\n");
  fflush(stdout);

  bmp280_Logging->stop();
  MAX30205_0_Logging->stop();
  MAX30205_1_Logging->stop();
  MAX30001_Helper_Stop(); // if any MAX30001 streams have been started, stop
                          // them
  MAX30101_Helper_Stop(); // if any MAX30101 streams have been started, stop
                          // them
  Peripherals::lis2dh()->stop();
  SetDataLoggingStream(FALSE);
  Peripherals::timestampTimer()->stop();
  hspLed->blink(1000);
  // default to non-usb packet speed optimizing
  highDataRate = 0;
  loggingTrigger = eTriggerLog_NULL;
  return 1;
}
