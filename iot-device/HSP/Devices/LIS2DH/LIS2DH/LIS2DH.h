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

#ifndef LIS2DH_H_
#define LIS2DH_H_

#include "mbed.h"

/**
 * Driver for the LIS2DH on the HSP Platform
 *
 */

void LIS2DHIntHandler(void);


#define LIS2DH_DATARATE_1HZ    0x1
#define LIS2DH_DATARATE_10HZ   0x2
#define LIS2DH_DATARATE_25HZ   0x3
#define LIS2DH_DATARATE_50HZ   0x4
#define LIS2DH_DATARATE_100HZ  0x5
#define LIS2DH_DATARATE_200HZ  0x6
#define LIS2DH_DATARATE_400HZ  0x7

#define LIS2DH_FIFOMODE_BYPASS  0x00
#define LIS2DH_FIFOMODE_FIFO    0x01
#define LIS2DH_FIFOMODE_STREAM  0x02
#define LIS2DH_FIFOMODE_TRIGGER 0x03

#define LIS2DH_ID 0x33
#define LIS2DH_FIFO_SIZE 32

#define LISD2DH_FIFO_MODE_BYPASS  0
#define LISD2DH_FIFO_MODE_FIFO    1
#define LISD2DH_FIFO_MODE_STREAM  2
#define LISD2DH_FIFO_MODE_TRIGGER 3

#define LIS2DH_NUMBER_OF_AXIS  3
#define LIS2DH_MAX_DATA_SIZE   (LIS2DH_NUMBER_OF_AXIS * LIS2DH_FIFO_SIZE)

class LIS2DH {
public:

  typedef enum {  ///< LIS2DH registers
  
    LIS2DH_READID          = 0x33,  	
    LIS2DH_STATUS_REG_AUX  = 0x07,
    LIS2DH_OUT_TEMP_L      = 0x0C,
    LIS2DH_OUT_TEMP_H      = 0x0D,
    LIS2DH_INT_COUNTER_REG = 0x0E,
    LIS2DH_WHO_AM_I        = 0x0F,
    LIS2DH_TEMP_CFG_REG    = 0x1F,
    LIS2DH_CTRL_REG1       = 0x20,
    LIS2DH_CTRL_REG2       = 0x21,
    LIS2DH_CTRL_REG3       = 0x22,
    LIS2DH_CTRL_REG4       = 0x23,
    LIS2DH_CTRL_REG5       = 0x24,
    LIS2DH_CTRL_REG6       = 0x25,
    LIS2DH_REFERENCE       = 0x26,
    LIS2DH_STATUS_REG2     = 0x27,
    LIS2DH_OUT_X_L         = 0x28,
    LIS2DH_OUT_X_H         = 0x29,
    LIS2DH_OUT_Y_L         = 0x2A,
    LIS2DH_OUT_Y_H         = 0x2B,
    LIS2DH_OUT_Z_L         = 0x2C,
    LIS2DH_OUT_Z_H         = 0x2D,
    LIS2DH_FIFO_CTRL_REG   = 0x2E,
    LIS2DH_FIFO_SRC_REG    = 0x2F,
    LIS2DH_INT1_CFG        = 0x30,
    LIS2DH_INT1_SRC        = 0x31,
    LIS2DH_INT1_THS        = 0x32,
    LIS2DH_INT1_DURATION   = 0x33,
    LIS2DH_INT2_CFG        = 0x34,
    LIS2DH_INT2_SRC        = 0x35,
    LIS2DH_INT2_THS        = 0x36,
    LIS2DH_INT2_DURATION   = 0x37,
    LIS2DH_CLICK_CFG       = 0x38,
    LIS2DH_CLICK_SRC       = 0x39,
    LIS2DH_CLICK_THS       = 0x3A,
    LIS2DH_TIME_LIMIT      = 0x3B,
    LIS2DH_TIME_LATENCY    = 0x3C,
    LIS2DH_TIME_WINDOW     = 0x3D,
    LIS2DH_Act_THS         = 0x3E,
    LIS2DH_Act_DUR         = 0x3F,
  	
  	}LIS2DH_REG_map_t;


  ///< @brief STATUS_AUX (07h)
  typedef union lis2dh_status_aux_reg {
    char all;
    struct {
      char reserved3 : 2;
      char TDA       : 1;
      char reserved2 : 3;
      char TOR       : 1;
      char reserved1 : 1;
    } bit;
  } lis2dh_status_aux_t;

  ///< @brief TEMP_CFG_REG (1Fh)
  typedef union lis2dh_temp_cfg_reg {
    char all;
    struct {
      char reserved1 : 6;
      char TEMP_EN0  : 1;
      char TEMP_EN1  : 1;
    } bit;
  } lis2dh_temp_cfg_t;

  ///< @brief CTRL_REG1 (20h)
  typedef union lis2dh_ctrl_reg1_reg {
    char all;
    struct {
      char Xen  : 1;
      char Yen  : 1;
      char Zen  : 1;
      char LPen : 1;
      char ODR  : 4;
    } bit;
  } lis2dh_ctrl_reg1_t;

  ///< @brief CTRL_REG1 (21h)
  typedef union lis2dh_ctrl_reg2_reg {
    char all;
    struct {
      char HPIS    : 3;
      char HPCLICK : 1;
      char FDS     : 1;
      char HPCF    : 2;
      char HPM     : 2;
    } bit;
  } lis2dh_ctrl_reg2_t;

  ///< @brief CTRL_REG3 (22h)
  typedef union lis2dh_ctrl_reg3_reg {
    char all;
    struct {
      char reserved   : 1;
      char I1_OVERRUN : 1;
      char I1_WTM     : 1;
      char I1_DRDY    : 2;
      char I1_AOI     : 1;
      char I1_CLICK   : 1;
    } bit;
  } lis2dh_ctrl_reg3_t;

