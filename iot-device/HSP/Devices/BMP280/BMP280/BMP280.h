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
*******************************************************************************/
/**
 * Bosch BMP280 Digital Pressure Sensor
 *
 */

#ifndef BMP280_H_
#define BMP280_H_

#include "mbed.h"

/**
 * @brief Bosch BMP280 Digital Pressure Sensor
 */
class BMP280 {

public:

  typedef enum {  ///< BMP280 Register addresses
	
    BMP280_READID     = 0x58,
    BMP280_TEMP_XLSB  = 0xFC,
    BMP280_TEMP_LSB   = 0xFB,
    BMP280_TEMP_MSB   = 0xFA,
    BMP280_PRESS_XLSB = 0xF9,
    BMP280_PRESS_LSB  = 0xF8,
    BMP280_PRESS_MSB  = 0xF7,
    BMP280_CONFIG     = 0xF5,
    BMP280_CTRL_MEAS  = 0xF4,
    BMP280_STATUS     = 0xF3,
    BMP280_RESET      = 0xE0,
    BMP280_ID         = 0xD0,
    ///< calib25-calib00: 0xA1-0x88
    BMP280_CALIB25    = 0xA1, ///< Beginning address
    BMP280_CALIB00    = 0x88, ///< Ending address

    BMP280_REGISTER_CHIPID       = 0xD0,
    BMP280_REGISTER_VERSION      = 0xD1,
    BMP280_REGISTER_SOFTRESET    = 0xE0,
    BMP280_REGISTER_CAL26        = 0xE1, // R calibration stored in 0xE1-0xF0
    BMP280_REGISTER_CONTROL      = 0xF4,
    BMP280_REGISTER_CONFIG       = 0xF5,
    BMP280_REGISTER_PRESSUREDATA = 0xF7,
    BMP280_REGISTER_TEMPDATA     = 0xFA,	
    	
  } BMP280_REG_map_t;
	

  /**
   * @brief BMP280 constructor.
   * @param sda mbed pin to use for SDA line of I2C interface.
   * @param scl mbed pin to use for SCL line of I2C interface.
   * @param slaveAddress Slave Address of the device.
   */
  BMP280(PinName sda, PinName scl, int slaveAddress);

  /**
   * @brief BMP280 constructor.
   * @param i2c I2C object to use.
   * @param slaveAddress Slave Address of the device.
   */
  BMP280(I2C *i2c, int slaveAddress);

  char loggingEnabled;
  char loggingSampleRate;

  /**
  * @brief Write a device register
  */
  int writeReg(char reg, char value);
  /**
  * @brief Read a device register
  */
  int readReg(char reg, char *value);
  
  ///< @brief CONFIG_REG (0xF5)
  typedef union bmp280_config_reg {
    char all;
    struct {
      char spi3w_en : 1;
      char reserved : 1;
      char filter   : 3;
      char t_sb     : 3;
    } bit;
  } bmp280_config_t;

  ///< @brief CTRL_MEAS (0xF4)
  typedef union bmp280_ctrl_meas {
    char all;
    struct {
      char mode   : 2;
      char osrs_p : 3;
      char osrs_t : 3;
    } bit;
  } bmp280_ctrl_meas_t;

  ///< @brief STATUS (0xF3)
  typedef union bmp280_status {
    char all;
    struct {
      char im_update : 1;
      char reserved1 : 2;
      char measuring : 1;
      char reserved2 : 4;
    } bit;
  } bmp280_status_t;


  ///< @brief  RESET (0xE0)
  char bmp280_reset;

  ///< @brief  ID (0xD0)
  char bmp280_id;

  typedef enum {
    SKIPPED_P = 0,
    OVERSAMPLING_X1_P  = 1,
    OVERSAMPLING_X2_P  = 2,
    OVERSAMPLING_X4_P  = 3,
    OVERSAMPLING_X8_P  = 4,
    OVERSAMPLING_X16_P = 5
  } bmp280_osrs_P_t;

  typedef enum {
    SKIPPED_T = 0,
    OVERSAMPLING_X1_T  = 1,
    OVERSAMPLING_X2_T  = 2,
    OVERSAMPLING_X4_T  = 3,
    OVERSAMPLING_X8_T  = 4,
    OVERSAMPLING_X16_T = 5
  } bmp280_osrs_T_t;

  typedef enum {
    FILT_OFF = 1,
    FILT_2   = 2,
    FILT_3   = 4,
    FILT_4   = 8,
    FILT_5   = 16
  } bmp280_FILT_t;

  typedef enum {
    SLEEP_MODE  = 0,
    FORCED_MODE = 1,
    NORMAL_MODE = 3
  } bmp280_MODE_t;

  typedef enum {
    T_0_5  = 0,
    T_62_5 = 1,
    T_125  = 2,
    T_250  = 3,
    T_500  = 4,
    T_1000 = 5,
    T_2000 = 6,
    T_4000 = 7
  } bmp280_TSB_t;

  ///< @brief calib25... calib00  (0xA1...0x88)
  char bmp280_Calib[26];

  uint16_t dig_T1; ///< Unique temp coeffs. read from the chip
  int16_t  dig_T2;
  int16_t  dig_T3;

  uint16_t dig_P1; ///< Unique Press. coeffs. read from the chip
  int16_t dig_P2;
  int16_t dig_P3;
  int16_t dig_P4;
  int16_t dig_P5;
  int16_t dig_P6;
  int16_t dig_P7;
  int16_t dig_P8;
  int16_t dig_P9;
                 
  int32_t t_fine; ///< This is calculated int the temperature to be used by the
                  ///< pressure

  int32_t bmp280_rawPress;
  int32_t bmp280_rawTemp;

  float Temp_degC;
  float Press_Pa; 

