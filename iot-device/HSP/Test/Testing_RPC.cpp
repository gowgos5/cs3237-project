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
#include "StringHelper.h"
#include "MAX30205.h"
#include "Test_BMP280.h"
#include "Test_LIS2DH.h"
#include "Test_MAX30001.h"
#include "Test_MAX30101.h"
#include "Test_MAX30205.h"
#include "Test_S25FS512.h"
#include "StringInOut.h"

void outputTestResultString(const char *resultStr);

//******************************************************************************
int Test_S25FS512(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t reply[1];
  test_S25FS512(outputTestResultString);
  reply[0] = 0x80;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int Test_BMP280(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t reply[1];
  test_BMP280(outputTestResultString);
  reply[0] = 0x80;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int Test_LIS2DH(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t reply[1];
  test_LIS2DH(outputTestResultString);
  reply[0] = 0x80;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int Test_LSM6DS3(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t reply[1];
  reply[0] = 0x80;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int Test_MAX30205_1(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t reply[1];
  MAX30205 *max30205;
  max30205 = Peripherals::max30205_top();
  test_MAX30205(max30205, outputTestResultString);
  reply[0] = 0x80;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int Test_MAX30205_2(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t reply[1];
  MAX30205 *max30205;
  max30205 = Peripherals::max30205_bottom();
  test_MAX30205(max30205, outputTestResultString);
  reply[0] = 0x80;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int Test_MAX30101(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t reply[1];
  test_MAX30101(outputTestResultString);
  reply[0] = 0x80;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int Test_MAX30001(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t reply[1];
  test_MAX30001(outputTestResultString);
  reply[0] = 0x80;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int Test_EM9301(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t reply[1];
  reply[0] = 0x80;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
void outputTestResultString(const char *resultStr) { putStr(resultStr); }
