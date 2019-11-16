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
#ifndef _RPCSERVER_H_
#define _RPCSERVER_H_

#include "mbed.h"
#include "RpcFifo.h"
#include "Device_Logging.h"

/**
* @brief Reads the Version of the HSP FCache_Writel
*/
int System_ReadVer(char argStrs[32][32], char replyStrs[32][32]);
/**
* @brief Reads the built time of the RPC FW
*/
int System_ReadBuildTime(char argStrs[32][32], char replyStrs[32][32]);

void RPC__init(void);
void RPC__call(char *request, char *reply);
fifo_t *GetUSBIncomingFifo(void);
fifo_t *GetStreamOutFifo(void);

/**
* @brief Batch process RPC commands
*/
void RPC_ProcessCmds(char *cmds);

/**
* @brief Initialize the RPC server with all of the commands that it supports
*/
void RPC_init(void);

/**
* @brief Initialize the RPC server with all of the commands that it supports
*/
void RPC_call(char *request, char *reply);

/**
* @brief Query to determine if flash pages are being transfered via RPC
*/
bool RPC_IsTransferingFlashPages(void);

/**
* @brief Called to indicate that a flash page is being transfered via RPC, this reset a timer to detect multiple page transfers over time
*/
void RPC_TransferingFlashPages(void) ;

#endif // _RPCSERVER_H_ 
