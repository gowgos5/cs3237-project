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
#include "RpcServer.h"
#include "StringInOut.h"
#include "StringHelper.h"
#include "MAX30001_RPC.h"
#include "MAX30101_RPC.h"
#include "LIS2DH_RPC.h"
#include "Logging_RPC.h"
#include "Peripherals.h"
#include "I2C_RPC.h"
#include "BMP280_RPC.h"
#include "MAX30205_RPC.h"
#include "HspLed_RPC.h"
#include "S25FS512_RPC.h"
#include "Testing_RPC.h"
#include "MAX14720_RPC.h"
#include "RpcDeclarations.h"
#include "Device_Logging.h"

/// define the version string that is reported with a RPC "ReadVer" command
#define FW_VERSION_STRING "HSP FW Version 3.0.1 4/21/16"

char args[32][32];
char results[32][32];

/// define a fifo for incoming USB data
static fifo_t fifo;
/// define a buffer for incoming USB data
static uint8_t fifoBuffer[128];
/// define stream out fifo
static fifo_t fifoStreamOut;
/// allocate a large fifo buffer for streaming out
static uint32_t streamOutBuffer[0xC000 / 4];

/// define a device log for the BMP280, keeps track of mission and loggin status
Device_Logging *bmp280_Logging;
/// define a device log for the MAX30205 (instance 0), keeps track of mission
/// and loggin status
Device_Logging *MAX30205_0_Logging;
/// define a device log for the MAX30205 (instance 1), keeps track of mission
/// and loggin status
Device_Logging *MAX30205_1_Logging;

///Defines a timer to detect if a series of binary pages are being transfered via RPC
Timer rpcFlashPagesTransferTimer;
bool rpcFlashPagesTransferTimer_running;

//******************************************************************************
fifo_t *GetUSBIncomingFifo(void) { return &fifo; }

//******************************************************************************
fifo_t *GetStreamOutFifo(void) { return &fifoStreamOut; }

//******************************************************************************
int System_ReadVer(char argStrs[32][32], char replyStrs[32][32]) {
  strcpy(replyStrs[0], FW_VERSION_STRING);
  strcpy(replyStrs[1], "\0");
  return 0;
}

//******************************************************************************
bool RPC_IsTransferingFlashPages(void) {
  if (rpcFlashPagesTransferTimer_running == true) {
    if (rpcFlashPagesTransferTimer.read_ms() < 200) {
      return true;
    } else {
      rpcFlashPagesTransferTimer.stop();
      rpcFlashPagesTransferTimer_running = false;
    }
  }
  return false;
}

//******************************************************************************
void RPC_TransferingFlashPages(void) {
  if (rpcFlashPagesTransferTimer_running == false) {
    rpcFlashPagesTransferTimer.start();
    rpcFlashPagesTransferTimer_running = true;
  }
  rpcFlashPagesTransferTimer.reset();
}

//******************************************************************************
int System_ReadBuildTime(char argStrs[32][32], char replyStrs[32][32]) {
  // strcpy(replyStrs[0],buildTime);
  // strcpy(replyStrs[1],"\0");
  return 0;
}

//******************************************************************************
int System_SystemCoreClock(char argStrs[32][32], char replyStrs[32][32]) {
  sprintf(replyStrs[0], "SystemCoreClock = %d", (unsigned int)SystemCoreClock);
  strcpy(replyStrs[1], "\0");
  return 0;
}

//******************************************************************************
int System_GetTimestamp(char argStrs[32][32], char replyStrs[32][32]) {
  sprintf(replyStrs[0], "GetTimestamp = %d", 0);
  strcpy(replyStrs[1], "\0");
  return 0;
}

static struct RPC_Object RPC_Procedures = {NULL, NULL};

//******************************************************************************
void RPC_addProcedure(struct RPC_registeredProcedure *procedure) {
  struct RPC_Object *obj = &RPC_Procedures;
  if (obj->last != NULL) {
    obj->last->next = procedure;
  }
  if (obj->head == NULL) {
    obj->head = procedure;
  }
  procedure->next = NULL;
  obj->last = procedure;
}

