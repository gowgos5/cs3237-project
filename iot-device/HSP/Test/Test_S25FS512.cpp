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
#include "Test_S25FS512.h"
#include "Test_Utilities.h"

//******************************************************************************
void test_S25FS512(void (*outputString)(const char *)) {
  char tempStr[32];
  uint8_t page[264];
  int totalPass = 1;
  int pass;
  int i;
  uint8_t data[128];
  S25FS512 *s25FS512;

  s25FS512 = Peripherals::s25FS512();

  // display header
  outputString("Testing S25FS512|");

  // read id test
  s25FS512->readIdentification(data, sizeof(data));
  s25FS512->readIdentification(data, sizeof(data));
  if ((data[1] == 0x01) && (data[2] == 0x02) && (data[3] == 0x19) &&
      (data[4] == 0x4D)) {
    sprintf(tempStr, "Read ID: Pass ");
    outputString(tempStr);
  } else {
    sprintf(tempStr, "Read ID: Fail ");
    outputString(tempStr);
    totalPass = 0;
  }
  sprintf(tempStr, "(%02X%02X%02X%02X)|", data[1], data[2], data[3], data[4]);
  outputString(tempStr);

  
  if (totalPass == 1) {
    // format sector 0
    outputString("Formatting Sector 0, ");
    s25FS512->sectorErase_Helper(0);

    // verify format sector 0
    outputString("Verifying Format of Sector 0: ");
    s25FS512->readPages_Helper(0, 0, page, 0);
    pass = s25FS512->isPageEmpty(page);
    _printPassFail(pass, 1, outputString);
    totalPass &= pass;

    // fill page with random pattern
    for (i = 0; i < 256; i++) {
      page[i] =  i;
    }
    // write to page 0
    outputString("Writing Page 0 to pattern, ");
    s25FS512->writePage_Helper(0, page, 0);
    // clear pattern in memory
    for (i = 0; i < 256; i++) {
      page[i] = 0x00;
    }
    // read back page and verify
    outputString("Read Page Verify: ");
    s25FS512->readPages_Helper(0, 0, page, 0);
    // compare memory for pattern
    pass = 1;
    for (i = 0; i < 256; i++) {
      if (page[i] != i)
        pass = 0;
    }
    _printPassFail(pass, 1, outputString);
    totalPass &= pass;

    // format sector 0 to clean up after tests
    s25FS512->sectorErase_Helper(0);
  } else {
    outputString("Read Id Failed, Skipping rest of test|");
  }

  // final results
  outputString("Result: ");
  _printPassFail(totalPass, 0, outputString);
}
