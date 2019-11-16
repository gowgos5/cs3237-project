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
#include "mbed.h"
#include "BMP280.h"

//******************************************************************************
BMP280::BMP280(PinName sda, PinName scl, int slaveAddress) : 
        slaveAddress(slaveAddress) {

  i2c = new I2C(sda, scl);
  isOwner = true;
}

//******************************************************************************
BMP280::BMP280(I2C *i2c, int slaveAddress) : 
        slaveAddress(slaveAddress) {

  this->i2c = i2c;
  isOwner = false;

  i2c->frequency(100000);
  loggingEnabled = 0;
  loggingSampleRate = 5;
}

//******************************************************************************
BMP280::~BMP280(void) {

  if (isOwner == true) {
    delete i2c;
  }
}

//******************************************************************************
int BMP280::init(BMP280::bmp280_osrs_P_t Osrs_p, BMP280::bmp280_osrs_T_t Osrs_t,
                 BMP280::bmp280_FILT_t Filter, BMP280::bmp280_MODE_t Mode,
                 BMP280::bmp280_TSB_t T_sb) {

  char raw_Tn[6];
  char raw_Pn[20];
  
  bmp280_ctrl_meas_t ctrl_meas;
  bmp280_config_t    config;

  ///
  /// Read all the temp coeffecients from the BMP280 memory. It will be used in
  /// calculation
  ///
  if (reg_read(BMP280_CALIB00, raw_Tn, 6) != 0) { 
    return -1;
  }

  dig_T1 = (((uint16_t)raw_Tn[1]) << 8) | raw_Tn[0];
  dig_T2 = (((int16_t)raw_Tn[3]) << 8) | raw_Tn[2];
  dig_T3 = (((int16_t)raw_Tn[5]) << 8) | raw_Tn[4];

  ///
  /// Read all the press coeffecients from the BMP280 memory. It will be used in
  /// calculation
  ///
  if (reg_read((BMP280_REG_map_t)(BMP280_CALIB00+6) /*reg*/, raw_Pn, 20) != 0) {
    return -1;
  }

  dig_P1 = (((uint16_t)raw_Pn[1]) << 8) | raw_Pn[0];
  dig_P2 = (((int16_t)raw_Pn[3]) << 8) | raw_Pn[2];
  dig_P3 = (((int16_t)raw_Pn[5]) << 8) | raw_Pn[4];
  dig_P4 = (((int16_t)raw_Pn[7]) << 8) | raw_Pn[6];
  dig_P5 = (((int16_t)raw_Pn[9]) << 8) | raw_Pn[8];
  dig_P6 = (((int16_t)raw_Pn[11]) << 8) | raw_Pn[10];
  dig_P7 = (((int16_t)raw_Pn[13]) << 8) | raw_Pn[12];
  dig_P8 = (((int16_t)raw_Pn[15]) << 8) | raw_Pn[14];
  dig_P9 = (((int16_t)raw_Pn[17]) << 8) | raw_Pn[16];


  wait(1.0 / 10.0);

  /****/
  if (reg_read(BMP280_CTRL_MEAS, &ctrl_meas.all, 1) != 0) {
    return -1;
  }

  ctrl_meas.bit.osrs_p = Osrs_p;
  ctrl_meas.bit.osrs_t = Osrs_t;

  ctrl_meas.bit.mode = Mode;

  if (reg_write(BMP280_CTRL_MEAS, ctrl_meas.all) != 0) {
    return -1;
  }

  /****/

  if (reg_read(BMP280_CONFIG, &config.all, 1) != 0) {
    return -1;
  }

  config.bit.filter = Filter;

  if (Mode == 0b11) {
    config.bit.t_sb = T_sb;
  }

  if (reg_write(BMP280_CONFIG, config.all) != 0) {
    return -1;
  }

  return 0;
}

//******************************************************************************
float BMP280::ToFahrenheit(float temperature) {

  return temperature * (9 / 5.0f) + 32;
}

//******************************************************************************
int BMP280::ReadCompDataRaw2(char *bmp280_rawData) {

  int i;
  char data[6];
  float temp;
  float pressure;
  int iPressure;
  char str[32];
  ReadCompDataRaw(data);
  ToFloat(data, &temp, &pressure);
  iPressure = (int)pressure;
  sprintf(str, "%d  ", iPressure);
  for (i = 0; i < 6; i++) {
    bmp280_rawData[i] = str[i];
  }
  return 0;
}

//******************************************************************************
int BMP280::ReadCompDataRaw(char *bmp280_rawData) {

  if (reg_read(BMP280_PRESS_MSB, bmp280_rawData, 6) != 0) {
    return -1;
  }
  return 0;
}

