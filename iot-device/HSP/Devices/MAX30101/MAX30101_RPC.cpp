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
#include "MAX30101_RPC.h"
#include "StringHelper.h"
#include "MAX30101.h"
#include "Streaming.h"
#include "RpcFifo.h"
#include "RpcServer.h"
#include "StringInOut.h"
#include "MAX30101_helper.h"
#include "LIS2DH.h"
#include "Peripherals.h"
#include "DataLoggingService.h"

//******************************************************************************
int MAX30101_WriteReg(char argStrs[32][32], char replyStrs[32][32]) {
	uint8_t args[2];
	uint8_t reply[1];
	ProcessArgs(argStrs, args, sizeof(args));
	reply[0] = 0x80;
	FormatReply(reply, sizeof(reply), replyStrs);
	return 0;
}

//******************************************************************************
int MAX30101_ReadReg(char argStrs[32][32], char replyStrs[32][32]) {
	uint8_t args[1];
	uint8_t reply[1];
	char value = 0;
	ProcessArgs(argStrs, args, sizeof(args));
	reply[0] = (uint8_t)value;
	FormatReply(reply, sizeof(reply), replyStrs);
	return 0;
}

//******************************************************************************
int MAX30101_COMMinit(char argStrs[32][32], char replyStrs[32][32])
{
	uint8_t args[4];
	uint8_t return_value = 0;
	ProcessArgs(argStrs,args,sizeof(args));

	strcpy(replyStrs[0],"80");
	return return_value;
}

//******************************************************************************
int MAX30101_SpO2mode_stop(char argStrs[32][32], char replyStrs[32][32])
{
	uint8_t args[4];
	uint8_t return_value = 0;
	ProcessArgs(argStrs,args,sizeof(args));

	Peripherals::max30101()->SpO2mode_stop();

	strcpy(replyStrs[0],"80");
	return return_value;
}

//******************************************************************************
int MAX30101_HRmode_stop(char argStrs[32][32], char replyStrs[32][32])
{
	uint8_t args[4];
	ProcessArgs(argStrs,args,sizeof(args));

	Peripherals::max30101()->HRmode_stop();

	strcpy(replyStrs[0],"80");
	return 0;
}

//******************************************************************************
int MAX30101_Multimode_stop(char argStrs[32][32], char replyStrs[32][32])
{
	uint8_t args[4];
	ProcessArgs(argStrs,args,sizeof(args));

	Peripherals::max30101()->Multimode_stop();

	strcpy(replyStrs[0],"80");
	return 0;
}

//******************************************************************************
void ClearInOutFifos() {
	fifo_clear(GetStreamOutFifo());	   ///< clear stream out fifo
	fifo_clear(GetUSBIncomingFifo());  ///< clear USB serial incoming fifo
}

int highDataRate = 0;
#define MAX30101_200HZ	2
#define MAX30101_400HZ	3

/**
* Adjust the threshold of the Accel based on the data rate
* @lis2dhDataRate Data rate of the accel to determine what would be a good threshold value
*/
static int adjustAccelThreshold(int lis2dhDataRate) {
	int lis2dhFifoThreshold;
	lis2dhFifoThreshold = 31;
	if (lis2dhDataRate <= LIS2DH_DATARATE_50HZ) lis2dhFifoThreshold = 4; 
	if (lis2dhDataRate == LIS2DH_DATARATE_100HZ) lis2dhFifoThreshold = 8; 
	return lis2dhFifoThreshold;
}

/**
* Evaluate the data rate of the Accel to see if we should stream using a high data rate mode
* @lis2dhDataRate Data rate of the accel to determine if this is a high data rate
*/
static void adjustHighDataRate(int lis2dhDataRate) {
	highDataRate = 0;
	if (lis2dhDataRate >= LIS2DH_DATARATE_200HZ) highDataRate = 1;
}

