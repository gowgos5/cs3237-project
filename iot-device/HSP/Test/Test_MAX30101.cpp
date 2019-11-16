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
#include "Test_MAX30101.h"
#include "Test_Utilities.h"

uint32_t testing_max30101 = 0;
uint32_t testing_max30101_flags[4];

//******************************************************************************
void test_MAX30101(void (*outputString)(const char *)) {
  Timer timer;
  int totalPass = 1;
  uint32_t foundHR = 0;
  uint32_t foundSPO2 = 0;
  uint32_t foundMULTI = 0;
  MAX30101 *max30101;

  max30101 = Peripherals::max30101();

  // display header
  outputString("Testing MAX30101|");

  // clear testing flags
  testing_max30101_flags[TESTING_HR_FLAG] = 0;
  testing_max30101_flags[TESTING_SPO2_FLAG] = 0;
  testing_max30101_flags[TESTING_MULTI_FLAG] = 0;
  testing_max30101 = 1;

  //
  // start stream (HR)
  //
  outputString("Start HR Streaming...|");
  max30101->HRmode_init(0x0, 0x00, 0x01, 0x03,
                        0x33); // This is the HR mode only (IR LED only)
                               // look for (HR) streaming
  timer.start();
  while (1) {
    if ((foundHR == 0) && (testing_max30101_flags[TESTING_HR_FLAG] == 1)) {
      foundHR = 1;
      outputString("HR Stream: PASS|");
      break;
    }
    if (timer.read() >= TESTING_MAX30101_TIMEOUT_SECONDS) {
      break;
    }
  }
  if (foundHR == 0) {
    outputString("HR Stream: FAIL|");
    totalPass = 0;
  }
  // stop stream
  max30101->HRmode_stop();

  //
  // start stream (SPO2)
  //
  outputString("Start SPO2 Streaming...|");
  max30101->SpO2mode_init(
      0xF, 0x00, 0x01, 0x03, 0x33,
      0x33); // This is the SpO2 mode only (Red and IR LED only)
  // look for (SPO2) stream
  timer.reset();
  while (1) {
    if ((foundSPO2 == 0) && (testing_max30101_flags[TESTING_SPO2_FLAG] == 1)) {
      foundSPO2 = 1;
      outputString("SPO2 Stream: PASS|");
      break;
    }
    if (timer.read() >= TESTING_MAX30101_TIMEOUT_SECONDS) {
      break;
    }
  }
  if (foundSPO2 == 0) {
    outputString("SPO2 Stream: FAIL|");
    totalPass = 0;
  }
  // stop stream
  max30101->SpO2mode_stop();

  //
  // start stream (MULTI)
  //
  outputString("Start Multi Streaming...|");
  max30101->Multimode_init(0x00, 0x00, 0x01, 0x03, 0x33, 0x33, 0x33, 0x01, 0x02,
                           0x03, 0x00); // Up to 4 LED can be turned on over
  // look for (SPO2) stream
  timer.reset();
  while (1) {
    if ((foundMULTI == 0) &&
        (testing_max30101_flags[TESTING_MULTI_FLAG] == 1)) {
      foundMULTI = 1;
      outputString("Multi Stream: PASS|");
      break;
    }
    if (timer.read() >= TESTING_MAX30101_TIMEOUT_SECONDS) {
      break;
    }
  }
  if (foundMULTI == 0) {
    outputString("Multi Stream: FAIL|");
    totalPass = 0;
  }
  // stop stream
  max30101->Multimode_stop();

  testing_max30101 = 0;

  timer.stop();
  // final results
  outputString("Result: ");
  _printPassFail(totalPass, 0, outputString);
}