  ///< @brief CTRL_REG4 (23h)
  typedef union lis2dh_ctrl_reg4_reg {
    char all;
    struct {
      char SIM : 1;
      char ST  : 2;
      char HR  : 1;
      char FS  : 2;
      char BLE : 1;
      char BDU : 1;
    } bit;
  } lis2dh_ctrl_reg4_t;

  ///< @brief CTRL_REG5 (24h)
  typedef union lis2dh_ctrl_reg5_reg {
    char all;
    struct {
      char D4D_INT2 : 1;
      char LIR_INT2 : 1;
      char D4D_INT1 : 1;
      char LIR_INT1 : 1;
      char reserved : 2;
      char FIFO_EN  : 1;
      char BOOT     : 1;
    } bit;
  } lis2dh_ctrl_reg5_t;

  ///< @brief CTRL_REG6 (25h)
  typedef union lis2dh_ctrl_reg6_reg {
    char all;
    struct {
      char reserved1 : 1;
      char H_LACTIVE : 1;
      char reserved2 : 1;
      char P2_ACT    : 1;
      char BOOT_I2   : 1;
      char I2_INT2   : 2;
      char I2_INT1   : 1;
      char I2_CLICKen: 1;
    } bit;
  } lis2dh_ctrl_reg6_t;

  ///< @brief REFERENCE (26h)
  typedef union lis2dh_reference_reg {
    char all;
  } lis2dh_reference;

  ///< @brief STATUS_REG (27h)
  typedef union lis2dh_status_reg_ {
    char all;
    struct {
      char XDA   : 1;
      char YDA   : 1;
      char ZDA   : 1;
      char ZYXDA : 1;
      char XOR   : 1;
      char YOR   : 1;
      char ZOR   : 1;
      char ZYXOR : 1;
    } bit;
  } lis2dh_status_reg_t;


  ///< @brief FIFO_CTRL_REG (2Eh)
  typedef union lis2dh_fifo_ctrl_reg_ {
    char all;
    struct {
      char FTH : 5;
      char TR  : 1;
      char FM  : 2;
    } bit;
  } lis2dh_fifo_ctrl_reg_t;

  ///< @brief FIFO_CTRL_REG (2Fh)
  typedef union lis2dh_fifo_src_reg_ {
    char all;
    struct {
      char FSS       : 5;
      char EMPTY     : 1;
      char OVRN_FIFO : 1;
      char WTM       : 1;
    } bit;
  } lis2dh_fifo_src_reg_t;

  ///< @brief INT1_CFG (30h)
  typedef union lis2dh_int1_cfg_reg_ {
    char all;
    struct {
      char XLIE_XDOWNE : 1;
      char XHIE_XUPE   : 1;
      char YLIE_YDOWNE : 1;
      char YHIE_YUPE   : 1;
      char ZLIE_ZDOWNE : 1;
      char ZHIE_ZUPE   : 1;
      char SIXD        : 1;
      char AOI         : 1;
    } bit;
  } lis2dh_int1_cfg_t;

  ///< @brief INT1_SRC (31h)
  typedef union lis2dh_int1_src_reg_ {
    char all;
    struct {
      char XL : 1;
      char XH : 1;
      char YL : 1;
      char YH : 1;
      char ZL : 1;
      char ZH : 1;
      char IA : 1;
      char reserved : 1;
    } bit;
  } lis2dh_int1_src_t;


  ///< @brief INT2_CFG (34h)
  typedef union lis2dh_int2_cfg_reg_ {
    char all;
    struct {
      char XLIE : 1;
      char XHIE : 1;
      char YLIE : 1;
      char YHIE : 1;
      char ZLIE : 1;
      char ZHIE : 1;
      char SIX6 : 1;
      char AOI  : 1;
    } bit;
  } lis2dh_int2_cfg_t;

  ///< @brief INT2_SRC (35h)
  typedef union lis2dh_int2_src_reg_ {
    char all;
    struct {
      char XL       : 1;
      char XH       : 1;
      char YL       : 1;
      char YH       : 1;
      char ZL       : 1;
      char ZH       : 1;
      char IA       : 1;
      char reserved : 1;
    } bit;
  } lis2dh_int2_src_t;



  LIS2DH(PinName sda, PinName scl, int slaveAddress);
  LIS2DH(I2C *i2c, int slaveAddress);
  ~LIS2DH(void);

  /** 
   * @brief Initialize the device
   */
  void init(void);
  /** 
   * @brief Interrupt handler
   */
  void int_handler(void);
  /** 
   * @brief Get motion data from the device fifo
   */
  int get_motion_fifo(short *valueX, short *valueY, short *valueZ);
  /** 
   * @brief Get the last cached motion values (cached from a previous interrupt
   * event)
   */
  int get_motion_cached(int16_t *valueX, int16_t *valueY, int16_t *valueZ);
  /** 
   * @brief Star Interrupts
   */
  int initStart(int dataRate, int fifoThreshold);
  /** 
   * @brief Stop interrupts
   */
  void stop(void);
  /** 
   * @brief Read device register
   */
  int readReg(LIS2DH_REG_map_t reg, char *value);
  /** 
   * @brief Write device regsiter
   */
  int writeReg(LIS2DH_REG_map_t reg, char value);
  /** 
   * @brief Detect if device exists
   */
  int detect(char *detected);
  /** 
   * @brief Read the device ID
   */
  char readId(void);

  static LIS2DH *instance;

private:
  /** @brief
  */
  void configure_interrupt(void);
  ///< I2C pointer
  I2C *i2c;
  ///< Is this object the owner of the I2C object
  bool isOwner;
  ///< Device slave address
  int slaveAddress;
};

#endif /* LIS2DH_H_ */
