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

#include "LIS2DH.h"
#include "Streaming.h"
#include "Peripherals.h"

void lis2dh_int_handler(void);
 /** 
  * @brief buffer array to hold fifo contents for packetizing
  */
uint32_t lis2dh_buffer[LIS2DH_MAX_DATA_SIZE];

int16_t motion_cached[3];
LIS2DH *LIS2DH::instance = NULL;

//******************************************************************************
LIS2DH::LIS2DH(PinName sda, PinName scl, int slaveAddress) : 
        slaveAddress(slaveAddress) {
  i2c = new I2C(sda, scl);
  i2c->frequency(400000);
  isOwner = true;
  instance = this;
}

//******************************************************************************
LIS2DH::LIS2DH(I2C *i2c, int slaveAddress) : 
        slaveAddress(slaveAddress) {
  this->i2c = i2c;
  i2c->frequency(400000);
  isOwner = false;
  instance = this;
}

//******************************************************************************
LIS2DH::~LIS2DH(void) {
  if (isOwner == true) {
    delete i2c;
  }
}

//******************************************************************************
int LIS2DH::writeReg(LIS2DH_REG_map_t reg, char value) {
  int result;
  char cmdData[2] = {(char)reg, value};
  result = i2c->write(slaveAddress, cmdData, 2);
  if (result != 0) {
    return -1;
  }
  return 0;
}

//******************************************************************************
int LIS2DH::readReg(LIS2DH_REG_map_t reg, char *value) {
  int result;
  char cmdData[1] = {(char)reg};

  result = i2c->write(slaveAddress, cmdData, 1);
  if (result != 0) {
    return -1;
  }
  result = i2c->read(slaveAddress, value, 1);
  if (result != 0) {
    return -1;
  }
  return 0;
}

//******************************************************************************
static void I2c_Reset(uint8_t index, int speed) {
  mxc_i2cm_regs_t *regs = MXC_I2CM_GET_I2CM(index);
  /* reset module */
  regs->ctrl = MXC_F_I2CM_CTRL_MSTR_RESET_EN;
  regs->ctrl = 0;
  /* enable tx_fifo and rx_fifo */
  regs->ctrl |= (MXC_F_I2CM_CTRL_TX_FIFO_EN | MXC_F_I2CM_CTRL_RX_FIFO_EN);
}

//******************************************************************************
///  Interrupt handler, this empties the hardware fifo and packetizes it for
///  streaming
void LIS2DH::int_handler(void) {
  char fifo_src;
  int16_t valueX;
  int16_t valueY;
  int16_t valueZ;
  int num;
  int index;

  I2c_Reset(2, 1);
  num = 0;
  index = 0;
  fifo_src = 0;
  while ((fifo_src & 0x20) != 0x20) {
    get_motion_fifo(&valueX, &valueY, &valueZ);
    lis2dh_buffer[index++] = valueX;
    lis2dh_buffer[index++] = valueY;
    lis2dh_buffer[index++] = valueZ;
    readReg(LIS2DH_FIFO_SRC_REG, &fifo_src);
    num++;
    if (num >= 32) {
      break;
    }
  }
  motion_cached[0] = valueX;
  motion_cached[1] = valueY;
  motion_cached[2] = valueZ;

  StreamPacketUint32(PACKET_LIS2DH, lis2dh_buffer, index);
}

//******************************************************************************
void LIS2DH::init(void) {
  stop();
  configure_interrupt();
}

//******************************************************************************
void LIS2DH::configure_interrupt(void) {
  
  lis2dh_ctrl_reg6_t ctrl_reg6;
  ///< interrupt enabled on INT1, interrupt active low
  ctrl_reg6.all = 0;
  ctrl_reg6.bit.I2_INT1 = 1;      ///< interrupt 1 function enabled on int1 pin
  ctrl_reg6.bit.H_LACTIVE = 1;    ///< interrupt active low
  writeReg(LIS2DH_CTRL_REG6, ctrl_reg6.all);
}

