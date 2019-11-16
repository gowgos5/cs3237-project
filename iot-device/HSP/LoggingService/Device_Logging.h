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
#ifndef _DEVICE_LOGGING_H_
#define _DEVICE_LOGGING_H_

#include "mbed.h"

/**
* @brief  Class that is used to store device logging parameters when logging to
* flash or streaming usb
*/
class Device_Logging {
public:
  /**
  * @brief Check if logging is enabled for this device
  */
  int isLoggingEnabled(void);
  /**
  * @brief Returns the sample rate for the device, rate is in seconds
  */
  int getLoggingSampleRate(void);
  /**
  * @brief Initialize the sampling rate for the device
  * @param sampleRate Rate to log device output in seconds
  */
  void initStart(int sampleRate);
  /**
  * @brief Gets a value that represents when device needs to be sampled again,
  * used for datalogging and usb streaming
  */
  int getNextSampleTime(void);
  /**
  * @brief Sets a value that represents when device needs to be sampled again,
  * used for datalogging and usb streaming
  * @param time Time for next sample in seconds, time is relative to a timer
  */
  void setNextSampleTime(int time);
  /**
  * @brief Disables further datalog and streaming sampling for the device
  * @param time Time for next sample in seconds, time is relative to a timer
  */
  void stop(void);

private:
  /// The sample rate in seconds
  int sampleRate;
  /// If logging is enabled or not
  int enabled;
  /// Bookkeeping var to keep track of the next sample time
  int nextSampleTime;
};

#endif /* _DEVICE_LOGGING_H_ */
