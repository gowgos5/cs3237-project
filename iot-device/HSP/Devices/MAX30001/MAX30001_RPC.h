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
#ifndef __MAX300001_RPC_H
#define __MAX300001_RPC_H

int MAX30001_WriteReg(char argStrs[32][32], char replyStrs[32][32]);
int MAX30001_ReadReg(char argStrs[32][32], char replyStrs[32][32]);
int MAX30001_Start(char argStrs[32][32], char replyStrs[32][32]);
int MAX30001_Stop(char argStrs[32][32], char replyStrs[32][32]);
int MAX30001_Rbias_FMSTR_Init(char argStrs[32][32], char replyStrs[32][32]);
int MAX30001_CAL_InitStart(char argStrs[32][32], char replyStrs[32][32]);
int MAX30001_ECG_InitStart(char argStrs[32][32], char replyStrs[32][32]);
int MAX30001_ECGFast_Init(char argStrs[32][32], char replyStrs[32][32]);
int MAX30001_PACE_InitStart(char argStrs[32][32], char replyStrs[32][32]);
int MAX30001_BIOZ_InitStart(char argStrs[32][32], char replyStrs[32][32]);
int MAX30001_RtoR_InitStart(char argStrs[32][32], char replyStrs[32][32]);
int MAX30001_Stop_ECG(char argStrs[32][32], char replyStrs[32][32]);
int MAX30001_Stop_PACE(char argStrs[32][32], char replyStrs[32][32]);
int MAX30001_Stop_BIOZ(char argStrs[32][32], char replyStrs[32][32]);
int MAX30001_Stop_RtoR(char argStrs[32][32], char replyStrs[32][32]);
int MAX30001_Stop_Cal(char argStrs[32][32], char replyStrs[32][32]);
int MAX30001_Enable_ECG_LeadON(char argStrs[32][32], char replyStrs[32][32]);
int MAX30001_Enable_BIOZ_LeadON(char argStrs[32][32], char replyStrs[32][32]);
int MAX30001_Read_LeadON(char argStrs[32][32], char replyStrs[32][32]);

int MAX30001_StartTest(char argStrs[32][32], char replyStrs[32][32]);
int MAX30001_INT_assignment(char argStrs[32][32], char replyStrs[32][32]);

#endif /* __MAX300001_RPC_H */
