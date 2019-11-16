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
#include "MAX30101.h"

MAX30101 *MAX30101::instance = NULL;

//******************************************************************************
MAX30101::MAX30101(PinName sda, PinName scl, int slaveAddress): 
          slaveAddress(slaveAddress) {
  i2c = new I2C(sda, scl);
  i2c_owner = true;
  i2c->frequency(400000);
  onInterruptCallback = NULL;
  onDataAvailableCallback = NULL;
  instance = this;
}

//******************************************************************************
MAX30101::MAX30101(I2C *_i2c, int slaveAddress) : 
          slaveAddress(slaveAddress) {
  i2c = _i2c;
  i2c_owner = false;
  i2c->frequency(400000);
  onInterruptCallback = NULL;
  onDataAvailableCallback = NULL;
  instance = this;
}

//******************************************************************************
MAX30101::~MAX30101(void) {
  if (i2c_owner) {
    delete i2c;
  }
}

//******************************************************************************
int MAX30101::int_handler(void) {
  uint16_t index, i;
  uint16_t rx_bytes, second_rx_bytes;
  char temp_int;
  char temp_frac;
  uint16_t num_active_led;
  uint32_t sample;
  int loop = 1;
  static uint8_t cntr_int = 0;

  max30101_Interrupt_Status_1_t   Interrupt_Status_1;
  max30101_Interrupt_Status_2_t   Interrupt_Status_2;
  max30101_mode_configuration_t   mode_configuration;
  max30101_multiLED_mode_ctrl_1_t multiLED_mode_ctrl_1;
  max30101_multiLED_mode_ctrl_2_t multiLED_mode_ctrl_2;
  max30101_spo2_configuration_t   spo2_configuration;
  max30101_fifo_configuration_t   fifo_configuration;

  cntr_int++;

  while (loop) {
    if (i2c_reg_read(REG_INT_STAT_1, &Interrupt_Status_1.all) != 0) { ///< Read Interrupt flag bits
      return -1;
    }

    if (i2c_reg_read(REG_INT_STAT_2, &Interrupt_Status_2.all) != 0) { ///< Read Interrupt flag bits
      return -1;
    }

    /* Read all the relevant register bits */
    if (i2c_reg_read(REG_MODE_CFG, &mode_configuration.all) != 0) {
      return -1;
    }


    if (i2c_reg_read(REG_SLT2_SLT1, &multiLED_mode_ctrl_1.all) != 0) {
      return -1;
    }
    

    if (i2c_reg_read(REG_SLT4_SLT3, &multiLED_mode_ctrl_2.all) != 0) {
      return -1;
    }    
    

    if (i2c_reg_read(REG_SPO2_CFG, &spo2_configuration.all) != 0) {
      return -1;
    }  
    

    if (i2c_reg_read(REG_FIFO_CFG, &fifo_configuration.all) != 0) {
      return -1;
    }     
    
    

    if (Interrupt_Status_1.bit.a_full) {
      ///< Read the sample(s)
      char reg = REG_FIFO_DATA;

      num_active_led = 0;

     if (mode_configuration.bit.mode == 0x02) {///< Heart Rate mode, i.e. 1 led    
        num_active_led = 1;
      } else if (mode_configuration.bit.mode == 0x03) { ///< SpO2 mode, i.e. 2 led
        num_active_led = 2;
      } else if (mode_configuration.bit.mode == 0x07) { ///< Multi-LED mode, i.e. 1-4 led
        if (multiLED_mode_ctrl_1.bit.slot1 != 0) {
          num_active_led++;
        }

        if (multiLED_mode_ctrl_1.bit.slot2 != 0) {
          num_active_led++;
        }

        if (multiLED_mode_ctrl_2.bit.slot3 != 0) {
          num_active_led++;
        }

        if (multiLED_mode_ctrl_2.bit.slot4 != 0) {
          num_active_led++;
        }
      }
               ///< 3bytes/LED x Number of Active LED x FIFO level selected
        rx_bytes =  3 * num_active_led * (32-fifo_configuration.bit.fifo_a_full);   

      second_rx_bytes = rx_bytes;
      
      /**
       * @brief: 
       * The FIFO Size is determined by the Sample size.  The number of bytes
       * in a Sample is dictated by number of LED's
       *
       *   #LED Selected     Bytes in "1" sample
       *        1                  3
       *        2                  6
       *        3                  9
       *        4                  12
       *
       *  The I2C API function limits the number of bytes to read, to 256 (i.e.
       *  char).  Therefore, when set for Multiple LED's and the FIFO
       *  size is set to 32.  It would mean there is more than 256 bytes.
       *  In that case two I2C reads have to be made.  However It is important
       *  to note that each "Sample" must be read completely and reading only
       *  partial number of bytes from a sample will result in erroneous data.
       * 
       *
       *  For example:
       *  Num of LED selected = 3 and FIFO size is set to 32 (i.e. 0 value in
       *  register), then the number of bytes will be
       *  3bytes/Led * 3led's * 32 = 288 bytes in all.  Since there are
       *  3 LED's each sample will contain (3 * 3) 9bytes.  
       *  Therefore Sample 1 = 9bytes, Sample 2 = 18,... Sample 28 = 252. 
       *  Therefore the first I2C read should be 252 bytes and the second
       *  read should be 288-252 = 36.
       *
       *  It turns out that this size issue comes up only when number of LED
       * selected is 3 or 4 and choosing 252bytes
       *  for the first I2C read would work for both Number of LED selection.
       */

      if (rx_bytes <= CHUNK_SIZE) {
        I2CM_Read(slaveAddress, &reg, 1, &max30101_rawData[0],
                  (char)rx_bytes /*total_databytes_1*/);
      } else {
        I2CM_Read(slaveAddress, &reg, 1, &max30101_rawData[0], CHUNK_SIZE);

        second_rx_bytes = second_rx_bytes - CHUNK_SIZE;
        I2CM_Read(slaveAddress, &reg, 1, &max30101_rawData[CHUNK_SIZE],
                  (char)second_rx_bytes);
      }

      index = 0;

      for (i = 0; i < rx_bytes; i += 3) {
        sample = ((uint32_t)(max30101_rawData[i] & 0x03) << 16) | (max30101_rawData[i + 1] << 8) | max30101_rawData[i + 2];

        ///< Right shift the data based on the LED_PW setting
        sample = sample >> (3 - spo2_configuration.bit.led_pw); // 0=shift 3, 1=shift 2, 2=shift 1, 3=no shift

        max30101_buffer[index++] = sample;
      }

      onDataAvailableCallback(MAX30101_OXIMETER_DATA + num_active_led, max30101_buffer, index);
    }


    ///< This interrupt handles the temperature interrupt
    if (Interrupt_Status_2.bit.die_temp_rdy) {
      char reg;

      reg = REG_TINT;
      if (I2CM_Read(slaveAddress, &reg, 1, &temp_int, 1) != 0) {
        return -1;
      }

      reg = REG_TFRAC;
      if (I2CM_Read(slaveAddress, &reg, 1, &temp_frac, 1) != 0) {
        return -1;
      }

      max30101_final_temp = (int8_t)temp_int + 0.0625f * temp_frac;

      if (i2c_reg_write(REG_TEMP_EN, 0x00) != 0) { ///< Die Temperature Config, Temp disable... after one read...
        return -1;
      }
    }

    if (i2c_reg_read(REG_INT_STAT_1, &Interrupt_Status_1.all) != 0) { ///< Read Interrupt flag bits

      return -1;
    }
    if (Interrupt_Status_1.bit.a_full != 1) {
      loop = 0;
    }
  }

  interruptPostCallback();


  return 0;
}

