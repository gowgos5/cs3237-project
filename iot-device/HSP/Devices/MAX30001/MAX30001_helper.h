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
#ifndef MAX30001_HELPER_H_
#define MAX30001_HELPER_H_

#include "mbed.h"

typedef enum eFlags {
  eStreaming_ECG,
  eStreaming_PACE,
  eStreaming_BIOZ,
  eStreaming_RtoR
} eFlags;

int MAX30001_Helper_IsStreaming(eFlags flag);
void MAX30001_Helper_SetStreamingFlag(eFlags flag, uint8_t state);
void MAX30001_Helper_Stop(void);
void MAX30001_Helper_ClearStreamingFlags(void);
int MAX30001_AnyStreamingSet(void);
void MAX30001_Helper_Debug_ShowStreamFlags(void);
void MAX30001_Helper_StartSync(void);
void MAX30001_Helper_SetupInterrupts(void);
uint8_t *MAX30001_Helper_getVersion(void);

#endif /* MAX30001_HELPER_H_ */
