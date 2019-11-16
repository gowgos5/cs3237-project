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
#include "Test_MAX30001.h"
#include "Test_Utilities.h"

uint32_t testing_max30001 = 0;
uint32_t testing_ecg_flags[4];
#define TESTING_TIMEOUT_SECONDS 10

//******************************************************************************
void test_MAX30001(void (*outputString)(const char *)) {
  int totalPass = 1;
  uint32_t foundEcg = 0;
  uint32_t foundBioz = 0;
  uint32_t foundPace = 0;
  uint32_t foundRtoR = 0;
  uint32_t id;
  char str2[128];
  int partVersion; // 0 = 30004
  // 1 = 30001
  // 2 = 30002
  // 3 = 30003
  Timer timer;
  MAX30001 *max30001;
  max30001 = Peripherals::max30001();

  // read the id
  max30001->reg_read(MAX30001::INFO, &id);
  // read id twice because it needs to be read twice
  max30001->reg_read(MAX30001::INFO, &id);
  partVersion = id >> 12;
  partVersion = partVersion & 0x3;

  // display header
  if (partVersion == 0)
    outputString("Testing MAX30004|");
  if (partVersion == 1) {
    outputString("Testing MAX30001|");
    outputString("Testing ECG, RtoR, BioZ, PACE|");
  }
  if (partVersion == 2)
    outputString("Testing MAX30002|");
  if (partVersion == 3) {
    outputString("Testing MAX30003|");
    outputString("Only Testing ECG and RtoR|");
  }
  sprintf(str2, "Device ID = 0x%06X|", (unsigned int)id);
  outputString(str2);

  // clear testing flags
  testing_ecg_flags[TESTING_ECG_FLAG] = 0;
  testing_ecg_flags[TESTING_BIOZ_FLAG] = 0;
  testing_ecg_flags[TESTING_PACE_FLAG] = 0;
  testing_ecg_flags[TESTING_RTOR_FLAG] = 0;

  // start streams
  testing_max30001 = 1;
  if (partVersion == 1)
    outputString("Start Streaming ECG, RtoR, PACE, BIOZ, CAL enabled, "
                 "verifying streams...|");
  if (partVersion == 3)
    outputString(
        "Start Streaming ECG, RtoR, CAL enabled, verifying streams...|");
  // CAL_InitStart(0b1, 0b1, 0b1, 0b011, 0x7FF, 0b0);
  max30001->CAL_InitStart(0b1, 0b1, 0b1, 0b011, 0x7FF, 0b0);
  max30001->ECG_InitStart(0b1, 0b1, 0b1, 0b0, 0b10, 0b11, 0x1F, 0b00,
                                   0b00, 0b0, 0b01);
  if (partVersion == 1)
    max30001->PACE_InitStart(0b1, 0b0, 0b0, 0b1, 0x0, 0b0, 0b00, 0b0,
                                      0b0);
  if (partVersion == 1)
    max30001->BIOZ_InitStart(0b1, 0b1, 0b1, 0b10, 0b11, 0b00, 7, 0b0,
                                      0b010, 0b0, 0b10, 0b00, 0b00, 2, 0b0,
                                      0b111, 0b0000);
  max30001->RtoR_InitStart(0b1, 0b0011, 0b1111, 0b00, 0b0011, 0b000001,
                                    0b00, 0b000, 0b01);
  max30001->Rbias_FMSTR_Init(0b01, 0b10, 0b1, 0b1, 0b00);
  max30001->synch();

  // look for each stream
  timer.start();
  while (1) {
    if ((foundEcg == 0) && (testing_ecg_flags[TESTING_ECG_FLAG] == 1)) {
      foundEcg = 1;
      outputString("ECG Stream: PASS|");
    }
    if ((foundBioz == 0) && (testing_ecg_flags[TESTING_BIOZ_FLAG] == 1)) {
      foundBioz = 1;
      outputString("Bioz Stream: PASS|");
    }
    if ((foundPace == 0) && (testing_ecg_flags[TESTING_PACE_FLAG] == 1)) {
      foundPace = 1;
      outputString("PACE Stream: PASS|");
    }
    if ((foundRtoR == 0) && (testing_ecg_flags[TESTING_RTOR_FLAG] == 1)) {
      foundRtoR = 1;
      outputString("RtoR Stream: PASS|");
    }
    if ((foundEcg == 1) && (foundBioz == 1) && (foundPace == 1) &&
        (foundRtoR == 1) && (partVersion == 1)) {
      break;
    }
    if ((foundEcg == 1) && (foundRtoR == 1) && (partVersion == 3)) {
      break;
    }
    if (timer.read() >= TESTING_TIMEOUT_SECONDS) {
      break;
    }
  }
  timer.stop();
  if (foundEcg == 0) {
    outputString("ECG Stream: FAIL|");
    totalPass = 0;
  }
  if ((foundBioz == 0) && (partVersion == 1)) {
    outputString("Bioz Stream: FAIL|");
    totalPass = 0;
  }
  if ((foundPace == 0) && (partVersion == 1)) {
    outputString("PACE Stream: FAIL|");
    totalPass = 0;
  }
  if (foundRtoR == 0) {
    outputString("RtoR Stream: FAIL|");
    totalPass = 0;
  }

  // stop all streams
  max30001->Stop_ECG();
  if (partVersion == 1)
    max30001->Stop_PACE();
  if (partVersion == 1)
    max30001->Stop_BIOZ();
  max30001->Stop_RtoR();
  testing_max30001 = 0;
  // final results
  outputString("Result: ");
  _printPassFail(totalPass, 0, outputString);
}
