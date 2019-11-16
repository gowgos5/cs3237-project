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

#include "MAX30001_helper.h"
#include "MAX30001.h"
#include "StringInOut.h"
#include "Peripherals.h"

static uint8_t flags[4];

int MAX30001_Helper_IsStreaming(eFlags flag) { 
 return flags[(uint32_t)flag]; 
}

void MAX30001_Helper_SetStreamingFlag(eFlags flag, uint8_t state) {
  flags[(uint32_t)flag] = state;
}

void MAX30001_Helper_Stop(void) {
  if (flags[(uint32_t)eStreaming_ECG] == 1) {
    Peripherals::max30001()->Stop_ECG();
  }
  if (flags[(uint32_t)eStreaming_PACE] == 1) {
    Peripherals::max30001()->Stop_PACE();
  }
  if (flags[(uint32_t)eStreaming_BIOZ] == 1) {
    Peripherals::max30001()->Stop_BIOZ();
  }
  if (flags[(uint32_t)eStreaming_RtoR] == 1) {
    Peripherals::max30001()->Stop_RtoR();
  }
  MAX30001_Helper_ClearStreamingFlags();
}

int MAX30001_AnyStreamingSet(void) {
  uint32_t i;
  for (i = 0; i < 4; i++) {
    if (flags[i] == 1) return 1;
  }
  return 0;
}

void MAX30001_Helper_StartSync(void) {
  if (MAX30001_AnyStreamingSet() == 1) {
    Peripherals::max30001()->synch();
  }
}

void MAX30001_Helper_ClearStreamingFlags(void) {
  uint32_t i;
  for (i = 0; i < 4; i++) {
    flags[i] = 0;
  }
}

void MAX30001_Helper_Debug_ShowStreamFlags(void) {
  putStr("\r\n");
  if (flags[(uint32_t)eStreaming_ECG] == 1) {
    putStr("eStreaming_ECG, ");
  }
  if (flags[(uint32_t)eStreaming_PACE] == 1) {
    putStr("eStreaming_PACE, ");
  }
  if (flags[(uint32_t)eStreaming_BIOZ] == 1) {
    putStr("eStreaming_BIOZ, ");
  }
  if (flags[(uint32_t)eStreaming_RtoR] == 1) {
    putStr("eStreaming_RtoR, ");
  }
  putStr("\r\n");
}

void MAX30001_Helper_SetupInterrupts() {
	Peripherals::max30001()->INT_assignment(MAX30001::MAX30001_INT_B,    MAX30001::MAX30001_NO_INT,   MAX30001::MAX30001_NO_INT,  ///<  en_enint_loc,      en_eovf_loc,   en_fstint_loc,
                                            MAX30001::MAX30001_INT_2B,   MAX30001::MAX30001_INT_2B,   MAX30001::MAX30001_NO_INT,  ///<  en_dcloffint_loc,  en_bint_loc,   en_bovf_loc,
                                            MAX30001::MAX30001_INT_2B,   MAX30001::MAX30001_INT_2B,   MAX30001::MAX30001_NO_INT,  ///<  en_bover_loc,      en_bundr_loc,  en_bcgmon_loc,
                                            MAX30001::MAX30001_INT_B,    MAX30001::MAX30001_NO_INT,   MAX30001::MAX30001_NO_INT,  ///<  en_pint_loc,       en_povf_loc,   en_pedge_loc,
                                            MAX30001::MAX30001_INT_2B,   MAX30001::MAX30001_INT_B,    MAX30001::MAX30001_NO_INT,  ///<  en_lonint_loc,     en_rrint_loc,  en_samp_loc,
                                            MAX30001::MAX30001_INT_ODNR, MAX30001::MAX30001_INT_ODNR);                            ///<  intb_Type,         int2b_Type)
}



static uint8_t serialNumber[6];
uint8_t *MAX30001_Helper_getVersion(void) {
  // read the id
  Peripherals::max30001()->reg_read(MAX30001::INFO, (uint32_t *)serialNumber);
  // read id twice because it needs to be read twice
  Peripherals::max30001()->reg_read(MAX30001::INFO, (uint32_t *)serialNumber);
  return serialNumber;
}
