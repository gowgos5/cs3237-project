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
 ******************************************************************************
 */
#include "StringHelper.h"
#include <stdint.h>
#include "Streaming.h"
#include "StringInOut.h"
#include "DataLoggingService.h"
#include "Peripherals.h"
#include "Logging.h"
#include "S25FS512.h"

extern char loggingMissionCmds[4096];
uint32_t missionCmdIndex;

 //******************************************************************************
 int Logging_RPC_StartMissionDefine(char argStrs[32][32],
                                   char replyStrs[32][32]) {
  uint32_t i;
  uint32_t reply[1];

  // reset the missionCmdIndex to the beginning of the cmd buffer
  missionCmdIndex = 0;
  // clear the mission command buffer, fill with zeros
  for (i = 0; i < sizeof(loggingMissionCmds); i++) {
    loggingMissionCmds[i] = 0;
  }

  reply[0] = 0x80;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int Logging_RPC_AppendMissionCmd(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t reply[1];
  char *strPtr;
  uint32_t count = 0;
  uint8_t result = 0x80;
  // append the string to the mission cmd log
  strPtr = argStrs[0];
  while (*strPtr != 0) {
    loggingMissionCmds[missionCmdIndex] = *strPtr;
    missionCmdIndex++;
    strPtr++;
    // do not overrun buffer
    if (missionCmdIndex > (sizeof(loggingMissionCmds) - 2)) {
      result = 0xFF;
      break;
    }
    count++;
    // do not read more than max count in incoming string
    if (count > (32 * 32)) {
      result = 0xFF;
      break;
    }
  }
  if (result != 0x80) {
    reply[0] = 0xFF;
    FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
    return 0;
  }
  // add cr/lf to the end of this cmd string
  loggingMissionCmds[missionCmdIndex++] = 13;
  loggingMissionCmds[missionCmdIndex++] = 10;

  reply[0] = 0x80;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int Logging_RPC_EndMissionDefine(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t reply[1];
  reply[0] = 0x80;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int Logging_RPC_WriteMission(char argStrs[32][32], char replyStrs[32][32]) {
  uint8_t page;
  char *ptr;
  uint32_t reply[1];

  Peripherals::s25FS512()->parameterSectorErase_Helper(0x0);
  ptr = loggingMissionCmds;
  for (page = 0; page < 16; page++) {
    Peripherals::s25FS512()->writePage_Helper(page, (uint8_t *)ptr, 0);
    ptr += 256;
  }

  reply[0] = 0x80;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int Logging_RPC_ReadMission(char argStrs[32][32], char replyStrs[32][32]) {
  char *ptr;
  uint32_t i;
  // read sector 0
  ptr = loggingMissionCmds;
  for (i = 0; i < 16; i++) {
    Peripherals::s25FS512()->readPages_Helper(i, i, (uint8_t *)ptr, 0);
    ptr += 256;
  }
  // strip header by shifting payload left
  ptr = loggingMissionCmds;
  for (i = 0; i < sizeof(loggingMissionCmds); i++) {
    if (*ptr == 13) {
      *ptr = ':';
    } else if (*ptr == 10) {
      *ptr = ' ';
    } 
    ptr++;
  }
  if (loggingMissionCmds[0] == 0xFF || loggingMissionCmds[0] == 0x0) {
    sprintf(loggingMissionCmds, "%s", "null ");
  }
  // send it out via uart
  putStr(loggingMissionCmds);
  replyStrs[0][0] = 0;
  return 0;
}

//******************************************************************************
int Logging_RPC_EraseMission(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t reply[1];
  Peripherals::s25FS512()->parameterSectorErase_Helper(0x0);
  reply[0] = 0x80;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//
// small helper function to determine if the X number of a page are FF (empty)
//
static bool isPartialPageEmpty(uint8_t *ptr, int count) {
	int i;
	for (i = 0; i < count; i++) {
		if (ptr[i] != 0xFF) return false;
	}
	return true;
}

//
// define the different sector sizes in flash
//
typedef enum {
  e4K,
  e32K,
  e64K
} loggingFlashArea_t;

//******************************************************************************
int Logging_EraseWrittenSectors(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t i;
  uint8_t data[512];
  uint32_t address;
  uint32_t pageNumber;
  bool pageEmpty;
  uint32_t pagesToScan;
  loggingFlashArea_t currentArea;
  uint32_t currentAreaSize;
  //Peripherals::s25FS512()->bulkErase_Helper();

  //
  // quickly scan through the first few bytes of a page and continue this through a sector
  // this will determine if the sector needs to be erased or can be skipped
  //
  currentArea = e4K;
  address = 0;
  // interate through the entire flash
  while (address < SIZE_OF_EXTERNAL_FLASH) {
    // determine the size of the current area 
    switch (currentArea) {
      case e4K: 
        currentAreaSize = ADDRESS_INC_4K;
        break;
      case e32K: 
        currentAreaSize = ADDRESS_INC_32K;
        break;
      default: 
        currentAreaSize = ADDRESS_INC_64K;
        break;
    }
    pagesToScan = currentAreaSize / SIZE_OF_PAGE;
    pageNumber = address >> 8;
    pageEmpty = true;
    // scan the first few bytes in each page for this area
    for (i = 0; i < pagesToScan; i++) {
      Peripherals::s25FS512()->readPartialPage_Helper(pageNumber + i, data, 4);
      pageEmpty = isPartialPageEmpty(data, 4);
      if (pageEmpty == false) break;
    }
    // if we detected data then erase this sector
    if (pageEmpty == false) {
      switch (currentArea) {
        case e4K: 
          Peripherals::s25FS512()->parameterSectorErase_Helper(address); 
          break;
        default: 
          Peripherals::s25FS512()->sectorErase_Helper(address); 
          break;
      }
    } 
    // update the address with the current area size
    address += currentAreaSize;
    // determine the new area in flash
    if (address < ADDRESS_4K_END) {  
      currentArea = e4K;
    } else if (address == ADDRESS_32K_START) {  
        currentArea = e32K;
      } else { 
          currentArea = e64K;
        }    
  }
  return 0;
}

//******************************************************************************
int Logging_Start(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t reply[1];
  Logging_SetStart(true);
  reply[0] = 0x80; // indicate success
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int Logging_GetLastWrittenPage(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t reply[1];

  uint32_t page;
  uint32_t lastPage;
  uint32_t pageEmpty;
  uint8_t data[512];

  printf("Logging_GetLastWrittenPage ");
  fflush(stdout);
  lastPage = Logging_GetLoggingEndPage();
  for (page = 2; page <= lastPage; page++) {
    // Peripherals::serial()->printf("checking page %d ",page); fflush(stdout);
    // sample the page
    Peripherals::s25FS512()->readPages_Helper(page, page, data, 0);
    pageEmpty = Peripherals::s25FS512()->isPageEmpty(data);
    if (pageEmpty != 0) {
      break;
    }
  }
  if (page > lastPage)
    page = lastPage;
  printf("last page %d, 0x%X ", (unsigned int)page, (unsigned int)page);
  fflush(stdout);
  reply[0] = page;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

extern int highDataRate;
//******************************************************************************
int Logging_StartLoggingUsb(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t reply[1];
  // highDataRate = 0;
  LoggingService_StartLoggingUsb();
  reply[0] = 0x80;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}
//******************************************************************************
int Logging_StartLoggingFlash(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t reply[1];
  // highDataRate = 0;
  LoggingService_StartLoggingFlash();
  reply[0] = 0x80;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}