//******************************************************************************
int MAX30101::SpO2mode_init(uint8_t fifo_waterlevel_mark, uint8_t sample_avg,
                            uint8_t sample_rate, uint8_t pulse_width,
                            uint8_t red_led_current, uint8_t ir_led_current) {

  char status;

  max30101_mode_configuration_t  mode_configuration;
  max30101_fifo_configuration_t  fifo_configuration;
  max30101_spo2_configuration_t  spo2_configuration;
  max30101_Interrupt_Enable_1_t  Interrupt_Enable_1;

  mode_configuration.all = 0;
  mode_configuration.bit.reset = 1;
  if (i2c_reg_write(REG_MODE_CFG, mode_configuration.all) != 0) // Reset the device
  {
    return -1;
  }

  ///< Give it some settle time (100ms)
  wait(1.0 / 10.0); ///< Let things settle down a bit

  fifo_configuration.all = 0;
  fifo_configuration.bit.smp_ave = sample_avg; ///< Sample averaging;
  fifo_configuration.bit.fifo_roll_over_en = 1; ///< FIFO Roll over enabled
  fifo_configuration.bit.fifo_a_full = fifo_waterlevel_mark; ///< Interrupt when certain level is filled

  if (i2c_reg_write(REG_FIFO_CFG, fifo_configuration.all) != 0) {
    return -1;
  }

  spo2_configuration.bit.spo2_adc_rge = 0x2; ///< ADC Range 8192 fullscale
  spo2_configuration.bit.spo2_sr = sample_rate; ///< 100 Samp/sec.
  spo2_configuration.bit.led_pw = pulse_width; ///< Pulse Width=411us and ADC Resolution=18
  if (i2c_reg_write(REG_SPO2_CFG, spo2_configuration.all) != 0) {
    return -1;
  }

  if (i2c_reg_write(REG_LED1_PA, red_led_current) != 0) {
    return -1;
  }

  if (i2c_reg_write(REG_LED2_PA, ir_led_current) != 0) {
    return -1;
  }

  /************/

  if (i2c_reg_read(REG_INT_STAT_1, &status) != 0) ///<  Clear INT1 by reading the status
  {
    return -1;
  }

  if (i2c_reg_read(REG_INT_STAT_2, &status) != 0) ///<  Clear INT2 by reading the status
  {
    return -1;
  }

  if (i2c_reg_write(REG_FIFO_W_PTR, 0x00) != 0) ///<  Clear FIFO ptr
  {
    return -1;
  }

  if (i2c_reg_write(REG_FIFO_OVF_CNT, 0x00) != 0) ///<  Clear FIFO ptr
  {
    return -1;
  }

  if (i2c_reg_write(REG_FIFO_R_PTR, 0x00) != 0) ///<  Clear FIFO ptr
  {
    return -1;
  }

  Interrupt_Enable_1.all = 0;
  Interrupt_Enable_1.bit.a_full_en = 1; ///<  Enable FIFO almost full interrupt
  if (i2c_reg_write(REG_INT_EN_1, Interrupt_Enable_1.all) != 0) {
    return -1;
  }

  mode_configuration.all = 0;
  mode_configuration.bit.mode = 0x03; ///< SpO2 mode
  if (i2c_reg_write(REG_MODE_CFG, mode_configuration.all) != 0) {
    return -1;
  }

  return 0;
}