//******************************************************************************
void RPC_init(void) {
  bmp280_Logging = new Device_Logging();
  MAX30205_0_Logging = new Device_Logging();
  MAX30205_1_Logging = new Device_Logging();

  rpcFlashPagesTransferTimer_running = false;

  fifo_init(&fifo, fifoBuffer, sizeof(fifoBuffer));
  fifo_init(&fifoStreamOut, streamOutBuffer,
            sizeof(streamOutBuffer) / sizeof(uint32_t));

  // I2c
  RPC_addProcedure(&Define_I2c_WriteRead);

  // MAX30101
  RPC_addProcedure(&Define_MAX30101_WriteReg);
  RPC_addProcedure(&Define_MAX30101_ReadReg);
  RPC_addProcedure(&Define_MAX30101_SpO2mode_Init);
  RPC_addProcedure(&Define_MAX30101_HRmode_Init);
  RPC_addProcedure(&Define_MAX30101_Multimode_init);
  RPC_addProcedure(&Define_MAX30101_SpO2mode_InitStart);
  RPC_addProcedure(&Define_MAX30101_HRmode_InitStart);
  RPC_addProcedure(&Define_MAX30101_Multimode_InitStart);
  RPC_addProcedure(&Define_MAX30101_SpO2mode_stop);
  RPC_addProcedure(&Define_MAX30101_HRmode_stop);
  RPC_addProcedure(&Define_MAX30101_Multimode_stop);

  // MAX30001
  RPC_addProcedure(&Define_MAX30001_WriteReg);
  RPC_addProcedure(&Define_MAX30001_ReadReg);
  RPC_addProcedure(&Define_MAX30001_Start);
  RPC_addProcedure(&Define_MAX30001_Stop);
  RPC_addProcedure(&Define_MAX30001_Enable_ECG_LeadON);
  RPC_addProcedure(&Define_MAX30001_Enable_BIOZ_LeadON);
  RPC_addProcedure(&Define_MAX30001_Read_LeadON);
  RPC_addProcedure(&Define_MAX30001_StartTest);
  RPC_addProcedure(&Define_MAX30001_INT_assignment);
  RPC_addProcedure(&Define_MAX30001_Rbias_FMSTR_Init);
  RPC_addProcedure(&Define_MAX30001_CAL_InitStart);
  RPC_addProcedure(&Define_MAX30001_ECG_InitStart);
  RPC_addProcedure(&Define_MAX30001_ECGFast_Init);
  RPC_addProcedure(&Define_MAX30001_PACE_InitStart);
  RPC_addProcedure(&Define_MAX30001_BIOZ_InitStart);
  RPC_addProcedure(&Define_MAX30001_RtoR_InitStart);
  RPC_addProcedure(&Define_MAX30001_Stop_ECG);
  RPC_addProcedure(&Define_MAX30001_Stop_PACE);
  RPC_addProcedure(&Define_MAX30001_Stop_BIOZ);
  RPC_addProcedure(&Define_MAX30001_Stop_RtoR);
  RPC_addProcedure(&Define_MAX30001_Stop_Cal);

  // Logging
  RPC_addProcedure(&Define_Logging_StartMissionDefine);
  RPC_addProcedure(&Define_Logging_AppendMissionCmd);
  RPC_addProcedure(&Define_Logging_EndMissionDefine);
  RPC_addProcedure(&Define_Logging_WriteMission);
  RPC_addProcedure(&Define_Logging_ReadMission);
  RPC_addProcedure(&Define_Logging_EraseMission);
  RPC_addProcedure(&Define_Logging_EraseWrittenSectors);
  RPC_addProcedure(&Define_Logging_StartLoggingUsb);
  RPC_addProcedure(&Define_Logging_StartLoggingFlash);
  RPC_addProcedure(&Define_Logging_GetLastWrittenPage);
  RPC_addProcedure(&Define_Logging_Start);

  // LIS2HD
  RPC_addProcedure(&Define_LIS2DH_InitStart);
  RPC_addProcedure(&Define_LIS2DH_ReadReg);
  RPC_addProcedure(&Define_LIS2DH_WriteReg);
  RPC_addProcedure(&Define_LIS2DH_Stop);

  // BMP280
  RPC_addProcedure(&Define_BMP280_InitStart);

  // MAX30205 and MAX31725 Alias
  RPC_addProcedure(&Define_MAX30205_1_InitStart);
  RPC_addProcedure(&Define_MAX30205_2_InitStart);
  RPC_addProcedure(&Define_MAX31725_1_InitStart);
  RPC_addProcedure(&Define_MAX31725_2_InitStart);

  // led
  RPC_addProcedure(&Define_Led_On);
  RPC_addProcedure(&Define_Led_Off);
  RPC_addProcedure(&Define_Led_BlinkHz);
  RPC_addProcedure(&Define_Led_BlinkPattern);

  // S25FS512
  RPC_addProcedure(&Define_S25FS512_ReadId);
  RPC_addProcedure(&Define_S25FS512_ReadPagesBinary);
  RPC_addProcedure(&Define_S25FS512_Reset);
  RPC_addProcedure(&Define_S25FS512_EnableHWReset);
  RPC_addProcedure(&Define_S25FS512_SpiWriteRead);
  RPC_addProcedure(&Define_S25FS512_SpiWriteRead4Wire);

  // Testing
  RPC_addProcedure(&Define_Testing_Test_S25FS512);
  RPC_addProcedure(&Define_Testing_Test_BMP280);
  RPC_addProcedure(&Define_Testing_Test_LIS2DH);
  RPC_addProcedure(&Define_Testing_Test_LSM6DS3);
  RPC_addProcedure(&Define_Testing_Test_MAX30205_1);
  RPC_addProcedure(&Define_Testing_Test_MAX30205_2);
  RPC_addProcedure(&Define_Testing_Test_MAX30101);
  RPC_addProcedure(&Define_Testing_Test_MAX30001);
  RPC_addProcedure(&Define_Testing_Test_EM9301);

  // System
  RPC_addProcedure(&Define_System_ReadVer);
  RPC_addProcedure(&Define_System_ReadBuildTime);

  // MAX14720
  RPC_addProcedure(&Define_MAX14720_ReadBoostVSet);
  RPC_addProcedure(&Define_MAX14720_WriteBoostVSet);
  RPC_addProcedure(&Define_MAX14720_ReadReg);
  RPC_addProcedure(&Define_MAX14720_WriteReg);
}