//******************************************************************************
int LIS2DH::initStart(int dataRate, int fifoThreshold) {
  
  lis2dh_ctrl_reg5_t     ctrl_reg5;
  lis2dh_fifo_ctrl_reg_t fifo_ctrl_reg;
  lis2dh_ctrl_reg1_t     ctrl_reg1;
  lis2dh_ctrl_reg3_t     ctrl_reg3;
  
  __disable_irq();

  configure_interrupt();
  ///
  /// enable FIFO
  ///
  ctrl_reg5.all = 0x0;
  ctrl_reg5.bit.FIFO_EN = 0x1;
  if (writeReg(LIS2DH_CTRL_REG5, ctrl_reg5.all) == -1) {
    __enable_irq();
    return -1;
  }
  
  ///
  /// set FIFO to stream mode, trigger select INT1
  ///
  fifo_ctrl_reg.all = 0x0;
  fifo_ctrl_reg.bit.FTH = fifoThreshold;
  fifo_ctrl_reg.bit.FM = LIS2DH_FIFOMODE_STREAM;
  fifo_ctrl_reg.bit.TR = 0x0;
  if (writeReg(LIS2DH_FIFO_CTRL_REG, fifo_ctrl_reg.all) == -1) {
    __enable_irq();
    return -1;
  }

  ///
  /// set HR (high resolution)
  ///
  if (writeReg(LIS2DH_CTRL_REG4, 0x8) == -1) {
    __enable_irq();
    return -1;
  }

  ///
  /// set the data rate, enable all axis
  ///
  dataRate = dataRate & 0xF;
  if (dataRate > 0x9) {
    dataRate = 0x9;
  }
  
  ctrl_reg1.bit.ODR = dataRate; ///< set the data rate
  ctrl_reg1.bit.LPen = 0x0;     ///< enable low power mode
  ctrl_reg1.bit.Zen = 0x1;      ///< enable z
  ctrl_reg1.bit.Yen = 0x1;      ///< enable y
  ctrl_reg1.bit.Xen = 0x1;      ///< enable x
  if (writeReg(LIS2DH_CTRL_REG1, ctrl_reg1.all) == -1) {
    __enable_irq();
    return -1;
  }

  ///
  /// enable watermark interrupt
  ///
  ctrl_reg3.all = 0x00;
  ctrl_reg3.bit.I1_WTM = 0x1;
  if (writeReg(LIS2DH_CTRL_REG3, ctrl_reg3.all) == -1) {
    __enable_irq();
    return -1;
  }
  __enable_irq();

  return 0;
}

//******************************************************************************
int LIS2DH::detect(char *detected) {
  char val;
  *detected = 0;
  if (readReg(LIS2DH_WHO_AM_I, &val) == -1) {
    return -1;
  }
  
  if (val == LIS2DH_ID) {
    *detected = 1;
  }
  
  return 0;
}

//******************************************************************************
int LIS2DH::get_motion_cached(int16_t *valueX, int16_t *valueY,
                              int16_t *valueZ) {
  *valueX = motion_cached[0];
  *valueY = motion_cached[1];
  *valueZ = motion_cached[2];
  return 0;
}

//******************************************************************************
int LIS2DH::get_motion_fifo(int16_t *valueX, int16_t *valueY, int16_t *valueZ) {
  char reg = LIS2DH_OUT_X_L | 0x80;
  char values[6];
  int i;

  reg = LIS2DH_OUT_X_L;
  for (i = 0; i < 6; i++) {
    if (readReg((LIS2DH_REG_map_t)reg, &values[i]) != 0) {
      return -1;
    }
    reg++;
  }

  *valueX = ((short)values[1] << 8) + values[0];
  *valueY = ((short)values[3] << 8) + values[2];
  *valueZ = ((short)values[5] << 8) + values[4];
  motion_cached[0] = *valueX;
  motion_cached[1] = *valueY;
  motion_cached[2] = *valueZ;
  return 0;
}

//******************************************************************************
char LIS2DH::readId(void) {
  char val;
  readReg(LIS2DH_WHO_AM_I, &val);
  return val;
}

//******************************************************************************
void LIS2DH::stop(void) {
  __disable_irq();
  writeReg(LIS2DH_CTRL_REG3, 0x00); // Disable watermark interrupt
  writeReg(LIS2DH_CTRL_REG1, 0x00); // Data rate = 0Hz
  writeReg(LIS2DH_FIFO_CTRL_REG,
           0x00);                   // set to bypass mode... clears FIFO_SRC_REG
  __enable_irq();
}

//******************************************************************************
void LIS2DHIntHandler(void) {
  char value;
  ///
  /// read the data rate axis enable register, if this is zero then just return,
  /// we are not ready for interrupts
  ///
  LIS2DH::instance->readReg(LIS2DH::LIS2DH_CTRL_REG1, &value);
  if (value == 0x0) {
    return;
  }
  LIS2DH::instance->int_handler();
}