//******************************************************************************
int MAX30101::SpO2mode_stop(void) {

  max30101_Interrupt_Enable_1_t  Interrupt_Enable_1;
  max30101_mode_configuration_t  mode_configuration;
  uint8_t                      led1_pa;
  uint8_t                      led2_pa;

  Interrupt_Enable_1.all = 0;
  Interrupt_Enable_1.bit.a_full_en = 0; ///<  Disable FIFO almost full interrupt
  if (i2c_reg_write(REG_INT_EN_1, Interrupt_Enable_1.all) != 0) {
    return -1;
  }

  mode_configuration.all = 0;
  mode_configuration.bit.mode = 0x00; ///< SpO2 mode off
  if (i2c_reg_write(REG_MODE_CFG, mode_configuration.all) != 0) {
    return -1;
  }

  led1_pa = 0; ///< RED LED current, 0.0
  if (i2c_reg_write(REG_LED1_PA, led1_pa) != 0) {
    return -1;
  }

  led2_pa = 0; ///< IR LED current, 0.0
  if (i2c_reg_write(REG_LED2_PA, led2_pa) != 0) {
    return -1;
  }

  return 0;
}

//******************************************************************************
int MAX30101::HRmode_init(uint8_t fifo_waterlevel_mark, uint8_t sample_avg,
                          uint8_t sample_rate, uint8_t pulse_width,
                          uint8_t red_led_current) {

  /*uint8_t*/ char status;

  max30101_mode_configuration_t  mode_configuration;
  max30101_fifo_configuration_t  fifo_configuration;
  max30101_spo2_configuration_t  spo2_configuration;
  max30101_Interrupt_Enable_1_t  Interrupt_Enable_1;

  mode_configuration.all = 0;
  mode_configuration.bit.reset = 1;
  if (i2c_reg_write(REG_MODE_CFG, mode_configuration.all) != 0) ///< Reset the device, Mode = don't use...
  {
    return -1;
  }

  ///< Give it some settle time (100ms)
  wait(1.0 / 10.0); ///< Let things settle down a bit

  fifo_configuration.all = 0;
  fifo_configuration.bit.smp_ave = sample_avg;  ///< Sample averaging;
  fifo_configuration.bit.fifo_roll_over_en = 1; ///< FIFO Roll over enabled
  fifo_configuration.bit.fifo_a_full = fifo_waterlevel_mark; ///< Interrupt when certain level is filled
  if (i2c_reg_write(REG_FIFO_CFG, fifo_configuration.all) != 0) {
    return -1;
  }

  spo2_configuration.bit.spo2_adc_rge = 0x2;    ///< ADC Range 8192 fullscale
  spo2_configuration.bit.spo2_sr = sample_rate; ///< 100 Samp/sec.
  spo2_configuration.bit.led_pw = pulse_width;  ///< Pulse Width=411us and ADC Resolution=18
  if (i2c_reg_write(REG_SPO2_CFG, spo2_configuration.all) != 0) {
    return -1;
  }

  if (i2c_reg_write(REG_LED1_PA, red_led_current) != 0) {
    return -1;
  }

  /************/

  if (i2c_reg_read(REG_INT_STAT_1, &status) != 0) ///<  Clear INT1 by reading the status
  {
    return -1;
  }

  if (i2c_reg_read(REG_INT_STAT_2, &status) != 0) ///< Clear INT2 by reading the status
  {
    return -1;
  }

  if (i2c_reg_write(REG_FIFO_W_PTR, 0x00) != 0) ///<  Clear FIFO ptr
  {
    return -1;
  }

  if (i2c_reg_write(REG_FIFO_OVF_CNT, 0x00) != 0) ///<  Clear FIFO ptr
  {
    return -1;
  }

  if (i2c_reg_write(REG_FIFO_R_PTR, 0x00) != 0) ///<  Clear FIFO ptr
  {
    return -1;
  }

  Interrupt_Enable_1.all = 0;
  Interrupt_Enable_1.bit.a_full_en = 1;
  
  // Interrupt
  if (i2c_reg_write(REG_INT_EN_1, Interrupt_Enable_1.all) != 0) {
    return -1;
  }

  mode_configuration.all = 0;
  mode_configuration.bit.mode = 0x02; ///< HR mode
  if (i2c_reg_write(REG_MODE_CFG, mode_configuration.all) != 0) {
    return -1;
  }

  return 0;
}

