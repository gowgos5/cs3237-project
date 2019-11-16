
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

#include <stdint.h>
#include "MAX30101_helper.h"
#include "MAX30101.h"
#include "Peripherals.h"

static uint8_t flags[3];

uint8_t MAX30101_Helper_IsStreaming(eMAX30101Flags flag) {
  return flags[(uint32_t)flag];
}
//******************************************************************************

void MAX30101_Helper_SetStreamingFlag(eMAX30101Flags flag, uint8_t state) {
  flags[(uint32_t)flag] = state;
}

//******************************************************************************
void MAX30101_Helper_Stop(void) {
  if (flags[(uint32_t)eStreaming_HR] == 1) {
    Peripherals::max30101()->HRmode_stop();
  }
  if (flags[(uint32_t)eStreaming_SPO2] == 1) {
    Peripherals::max30101()->SpO2mode_stop();
  }
  if (flags[(uint32_t)eStreaming_MULTI] == 1) {
    Peripherals::max30101()->Multimode_stop();
  }
  MAX30101_Helper_ClearStreamingFlags();
}

//******************************************************************************
void MAX30101_Helper_ClearStreamingFlags(void) {
  uint32_t i;
  for (i = 0; i < 3; i++) {
    flags[i] = 0;
  }
}

//******************************************************************************
void MAX30101_OnInterrupt(void) { I2CM_Init_Reset(2, 1); }
