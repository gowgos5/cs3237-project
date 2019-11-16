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
#include "RpcServer.h"
#include "Logging.h"
#include "S25FS512.h"
#include "Peripherals.h"

/// length of flash page as dictated by the device
#define LENGTH_OF_FLASH_PAGE 256 // length of flash page in bytes
/// size in bytes of the external flash device on the HSP platform
#define SIZE_OF_EXTERNAL_FLASH  0x2000000 // 33,554,432 Bytes
/// start page of where the mission is defined
#define MISSION_DEFINITION_START_PAGE 0x00
/// end page of the mission
#define MISSION_DEFINITION_END_PAGE 0x0F
/// page of where the logging data starts
#define LOGGING_START_PAGE 0x12
/// the last logging page
#define LOGGING_END_PAGE ((SIZE_OF_EXTERNAL_FLASH / LENGTH_OF_FLASH_PAGE) - 1)

/// static flag to know if logging was started via RPC
static bool startLoggingViaRpc = false;

/**
* @brief This will read the mission location and if there is something valid,
* then run the Logging_ProcessMissionCmds()
* @param cmdBuffer buffer
*/
uint32_t Logging_IsMissionDefined(uint8_t *cmdBuffer) {
  uint32_t valid = 1;
  if ((cmdBuffer[0] == 0xFF) || (cmdBuffer[0] == 0x0))
    valid = 0;
  return valid;
}

/**
* @brief Read the mission from flash and place in buffer
* @param buffer pointer to byte array that will contain the read results
*/
int8_t Logging_ReadMissionFromFlash(uint8_t *buffer) {
  return Peripherals::s25FS512()->readPages_Helper(
      MISSION_DEFINITION_START_PAGE, MISSION_DEFINITION_END_PAGE, buffer, 0);
}

//******************************************************************************
// return the page where mission is defined, Mission specific
uint32_t Logging_GetMissionStartPage(void) {
  return MISSION_DEFINITION_START_PAGE;
}

//******************************************************************************
// return the page where the mission definition ends, Mission specific
uint32_t Logging_GetMissionEndPage(void) { return MISSION_DEFINITION_END_PAGE; }

//******************************************************************************
// Returns the location where the Writing can start from, for data logging...
uint32_t Logging_GetLoggingStartPage(void) { return LOGGING_START_PAGE; }

//******************************************************************************
// Returns the end location available where the Flash ends essentially.... for
// data logging.
uint32_t Logging_GetLoggingEndPage(void) { return LOGGING_END_PAGE; }

//******************************************************************************
void Logging_SetStart(bool state) { startLoggingViaRpc = state; }

//******************************************************************************
bool Logging_GetStart(void) { return startLoggingViaRpc; }

//******************************************************************************
// for debugging... always say that usb is not connected... for easy bench
// testing
uint32_t Usb_IsConnected(void) { return 0; }