//******************************************************************************
int MAX30101::HRmode_stop(void) {

  max30101_Interrupt_Enable_1_t  Interrupt_Enable_1;
  max30101_mode_configuration_t  mode_configuration;

  Interrupt_Enable_1.all = 0;
  Interrupt_Enable_1.bit.a_full_en = 0; ///< Disable FIFO almost full interrupt
  if (i2c_reg_write(REG_INT_EN_1, Interrupt_Enable_1.all) != 0) {
    return -1;
  }

  mode_configuration.all = 0;
  mode_configuration.bit.mode = 0x00; ///< HR mode off
  if (i2c_reg_write(REG_MODE_CFG, mode_configuration.all) != 0) {
    return -1;
  }

  if (i2c_reg_write(REG_LED1_PA, 0) != 0) {
    return -1;
  }

  return 0;
}

//******************************************************************************
int MAX30101::Multimode_init(uint8_t fifo_waterlevel_mark, uint8_t sample_avg,
                             uint8_t sample_rate, uint8_t pulse_width,
                             uint8_t red_led_current, uint8_t ir_led_current,
                             uint8_t green_led_current, uint8_t slot_1,
                             uint8_t slot_2, uint8_t slot_3, uint8_t slot_4) {

  char status;
  max30101_mode_configuration_t    mode_configuration;
  max30101_fifo_configuration_t    fifo_configuration;
  max30101_spo2_configuration_t    spo2_configuration;
  max30101_multiLED_mode_ctrl_1_t  multiLED_mode_ctrl_1;
  max30101_multiLED_mode_ctrl_2_t  multiLED_mode_ctrl_2;
  max30101_Interrupt_Enable_1_t    Interrupt_Enable_1;
  
  mode_configuration.all = 0;
  mode_configuration.bit.reset = 1;
  if (i2c_reg_write(REG_MODE_CFG, mode_configuration.all) != 0) ///< Reset the device, Mode = don't use...
  {
    return -1;
  }

  /* Give it some settle time (100ms) */ ///< Let things settle down a bit
  wait(1.0 / 10.0);

  fifo_configuration.all = 0;
  fifo_configuration.bit.smp_ave = sample_avg; ///< Sample averaging;
  fifo_configuration.bit.fifo_roll_over_en = 1; ///< FIFO Roll over enabled
  fifo_configuration.bit.fifo_a_full =
      fifo_waterlevel_mark; ///< Interrupt when certain level is filled
  if (i2c_reg_write(REG_FIFO_CFG, fifo_configuration.all) != 0) {
    return -1;
  }

  spo2_configuration.bit.spo2_adc_rge = 0x2;    ///< ADC Range 8192 fullscale
  spo2_configuration.bit.spo2_sr = sample_rate; ///< 100 Samp/sec.
  spo2_configuration.bit.led_pw = pulse_width;  ///< Pulse Width=411us and ADC Resolution=18
  if (i2c_reg_write(REG_SPO2_CFG, spo2_configuration.all) != 0) {
    return -1;
  }

  if (i2c_reg_write(REG_LED1_PA, red_led_current) != 0) {
    return -1;
  }

  if (i2c_reg_write(REG_LED2_PA, ir_led_current) != 0) {
    return -1;
  }

  if (i2c_reg_write(REG_LED3_PA, green_led_current) != 0) {
    return -1;
  }

  ///< 0x01=Red(LED1), 0x02=IR(LED2), 0x03=Green(LED3) : Use LEDn_PA to adjust the intensity
  ///< 0x05=Red      , 0x06=IR      , 0x07=Green       : Use PILOT_PA to adjust the intensity DO NOT USE THIS ROW...

  multiLED_mode_ctrl_1.bit.slot1 = slot_1;
  multiLED_mode_ctrl_1.bit.slot2 = slot_2;
  if (i2c_reg_write(REG_SLT2_SLT1, multiLED_mode_ctrl_1.all)) {
    return -1;
  }

  multiLED_mode_ctrl_2.all = 0;
  multiLED_mode_ctrl_2.bit.slot3 = slot_3;
  multiLED_mode_ctrl_2.bit.slot4 = slot_4;
  if (i2c_reg_write(REG_SLT4_SLT3, multiLED_mode_ctrl_2.all)) {
    return -1;
  }

  /************/

  if (i2c_reg_read(REG_INT_STAT_1, &status) != 0) ///<  Clear INT1 by reading the status
  {
    return -1;
  }

  if (i2c_reg_read(REG_INT_STAT_2, &status) != 0) ///<  Clear INT2 by reading the status
  {
    return -1;
  }

  if (i2c_reg_write(REG_FIFO_W_PTR, 0x00) != 0) ///<  Clear FIFO ptr
  {
    return -1;
  }

  if (i2c_reg_write(REG_FIFO_OVF_CNT, 0x00) != 0) ///<  Clear FIFO ptr
  {
    return -1;
  }

  if (i2c_reg_write(REG_FIFO_R_PTR, 0x00) != 0) ///<  Clear FIFO ptr
  {
    return -1;
  }

  Interrupt_Enable_1.all = 0;
  Interrupt_Enable_1.bit.a_full_en = 1; ///<  Enable FIFO almost full interrupt
  if (i2c_reg_write(REG_INT_EN_1, Interrupt_Enable_1.all) != 0) {
    return -1;
  }

  mode_configuration.all = 0;
  mode_configuration.bit.mode = 0x07; ///< Multi-LED mode
  if (i2c_reg_write(REG_MODE_CFG, mode_configuration.all) != 0) {
    return -1;
  }

  return 0;
}

