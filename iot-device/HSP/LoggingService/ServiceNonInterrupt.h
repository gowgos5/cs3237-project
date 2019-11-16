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
#ifndef _SERVICENONINTERRUPT_H_
#define _SERVICENONINTERRUPT_H_

#include "mbed.h"
#include "Device_Logging.h"
#include "MAX30205.h"
/**
* @brief Initialize the book keeping variables for non interrupt devices
*/
void ServiceNonInterrupt_Init(void);
/**
* @brief Start the 1 second timer
*/
void ServiceNonInterrupt_StartTimer(void);
/**
* @brief Stop the 1 second timer
*/
void ServiceNonInterrupt_StopTimer(void);
/**
* @brief Log the BMP280 sensor value if it is time
* @param deviceLogging pointer to object that keeps track of logging for a device
*/
void ServiceNonInterrupt_BMP280(Device_Logging *deviceLogging);
/**
* @brief Log the BMP280 sensor value if it is time
* @param deviceLogging pointer to object that keeps track of logging for a device
* @param device pointer to the device instance (MAX30205 instance 0 or MAX30205 instance 1)
* @param packetId packet id that is used when building a packet
*/
void ServiceNonInterrupt_MAX30205(Device_Logging *deviceLogging, MAX30205 *device, uint32_t packetId);

#endif /* _SERVICENONINTERRUPT_H_ */