  /**
   * @brief BMP280 constructor.
   * @param sda mbed pin to use for SDA line of I2C interface.
   * @param scl mbed pin to use for SCL line of I2C interface.
   */
  BMP280(PinName sda, PinName scl);

  /**
   * @brief BMP280 constructor.
   * @param i2c I2C object to use.
   */
  BMP280(I2C *i2c);

  /**
   * @brief BMP280 destructor.
   */
  ~BMP280(void);

  // Function Prototypes

  /**
  * @brief This initializes the BMP280 
  * @brief The BMP280 has 2 modes. FORCED mode and  NORMAL mode.  FORCED Mode gives more
  * @brief control to the processor as the processor sends out the Mode to initiate a conversion
  * @brief and a data is sent out then. NORMAL mode is initialized once and it just runs and sends
  * @brief out data at a programmed timed interval.  (In this example the main() will set this to Normal
  * @brief function)
  * @param Osrs_p- Pressure oversampling
  * @param Osrs_t-  Temperature oversampling
  * @param Filter-  Filter Settings
  * @param Mode-    Power Modes
  * @param T_sb-    Standby time (used with Normal mode)
  * @param dig_T1, dig_T2, dig_T3-  Coeffs used for temp conversion - GLOBAL variables (output)
  * @param dig_P1, ....  , dig_P9-  Coeffs used for press conversion - GLOBAL variables (output)
  * @returns  0-if no error.  A non-zero value indicates an error.
  */
  int init(bmp280_osrs_P_t Osrs_p, bmp280_osrs_T_t Osrs_t, bmp280_FILT_t Filter,
           bmp280_MODE_t Mode, bmp280_TSB_t T_sb);

  /**
   * @brief The BMP280 has 2 modes. FORCED mode and  NORMAL mode.  FORCED Mode
   * gives more
   * @brief control to the processor as the processor sends out the Mode to
   * initiate a conversion
   * @brief and a data is sent out then. NORMAL mode is initialized once and it
   * just runs and sends
   * @brief out data at a programmed timed interval.  (In this example the
   * main() will set this to Normal
   * @brief function)
   * @param *Temp_degC - Pointer to temperature (result in deg C)
   * @param *Press_Pa - Pointer to pressure (resul in Pascal)
   * @returns 0-if no error.  A non-zero value indicates an error.
   */
  int ReadCompData(float *Temp_degC, float *Press_Pa);

  /**
   *  @brief This function allows writing to a register.
   *  @param reg- Address of the register to write to
   *  @param value- Data written to the register
   *  @returns 0-if no error.  A non-zero value indicates an error.
   */
  int reg_write(BMP280_REG_map_t reg, char value);

  /**
   * @brief This function allows writing to a register.
   * @params reg- Address of the register to read from (input)
   * @params *value- Pointer to the value read from the register (output)
   * @returns 0-if no error.  A non-zero value indicates an error.
   */
  int reg_read(BMP280_REG_map_t reg, char *value, char number);

  /**
   * @brief Performs a soft reset on the BMP280
   * @param none
   * @returns none
   */
  void Reset(void);

  /**
   * @brief Detects if the BMP280 is present
   * @param none
   * @returns 1 for found, 0 for not found, -1 for comm error
   */
  int Detect(void);

  /**
   * @brief Performs calculations on the raw temperature data to convert to
   * @brief temperature in deg C, based on Bosch's algorithm
   * @param Raw Temp ADC value, Global dig_T1, dig_T2, dig_T3
   * @returns The Temperature in deg C
   */
  float compensate_T_float(int32_t adc_T); // returned value Deg C.

  /**
    * @brief Performs calculations on the raw pressure data to convert to
    * @brief pressure in Pascal, based on Bosch's algorithm
    * @param adc_P Raw Press ADC value, Global dig_P1, dig_P2,..., dig_P9
    * @returns The Pressure in Pascals
    */
  float compensate_P_float(int32_t adc_P); //  returned value Pascal.

  /**
   * @brief Puts the BMP280 in low power Sleep mode
   * @param none
   * @returns 0 if no errors, -1 if error.
   */
  int Sleep(void);

  /**
   * @brief This reads the raw BMP280 data
   * @param *bmp280_rawData- array of raw output data
   * @returns 0-if no error.  A non-zero value indicates an error.
   *
   */
  int ReadCompDataRaw(char *bmp280_rawData);
  /**
   * @brief This reads the raw BMP280 data uses the Bosch algorithm to get the
   * data
   * @brief in float, then the float gets converted to an String
   * @param *bmp280_rawData- array of raw output data
   * @returns 0-if no error.  A non-zero value indicates an error.
   *
   */
  int ReadCompDataRaw2(char *bmp280_rawData);
  /**
   * @brief This converts the raw BMP280 data to couble based on Bosch's
   * algorithm
   * @param *bmp280_rawData- array of raw input data
   * @param *Temp_degC- pointer to output, Temp value in deg C
   * @param *Press_Pa- pointer to output, Press value in Pascals
   * @returns 0-if no error.  A non-zero value indicates an error.
   *
   */
  void ToFloat(char *bmp280_rawData, float *Temp_degC, float *Press_Pa);
  /**
   * @brief converts to Farenhite from Centigrade
   * @param temperature in Centigrade
   * @returns temperature value in Farenhite
   */
  float ToFahrenheit(float temperature);

  /**
   * @brief Reads a unique ID from the register
   * @param none
   * @returns The correct id value which is 0x58
   */
  int ReadId(void);

private:
  /**
   * @brief I2C pointer
   */
  I2C *i2c;
  /**
   * @brief Is this object the owner of the I2C object
   */
  bool isOwner;
  /**
   * @brief Device slave address
   */
  int slaveAddress;
};

#endif // BMP280_H_