//******************************************************************************
struct RPC_registeredProcedure *RPC_lookup(char *objectName, char *methodName) {
  struct RPC_registeredProcedure *ptr;
  // lookup all registered methods
  ptr = RPC_Procedures.head;
  while (ptr != NULL) {
    if (strcmp(ptr->objectName, objectName) == 0 &&
        strcmp(ptr->methodName, methodName) == 0) {
      // we found a match... return with it
      return ptr;
    }
    ptr = ptr->next;
  }
  return NULL;
}

//******************************************************************************
char *GetToken(char *inStr, char *outStr, int start, char ch) {
  int i;
  int index = 0;
  int length = strlen(inStr);
  for (i = start; i < length; i++) {
    if (inStr[i] != ch) {
      outStr[index++] = inStr[i];
    } else {
      break;
    }
  }
  outStr[index++] = 0;
  return outStr;
}

//******************************************************************************
void SendCommandList(char *reply) {
  struct RPC_registeredProcedure *ptr;
  reply[0] = 0;
  ptr = RPC_Procedures.head;
  while (ptr != NULL) {
    strcat(reply, "/");
    strcat(reply, ptr->objectName);
    strcat(reply, "/");
    strcat(reply, ptr->methodName);
    strcat(reply, ",");
    ptr = ptr->next;
  }
  strcat(reply, "\r\n");
}

//******************************************************************************
int CheckForDoubleQuote(const char *str) {
  int doubleQuoteFound;
  // scan through arguments, see if there is a double quote for a string
  // argument
  doubleQuoteFound = 0;
  while (*str != 0) {
    if (*str == '\"') {
      doubleQuoteFound = 1;
      break;
    }
    str++;
  }
  return doubleQuoteFound;
}

