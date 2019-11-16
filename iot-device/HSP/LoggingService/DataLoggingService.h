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
#ifndef _LOGGINGSERVICE_H_
#define _LOGGINGSERVICE_H_

#include "mbed.h"

/// types of logging
typedef enum {
  /// do not log
  eLogToNothing,
  /// log to USB
  eLogtoUsb,
  /// Log to external flash memory
  eLogToFlash
} eLoggingOutput;

/// types of logging
typedef enum {
  eTriggerLog_NULL,
  eTriggerLog_RPC_USB,
  eTriggerLog_RPC_FLASH,
  eTriggerLog_BUTTON,
  eTriggerLog_BLE,
} eLoggingTrigger;

/// extern that indicates the hardware button on the HSP was pressed
extern uint32_t buttonTrigger;

void LoggingService_Init(void);

/**
* @brief This routine checks to see if a USB or flash logging action needs to be
* taken
*           The routine checks for a start condition via button press, USB
* command, or BLE command
*           Once one of these start conditions is present, the logging begins
* until stopped or memory is full
* @return 1 if successful, 0 if error or logging was aborted and no logging
* occurred
*/
uint8_t LoggingService_ServiceRoutine(void);
/**
* @brief This is called via one of the RPC USB functions to set start conditons
* to start streaming USB
*/
void LoggingService_StartLoggingUsb(void);
/**
* @brief This is called via one of the RPC USB functions to set start conditons
* to start logging to flash
*/
void LoggingService_StartLoggingFlash(void);

#endif /* _LOGGINGSERVICE_H_ */
