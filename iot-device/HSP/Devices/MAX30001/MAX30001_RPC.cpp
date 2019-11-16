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

#include <stdio.h>
#include "USBSerial.h"
#include "StringHelper.h"
#include "MAX30001.h"
#include "Streaming.h"
#include "StringInOut.h"
#include "MAX30001_helper.h"
#include "RpcFifo.h"
#include "RpcServer.h"
#include "Peripherals.h"
#include "DataLoggingService.h"

extern int highDataRate;
extern USBSerial *usbSerialPtr;

uint32_t max30001_RegRead(MAX30001::MAX30001_REG_map_t addr) {
  uint32_t data;
  Peripherals::max30001()->reg_read(addr, &data);
  return data;
}

//******************************************************************************
void max30001_RegWrite(MAX30001::MAX30001_REG_map_t addr, uint32_t data) {
  Peripherals::max30001()->reg_write(addr, data);
}

int MAX30001_WriteReg(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t args[2];
  uint32_t reply[1];
  ProcessArgs32(argStrs, args, sizeof(args) / sizeof(uint32_t));

  max30001_RegWrite((MAX30001::MAX30001_REG_map_t)args[0], args[1]);
  reply[0] = 0x80;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int MAX30001_ReadReg(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t args[1];
  uint32_t reply[1];
  uint32_t value;
  ProcessArgs32(argStrs, args, sizeof(args) / sizeof(uint32_t));
  value = max30001_RegRead((MAX30001::MAX30001_REG_map_t)args[0]);
  reply[0] = value;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int MAX30001_Rbias_FMSTR_Init(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t args[5];
  uint32_t reply[1];
  uint32_t value;
  ProcessArgs32(argStrs, args, sizeof(args) / sizeof(uint32_t));

  value = Peripherals::max30001()->Rbias_FMSTR_Init(args[0],  ///< En_rbias
                                                    args[1],  ///< Rbiasv
                                                    args[2],  ///< Rbiasp
                                                    args[3],  ///< Rbiasn
                                                    args[4]); ///< Fmstr

  reply[0] = value;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int MAX30001_CAL_InitStart(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t args[6];
  uint32_t reply[1];
  uint32_t value;
  ProcessArgs32(argStrs, args, sizeof(args) / sizeof(uint32_t));

  // Peripherals::serial()->printf("MAX30001_CAL_InitStart 0 ");
  value = Peripherals::max30001()->CAL_InitStart(args[0],  ///< En_Vcal
                                                 args[1],  ///< Vmag
                                                 args[2],  ///< Fcal
                                                 args[3],  ///< Thigh
                                                 args[4],  ///< Fifty
                                                 args[5]); ///< Vmode

  reply[0] = value;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int MAX30001_ECG_InitStart(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t args[11];
  uint32_t reply[1];
  uint32_t value;
  ProcessArgs32(argStrs, args, sizeof(args) / sizeof(uint32_t));

  value = Peripherals::max30001()->ECG_InitStart(args[0], ///< En_ecg
                                                 args[1], ///< Openp
                                                 args[2], ///< Openn
                                                 args[3], ///< Pol
                                                 args[4], ///< Calp_sel
                                                 args[5], ///< Caln_sel
                                                 args[6], ///< E_fit
                                                 args[7], ///< Rate
                                                 args[8], ///< Gain
                                                 args[9],   ///< Dhpf
                                                 args[10]); ///< Dlpf

  MAX30001_Helper_SetStreamingFlag(eStreaming_ECG, 1);
  reply[0] = value;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int MAX30001_ECGFast_Init(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t args[3];
  uint32_t reply[1];
  uint32_t value;
  ProcessArgs32(argStrs, args, sizeof(args) / sizeof(uint32_t));

  value = Peripherals::max30001()->ECGFast_Init(args[0],  ///< Clr_Fast
                                                args[1],  ///< Fast
                                                args[2]); ///< Fast_Th

  reply[0] = value;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int MAX30001_PACE_InitStart(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t args[9];
  uint32_t reply[1];
  uint32_t value;
  ProcessArgs32(argStrs, args, sizeof(args) / sizeof(uint32_t));

  value = Peripherals::max30001()->PACE_InitStart(args[0],  ///< En_pace
                                                  args[1],  ///< Clr_pedge
                                                  args[2],  ///< Pol
                                                  args[3],  ///< Gn_diff_off
                                                  args[4],  ///< Gain
                                                  args[5],  ///< Aout_lbw
                                                  args[6],  ///< Aout
                                                  args[7],  ///< Dacp
                                                  args[8]); ///< Dacn


  MAX30001_Helper_SetStreamingFlag(eStreaming_PACE, 1);
  reply[0] = value;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int MAX30001_BIOZ_InitStart(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t args[17];
  uint32_t reply[1];
  uint32_t value;
  ProcessArgs32(argStrs, args, sizeof(args) / sizeof(uint32_t));

  value = Peripherals::max30001()->BIOZ_InitStart(args[0],   ///< En_bioz
                                                  args[1],   ///< Openp
                                                  args[2],   ///< Openn
                                                  args[3],   ///< Calp_sel
                                                  args[4],   ///< Caln_sel
                                                  args[5],   ///< CG_mode
                                                  args[6],   ///< B_fit
                                                  args[7],   ///< Rate
                                                  args[8],   ///< Ahpf
                                                  args[9],   ///< Ext_rbias
                                                  args[10],  ///< Gain
                                                  args[11],  ///< Dhpf
                                                  args[12],  ///< Dlpf
                                                  args[13],  ///< Fcgen
                                                  args[14],  ///< Cgmon
                                                  args[15],  ///< Cgmag
                                                  args[16]); ///< Phoff

  MAX30001_Helper_SetStreamingFlag(eStreaming_BIOZ, 1);
  reply[0] = value;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int MAX30001_RtoR_InitStart(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t args[9];
  uint32_t reply[1];
  uint32_t value;
  ProcessArgs32(argStrs, args, sizeof(args) / sizeof(uint32_t));

  value =  Peripherals::max30001()->RtoR_InitStart(args[0], ///< En_rtor
                                                   args[1], ///< Wndw
                                                   args[2], ///< Gain
                                                   args[3], ///< Pavg
                                                   args[4], ///< Ptsf
                                                   args[5], ///< Hoff
                                                   args[6], ///< Ravg
                                                   args[7], ///< Rhsf
                                                   args[8]); ///< Clr_rrint

  MAX30001_Helper_SetStreamingFlag(eStreaming_RtoR, 1);
  reply[0] = value;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int MAX30001_Stop_ECG(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t reply[1];
  Peripherals::max30001()->Stop_ECG();
  reply[0] = 0x80;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int MAX30001_Stop_PACE(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t reply[1];
  Peripherals::max30001()->Stop_PACE();
  reply[0] = 0x80;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int MAX30001_Stop_BIOZ(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t reply[1];
  Peripherals::max30001()->Stop_BIOZ();
  reply[0] = 0x80;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int MAX30001_Stop_RtoR(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t reply[1];
  Peripherals::max30001()->Stop_RtoR();
  reply[0] = 0x80;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int MAX30001_Stop_Cal(char argStrs[32][32], char replyStrs[32][32]) {

  uint32_t reply[1];

  reply[0] = 0x80;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
void max30001_ServiceStreaming() {
  //char ch;
  uint32_t val;

  fifo_clear(GetStreamOutFifo());

  SetStreaming(TRUE);
  clearOutReadFifo();
  while (IsStreaming() == TRUE) {

    if (fifo_empty(GetStreamOutFifo()) == 0) {
      fifo_get32(GetStreamOutFifo(), &val);

      usbSerialPtr->printf("%02X ", val);

    }
    if (usbSerialPtr->available()) {
      usbSerialPtr->_getc();

      MAX30001_Helper_Stop();
      SetStreaming(FALSE);
      fifo_clear(GetUSBIncomingFifo()); ///< clear USB serial incoming fifo
      fifo_clear(GetStreamOutFifo());
    }

  }
}

//******************************************************************************
int MAX30001_Start(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t reply[1];
  uint32_t all;
  fifo_clear(GetUSBIncomingFifo());
  Peripherals::max30001()->synch();
  ///<	max30001_ServiceStreaming();
  highDataRate = 0;
  Peripherals::max30001()->reg_read(MAX30001::STATUS, &all);
  LoggingService_StartLoggingUsb();

  reply[0] = 0x80;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int MAX30001_Stop(char argStrs[32][32], char replyStrs[32][32]) {
  /*	uint32_t args[1];
          uint32_t reply[1];
          uint32_t value;
          //ProcessArgs32(argStrs, args, sizeof(args) / sizeof(uint32_t));
          max30001_StopTest();
          reply[0] = 0x80;
          FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);*/
  return 0;
}

//******************************************************************************
int MAX30001_INT_assignment(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t args[17];
  uint32_t reply[1];
  ProcessArgs32(argStrs, args, sizeof(args) / sizeof(uint32_t));

  Peripherals::max30001()->INT_assignment(
      (MAX30001::max30001_intrpt_Location_t)args[0],
      (MAX30001::max30001_intrpt_Location_t)args[1],
      (MAX30001::max30001_intrpt_Location_t)args[2],
      (MAX30001::max30001_intrpt_Location_t)args[3],
      (MAX30001::max30001_intrpt_Location_t)args[4],
      (MAX30001::max30001_intrpt_Location_t)args[5],
      (MAX30001::max30001_intrpt_Location_t)args[6],
      (MAX30001::max30001_intrpt_Location_t)args[7],
      (MAX30001::max30001_intrpt_Location_t)args[8],
      (MAX30001::max30001_intrpt_Location_t)args[9],
      (MAX30001::max30001_intrpt_Location_t)args[10],
      (MAX30001::max30001_intrpt_Location_t)args[11],
      (MAX30001::max30001_intrpt_Location_t)args[12],
      (MAX30001::max30001_intrpt_Location_t)args[13],
      (MAX30001::max30001_intrpt_Location_t)args[14],
      (MAX30001::max30001_intrpt_type_t)args[15],
      (MAX30001::max30001_intrpt_type_t)args[16]);
  reply[0] = 0x80;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int MAX30001_StartTest(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t reply[1];
  // ProcessArgs32(argStrs, args, sizeof(args) / sizeof(uint32_t));

  /*** Set FMSTR over here ****/

  /*** Set and Start the VCAL input ***/
  /* NOTE VCAL must be set first if VCAL is to be used */
  Peripherals::max30001()->CAL_InitStart(0b1, 0b1, 0b1, 0b011, 0x7FF, 0b0);

  /**** ECG Initialization ****/
  Peripherals::max30001()->ECG_InitStart(0b1, 0b1, 0b1, 0b0, 0b10, 0b11, 31, 0b00, 0b00, 0b0, 0b01);

  /***** PACE Initialization ***/
  Peripherals::max30001()->PACE_InitStart(0b1, 0b0, 0b0, 0b1, 0b000, 0b0, 0b00, 0b0, 0b0);

  /**** BIOZ Initialization ****/
  Peripherals::max30001()->BIOZ_InitStart(
      0b1, 0b1, 0b1, 0b10, 0b11, 0b00, 7, 0b0, 0b111, 0b0, 0b10, 0b00, 0b00, 0b0001, 0b0, 0b111, 0b0000);

  /*** Set RtoR registers ***/
  Peripherals::max30001()->RtoR_InitStart(
      0b1, 0b0011, 0b1111, 0b00, 0b0011, 0b000001, 0b00, 0b000, 0b01);

  /*** Set Rbias & FMSTR over here ****/
  Peripherals::max30001()->Rbias_FMSTR_Init(0b01, 0b10, 0b1, 0b1, 0b00);

  /**** Interrupt Setting  ****/

  /*** Set ECG Lead ON/OFF ***/
  //     max30001_ECG_LeadOnOff();

  /*** Set BIOZ Lead ON/OFF ***/
  //     max30001_BIOZ_LeadOnOff();  Does not work yet...

  /**** Do a Synch ****/
  Peripherals::max30001()->synch();

  fifo_clear(GetUSBIncomingFifo());
  max30001_ServiceStreaming();

  reply[0] = 0x80;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int MAX30001_Enable_ECG_LeadON(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t reply[1];
  ///< switch to ECG DC Lead ON
  Peripherals::max30001()->Enable_LeadON(0b01);
  reply[0] = 0x80;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int MAX30001_Enable_BIOZ_LeadON(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t reply[1];
  ///< switch to BIOZ DC Lead ON
  Peripherals::max30001()->Enable_LeadON(0b10);
  reply[0] = 0x80;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}

//******************************************************************************
int MAX30001_Read_LeadON(char argStrs[32][32], char replyStrs[32][32]) {
  uint32_t reply[1];
  ///< return the max30001_LeadOn var from the MAX30001 driver
  reply[0] = Peripherals::max30001()->max30001_LeadOn;
  FormatReply32(reply, sizeof(reply) / sizeof(uint32_t), replyStrs);
  return 0;
}
