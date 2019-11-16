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

#include "MAX14720_RPC.h"
#include "MAX14720.h"
#include "StringHelper.h"
#include "Peripherals.h"
#include "Device_Logging.h"

/**
* @brief Read the identification of the s25fs512 using quad spi
* @param argStrs incoming rpc argument strings
*  OUTPUT:
*		replyStrs: strings that get returned to the rpc caller
* @return 0 if successful, -1 if error
*/
int MAX14720_ReadBoostVSet(char argStrs[32][32], char replyStrs[32][32]) {
  uint8_t args[1];
  uint8_t reply[1];
  //MAX14720::BoostVSet_Values value;

  ProcessArgs(argStrs, args, sizeof(args));
  //Peripherals::max14720()->readBoostVSet(&value);
  //reply[0] = (char)value;
  FormatReply(reply, sizeof(reply), replyStrs);
  return 0;
}

/**
* @brief Read the identification of the s25fs512 using quad spi
* @param argStrs incoming rpc argument strings
*  OUTPUT:
*		replyStrs: strings that get returned to the rpc caller
* @return 0 if successful, -1 if error
*/
int MAX14720_WriteBoostVSet(char argStrs[32][32], char replyStrs[32][32]) {
  uint8_t args[1];
  uint8_t reply[1];

  ProcessArgs(argStrs, args, sizeof(args));
  // Peripherals::max14720()->boostEn(MAX14720::BoostEn_Disable);
  // Peripherals::max14720()->boostVSet((MAX14720::BoostVSet_Values)args[0]);
  // Peripherals::max14720()->boostEn(MAX14720::BoostEn_Enable);
  reply[0] = 0x80;
  FormatReply(reply, sizeof(reply), replyStrs);
  return 0;
}

/**
* @brief Read the identification of the s25fs512 using quad spi
* @param argStrs incoming rpc argument strings
*  OUTPUT:
*		replyStrs: strings that get returned to the rpc caller
* @return 0 if successful, -1 if error
*/
int MAX14720_ReadReg(char argStrs[32][32], char replyStrs[32][32]) {
  uint8_t args[1];
  uint8_t reply[1];
  ProcessArgs(argStrs, args, sizeof(args));
  // Peripherals::max14720()->readReg((MAX14720::MAX14720_REG_map_t)address,&value);
  reply[0] = 0;
  FormatReply(reply, sizeof(reply), replyStrs);
  return 0;
}

/**
* @brief Read the identification of the s25fs512 using quad spi
* @param argStrs incoming rpc argument strings
*  OUTPUT:
*		replyStrs: strings that get returned to the rpc caller
* @return 0 if successful, -1 if error
*/
int MAX14720_WriteReg(char argStrs[32][32], char replyStrs[32][32]) {
  uint8_t args[2];
  uint8_t reply[1];

  ProcessArgs(argStrs, args, sizeof(args));
  reply[0] = 0x80;
  FormatReply(reply, sizeof(reply), replyStrs);
  return 0;
}