//******************************************************************************
void BMP280::ToFloat(char *bmp280_rawData, float *Temp_degC, float *Press_Pa) {

  bmp280_rawPress = (uint32_t)(bmp280_rawData[0] << 12) |
                    (bmp280_rawData[1] << 4) | (bmp280_rawData[2] >> 4);

  bmp280_rawTemp = (uint32_t)(bmp280_rawData[3] << 12) |
                   (bmp280_rawData[4] << 4) | (bmp280_rawData[5] >> 4);

  *Temp_degC = compensate_T_float(bmp280_rawTemp);
  *Press_Pa = compensate_P_float(bmp280_rawPress);
}

//******************************************************************************
int BMP280::ReadCompData(float *Temp_degC, float *Press_Pa) {

  char bmp280_rawData[6];

  if (ReadCompDataRaw(bmp280_rawData) != 0) {
    return -1;
  }
  ToFloat(bmp280_rawData, Temp_degC, Press_Pa);
  return 0;
}

//******************************************************************************
int BMP280::reg_write(BMP280_REG_map_t reg, char value) {

  int result;
  char cmdData[2] = {(char)reg, value};
  result = i2c->write(slaveAddress, cmdData, 2);
  if (result != 0) {
    return -1;
  }
  return 0;
}

//******************************************************************************
int BMP280::reg_read(BMP280_REG_map_t reg, char *value, char number) {

  int result;
  char cmdData[1] = {(char)reg};

  result = i2c->write(slaveAddress, cmdData, 1);
  if (result != 0) {
    return -1;
  }
  result = i2c->read(slaveAddress, value, number);
  if (result != 0) {
    return -1;
  }
  return 0;
}

//******************************************************************************
int BMP280::Sleep(void) {
  
  bmp280_ctrl_meas_t ctrl_meas;

  if (reg_read(BMP280_CTRL_MEAS, &ctrl_meas.all, 1) != 0) {
    return -1;
  }
  ctrl_meas.bit.mode = 0b00; // put everything to sleep mode...

  if (reg_write(BMP280_CTRL_MEAS, ctrl_meas.all) != 0) {
    return -1;
  }
  return 0;
}

//******************************************************************************
void BMP280::Reset(void) {

  reg_write(BMP280_RESET, 0xB6); // Initiate a Soft Reset
}

//******************************************************************************
int BMP280::Detect(void) {

  if (reg_read(BMP280_ID, &bmp280_id, 1) != 0) {
    return -1;
  }

  if (bmp280_id == 0x58) {
    return 1;
  }
  return 0;
}

//******************************************************************************
int BMP280::ReadId(void) {

  if (reg_read(BMP280_ID, &bmp280_id, 1) != 0) {
    return -1;
  }
  return bmp280_id;
}

//******************************************************************************
float BMP280::compensate_T_float(int32_t adc_T) {

  float var1, var2, T;
  var1 =
      (((float)adc_T) / 16384.0f - ((float)dig_T1) / 1024.0f) * ((float)dig_T2);

  var2 = ((((float)adc_T) / 131072.0f - ((float)dig_T1) / 8192.0f) *
          (((float)adc_T) / 131072.0f - ((float)dig_T1) / 8192.0f)) *
         ((float)dig_T3);

  t_fine = (int32_t)(var1 + var2);

  T = (var1 + var2) / 5120.0f;

  return T;
}

//******************************************************************************
float BMP280::compensate_P_float(int32_t adc_P) {

  float var1, var2, p;
  var1 = ((float)t_fine / 2.0f) - 64000.0f;
  var2 = var1 * var1 * ((float)dig_P6) / 32768.0f;
  var2 = var2 + var1 * ((float)dig_P5) * 2.0f;
  var2 = (var2 / 4.0f) + (((float)dig_P4) * 65536.0f);
  var1 = (((float)dig_P3) * var1 * var1 / 524288.0f + ((float)dig_P2) * var1) / 524288.0f;
  var1 = (1.0f + var1 / 32768.0f) * ((float)dig_P1);
  if (var1 == 0.0f) {
    return 0; // avoid exception caused by division by zero
  }
  p = 1048576.0f - (float)adc_P;
  p = (p - (var2 / 4096.0f)) * 6250.0f / var1;
  var1 = ((float)dig_P9) * p * p / 2147483648.0f;
  var2 = p * ((float)dig_P8) / 32768.0f;
  p = p + (var1 + var2 + ((float)dig_P7)) / 16.0f;
  return p;
}