//******************************************************************************
void ExtractDoubleQuoteStr(const char *src, char *dst) {
  int start;

  dst[0] = 0;
  start = 0;
  while (*src != 0) {
    // look for start
    if ((*src == '\"') && (start == 0)) {
      start = 1;
      src++;
      continue;
    }
    // look for end
    if ((*src == '\"') && (start == 1)) {
      *dst = 0; // terminate the string
      break;
    }
    if (start == 1) {
      *dst = *src;
      dst++;
    }
    src++;
  }
}

//******************************************************************************
void RPC_call_test(void) {
  int doubleQuoteFound;
  char doubleQuoteStr[64];
  const char *request = "/Logging/AppendMissionCmd \"BMP280 InitStart 1\"";

  // scan through arguments, see if there is a double quote for a string
  // argument
  doubleQuoteFound = CheckForDoubleQuote(request);
  if (doubleQuoteFound) {
    ExtractDoubleQuoteStr(request, doubleQuoteStr);
  }
}

//******************************************************************************
void RPC_call(char *request, char *reply) {
  const char slash[2] = "/";
  const char space[2] = " ";
  char *objectName;
  char *methodName;
  char doubleQuoteStr[64];
  char requestCpy[128];
  char *token;
  int argIndex;
  int resultIndex;
  int doubleQuoteFound;
  struct RPC_registeredProcedure *procedurePtr;
  int callResult;

  // clear out the reply
  reply[0] = 0;
  // copy the request for scanning and extraction later
  strcpy(requestCpy, request);
  // check for beginning forward slash
  if (request[0] != '/') {
    return;
  }
  // check for only a forward slash
  if (request[0] == '/' && request[1] == 0) {
    SendCommandList(reply);
    return;
  }
  strcat(request, " ");
  // get the object name
  token = strtok(request, slash);
  // token = GetToken(request, tokenBuffer, 1, '/');
  objectName = token;
  if (objectName == NULL)
    return; // must have an object name
  // get the method name
  token = strtok(NULL, space);
  methodName = token;
  if (methodName == NULL)
    return; // must have a method name

  // scan through arguments, see if there is a double quote for a string
  // argument
  doubleQuoteFound = CheckForDoubleQuote(requestCpy);

  if (doubleQuoteFound == 0) {
    // walk through arguments
    argIndex = 0;
    token = strtok(NULL, space);
    while (token != NULL) {
      // save this arg in array
      strcpy(args[argIndex++], token);
      // read next token arg if any
      token = strtok(NULL, space);
    }
    // terminate the end of the string array with an empty string
    strcpy(args[argIndex], "\0");
    strcpy(results[0], "\0");
  } else {
    // grab out the double quote string
    ExtractDoubleQuoteStr(requestCpy, doubleQuoteStr);
    argIndex = 0;
    // token = strtok(NULL, quote);
    strcpy(args[argIndex++], doubleQuoteStr);
  }

  //
  // alias the MAX30001 and MAX30003 names
  //
  if (strcmp(objectName, MAX30003_NAME) == 0) {
    strcpy(objectName, MAX30001_NAME);
  }

  callResult = 0;
  procedurePtr = RPC_lookup(objectName, methodName);
  if (procedurePtr != NULL) {
    // printf("RPC_call: %s processing\n",requestCpy);
    callResult = procedurePtr->func(args, results);
  } else {
    printf("RPC_call: %s not found\n", requestCpy);
    // printf("Unable to lookup %s %s", objectName, methodName);
  }

  // loop while (if) there are results to return
  resultIndex = 0;
  strcpy(reply, "\0");
  if (callResult != RPC_SKIP_CRLF) { 
    while (results[resultIndex][0] != '\0') {
      strcat(reply, results[resultIndex++]);
      strcat(reply, " ");
    }
    strcat(reply, "\r\n");
  }
}

//******************************************************************************
void RPC_ProcessCmds(char *cmds) {
  char cmd[32 * 32];
  char *ptrCmds;
  char reply[512];
  ptrCmds = cmds;

  while (*ptrCmds != 0) {
    ptrCmds = ParseUntilCRLF(ptrCmds, cmd, sizeof(cmd));
    if (*cmd != 0) {
      RPC_call(cmd, reply);
    }
  }
}