//******************************************************************************
int MAX30101::Multimode_stop(void) {

max30101_Interrupt_Enable_1_t  Interrupt_Enable_1;
max30101_mode_configuration_t  mode_configuration;


  Interrupt_Enable_1.all = 0;
  Interrupt_Enable_1.bit.a_full_en = 0; ///< Disable FIFO almost full interrupt
  if (i2c_reg_write(REG_INT_EN_1, Interrupt_Enable_1.all) != 0) {
    return -1;
  }

  mode_configuration.all = 0;
  mode_configuration.bit.mode = 0x00; ///< Multi-LED mode off
  if (i2c_reg_write(REG_MODE_CFG, mode_configuration.all) != 0) {
    return -1;
  }

  if (i2c_reg_write(REG_LED1_PA, 0) != 0) {
    return -1;
  }

  if (i2c_reg_write(REG_LED2_PA, 0) != 0) {
    return -1;
  }

  if (i2c_reg_write(REG_LED3_PA, 0) != 0) {
    return -1;
  }
  return 0;
}

//******************************************************************************
int MAX30101::tempread(void) {
   
  if (i2c_reg_write(REG_INT_EN_2, 0x02) != 0) {///< Interrupt Enable 2, Temperature Interrupt
    return -1;
  }

  if (i2c_reg_write(REG_TEMP_EN, 0x01) != 0) {///< Die Temperature Config, Temp enable...

    return -1;
  }
  return 0;
}