//******************************************************************************
int MAX30101_SpO2mode_init(char argStrs[32][32], char replyStrs[32][32])
{
	uint8_t args[7];
	uint8_t lis2dhDataRate;
	uint8_t lis2dhFifoThreshold;
	ProcessArgs(argStrs, args, sizeof(args));

	ClearInOutFifos();
	MAX30101_Helper_SetStreamingFlag(eStreaming_SPO2,1);
	Peripherals::max30101()->SpO2mode_init(args[0], args[1], args[2], args[3], args[4], args[5]);

	lis2dhDataRate = args[6];
	lis2dhFifoThreshold = adjustAccelThreshold(lis2dhDataRate);
	adjustHighDataRate(lis2dhDataRate);

	Peripherals::lis2dh()->initStart(lis2dhDataRate,lis2dhFifoThreshold);
	LoggingService_StartLoggingUsb();

	strcpy(replyStrs[0],"80");
	return 0;
}

//******************************************************************************
int MAX30101_SpO2mode_InitStart(char argStrs[32][32], char replyStrs[32][32])
{
	uint8_t args[6];   // Size
	ProcessArgs(argStrs, args, sizeof(args));

	ClearInOutFifos();
	MAX30101_Helper_SetStreamingFlag(eStreaming_SPO2,1);
	if (args[2] >= MAX30101_200HZ) {
      highDataRate = 1;
    }
	Peripherals::max30101()->SpO2mode_init(args[0], args[1], args[2], args[3], args[4], args[5]);

	strcpy(replyStrs[0],"80");
	return 0;
}

//******************************************************************************
int MAX30101_HRmode_init(char argStrs[32][32], char replyStrs[32][32])
{
	uint8_t args[6];
	uint8_t lis2dhDataRate;
	uint8_t lis2dhFifoThreshold;
	ProcessArgs(argStrs, args, sizeof(args));

	ClearInOutFifos();
	MAX30101_Helper_SetStreamingFlag(eStreaming_HR,1);
	Peripherals::max30101()->HRmode_init(args[0], args[1], args[2], args[3], args[4]);

	lis2dhDataRate = args[5];
	lis2dhFifoThreshold = adjustAccelThreshold(lis2dhDataRate);
	adjustHighDataRate(lis2dhDataRate);
	
	Peripherals::lis2dh()->initStart(lis2dhDataRate,lis2dhFifoThreshold);
	LoggingService_StartLoggingUsb();
	strcpy(replyStrs[0],"80");
	return 0;
}

//******************************************************************************
int MAX30101_HRmode_InitStart(char argStrs[32][32], char replyStrs[32][32])
{
	uint8_t args[5];
	ProcessArgs(argStrs, args, sizeof(args));

	ClearInOutFifos();
	MAX30101_Helper_SetStreamingFlag(eStreaming_HR,1);
	if (args[2] >= MAX30101_200HZ) {
      highDataRate = 1;
    }
	Peripherals::max30101()->HRmode_init(args[0], args[1], args[2], args[3], args[4]);
	strcpy(replyStrs[0],"80");
	return 0;
}

//******************************************************************************
int MAX30101_Multimode_init(char argStrs[32][32], char replyStrs[32][32])
{
	uint8_t args[12];
	uint8_t lis2dhDataRate;
	uint8_t lis2dhFifoThreshold;
	ProcessArgs(argStrs, args, sizeof(args));

	ClearInOutFifos();

	lis2dhDataRate = args[11];
	lis2dhFifoThreshold = adjustAccelThreshold(lis2dhDataRate);
	adjustHighDataRate(lis2dhDataRate);
	
	Peripherals::lis2dh()->initStart(lis2dhDataRate,lis2dhFifoThreshold);

	wait(0.1f);
	MAX30101_Helper_SetStreamingFlag(eStreaming_MULTI, 1);
	Peripherals::max30101()->Multimode_init(args[0], args[1], args[2],
											args[3], args[4], args[5], args[6],
											args[7], args[8], args[9], args[10]);
	LoggingService_StartLoggingUsb();

	strcpy(replyStrs[0],"80");
	return 0;
}

//******************************************************************************
int MAX30101_Multimode_InitStart(char argStrs[32][32], char replyStrs[32][32])
{
	uint8_t args[11];
	ProcessArgs(argStrs, args, sizeof(args));

	ClearInOutFifos();
	MAX30101_Helper_SetStreamingFlag(eStreaming_MULTI, 1);
	if (args[2] >= MAX30101_200HZ) {
      highDataRate = 1;
    }
	Peripherals::max30101()->Multimode_init(args[0], args[1], args[2],
											args[3], args[4], args[5], args[6],
											args[7], args[8], args[9], args[10]);

	strcpy(replyStrs[0],"80");
	return 0;
}



