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
#ifndef _LOGGING_H_
#define _LOGGING_H_

int Logging_RPC_StartMissionDefine(char argStrs[32][32], char replyStrs[32][32]);
int Logging_RPC_AppendMissionCmd(char argStrs[32][32], char replyStrs[32][32]);
int Logging_RPC_EndMissionDefine(char argStrs[32][32], char replyStrs[32][32]);
int Logging_RPC_WriteMission(char argStrs[32][32], char replyStrs[32][32]);
int Logging_RPC_ReadMission(char argStrs[32][32], char replyStrs[32][32]);
int Logging_RPC_EraseMission(char argStrs[32][32], char replyStrs[32][32]);
int Logging_RPC_TestMission(char argStrs[32][32], char replyStrs[32][32]);
int Logging_RPC_TestWriteLog(char argStrs[32][32], char replyStrs[32][32]);
int Logging_RPC_TestReadLog(char argStrs[32][32], char replyStrs[32][32]);
int Logging_RPC_TestBulkErase(char argStrs[32][32], char replyStrs[32][32]);
int Logging_RPC_TestSectorsErase(char argStrs[32][32], char replyStrs[32][32]);
int Logging_RPC_TestReadPage(char argStrs[32][32], char replyStrs[32][32]);
int Logging_RPC_TestWritePage(char argStrs[32][32], char replyStrs[32][32]);
int Logging_EraseWrittenSectors(char argStrs[32][32], char replyStrs[32][32]);
int Logging_StartLoggingUsb(char argStrs[32][32], char replyStrs[32][32]);
int Logging_StartLoggingFlash(char argStrs[32][32], char replyStrs[32][32]);
int Logging_GetLastWrittenPage(char argStrs[32][32], char replyStrs[32][32]);
int Logging_Start(char argStrs[32][32], char replyStrs[32][32]);

#endif /* _LOGGING_H_ */