//******************************************************************************
int MAX30101::i2c_reg_write(MAX30101_REG_map_t reg, char value) {

  char cmdData[2] = {reg, value};

  if (I2CM_Write(slaveAddress, NULL, 0, cmdData, 2) != 0) {
    return -1;
  }

  return 0;
}

//******************************************************************************
int MAX30101::i2c_reg_read(MAX30101_REG_map_t reg, char *value) {
  if (I2CM_Read(slaveAddress, (char *)&reg, 1, value, 1) != 0 /*1*/) {
    return -1;
  }

  return 0;
}

//******************************************************************************
int MAX30101::I2CM_Read(int slaveAddress, char *writeData, char writeCount,
                        char *readData, char readCount) {

  if (writeData != NULL && writeCount != 0) {
    i2c->write(slaveAddress, writeData, writeCount, true);
  }
  if (readData != NULL && readCount != 0) {
    i2c->read(slaveAddress, readData, readCount);
  }
  return 0;
}

//******************************************************************************
int MAX30101::I2CM_Write(int slaveAddress, char *writeData1, char writeCount1,
                         char *writeData2, char writeCount2) {
 
  if (writeData1 != NULL && writeCount1 != 0) {
    i2c->write(slaveAddress, writeData1, writeCount1);
  }
  if (writeData2 != NULL && writeCount2 != 0) {
    i2c->write(slaveAddress, writeData2, writeCount2);
  }
  return 0;
}

//******************************************************************************
void MAX30101::onDataAvailable(DataCallbackFunction _onDataAvailable) {

  onDataAvailableCallback = _onDataAvailable;
}

//******************************************************************************
void MAX30101::dataAvailable(uint32_t id, uint32_t *buffer, uint32_t length) {

  if (onDataAvailableCallback != NULL) {
    (*onDataAvailableCallback)(id, buffer, length);
  }
}

//******************************************************************************
void MAX30101::onInterrupt(InterruptFunction _onInterrupt) {

  onInterruptCallback = _onInterrupt;
}

//******************************************************************************
void MAX30101::interruptPostCallback(void) {

  if (onInterruptCallback != NULL) {

    (*onInterruptCallback)();
  }
}

//******************************************************************************
void MAX30101::MidIntHandler(void) { 

  MAX30101::instance->int_handler();
}
