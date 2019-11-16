/*******************************************************************************
/ * Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
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
 * Maxim Integrated MAX30101 Oximeter chip
 *  
 * IMPORTANT: The code below will also need MAX14720.cpp and MAX14720.h
 * 
 * @code
 * #include "mbed.h"
 * #include "MAX14720.h"
 * #include "MAX30101.h"
 * 
 * 
 * /// define the HVOUT Boost Voltage default for the MAX14720 PMIC
 * #define HVOUT_VOLTAGE 4500 // set to 4500 mV
 * 
 * /// define all I2C addresses
 * #define MAX14720_I2C_SLAVE_ADDR (0x54)
 * #define MAX30101_I2C_SLAVE_ADDR (0xAE)
 * 
 * /// Settings for the HR initialization
 * #define FIFO_WATERLEVEL_MARK 15
 * #define SAMPLE_AVG           2
 * #define SAMPLE_RATE          1
 * #define PULSE_WIDTH          2
 * #define RED_LED_CURRENT      0x1F
 *  
 *  /// Buffer size for streaming data out.
 * #define BUFFER_LENGTH 50 
 * 
 * 
 * /// I2C Master 2
 * I2C i2c2(I2C2_SDA, I2C2_SCL); // used by MAX14720, MAX30101, LIS2DH
 * /// SPI Master 0 with SPI0_SS for use with MAX30001
 *  SPI spi(SPI0_MOSI, SPI0_MISO, SPI0_SCK, SPI0_SS); // used by MAX30001
 * 
 * /// PMIC
 * MAX14720 max14720(&i2c2, MAX14720_I2C_SLAVE_ADDR);
 * /// Optical Oximeter
 * MAX30101 max30101(&i2c2, MAX30101_I2C_SLAVE_ADDR);
 * InterruptIn max30101_Interrupt(P4_0);
 * 
 * //@brief Creating a buffer to hold the data
 * uint32_t oxiBuffer[BUFFER_LENGTH];
 * int oxiIndex = 0;
 * char data_trigger = 0;
 *  
 *
 * //@brief Creates a packet that will be streamed via USB Serial
 * //@brief the packet created will be inserted into a fifo to be streamed at a later time
 * //@param id Streaming ID
 * //@param buffer Pointer to a uint32 array that contains the data to include in the packet
 * //@param number Number of elements in the buffer
 * //
 * void StreamPacketUint32_ex(uint32_t id, uint32_t *buffer, uint32_t number) {
 *   int i;
 *   if (id == MAX30101_OXIMETER_DATA + 1) {
 * 
 *     for (i = 0; i < number; i++) {
 *       oxiBuffer[oxiIndex] = buffer[i];
 *       oxiIndex++;
 *           
 *       if (oxiIndex > BUFFER_LENGTH)
 *         {
 *         data_trigger = 1;
 *         oxiIndex = 0;
 *         }
 *     }
 *   }
 * }
 * 
 * int main() {
 *   // hold results for returning functions
 *   int result;
 * 
 *   // initialize HVOUT on the MAX14720 PMIC
 *   result = max14720.init();
 *   if (result == MAX14720_ERROR){
 *     printf("Error initializing MAX14720");
 *     }
 *   max14720.boostEn = MAX14720::BOOST_ENABLED;
 *   max14720.boostSetVoltage(HVOUT_VOLTAGE);
 * 
 *   // MAX30101 initialize interrupt
 *   max30101.onDataAvailable(&StreamPacketUint32_ex);
 *   max30101_Interrupt.fall(&MAX30101::MAX30101MidIntHandler);
 *   
 *   // This is the HR mode only (IR LED only)  
 *   max30101.HRmode_init(FIFO_WATERLEVEL_MARK, SAMPLE_AVG, SAMPLE_RATE,PULSE_WIDTH, RED_LED_CURRENT); 
 * 
 *   printf("Please wait for data to start streaming\n");
 *   fflush(stdout);
 *   
 *   while (1) {
 *     if(data_trigger == 1)
 *      {
 *       printf("%ld ", oxiBuffer[oxiIndex]);  // Print the ECG data on a serial port terminal software
 *       fflush(stdout);
 *      }
 *   } 
 * }
 * @endcode
 *
 */

#ifndef _MAX30101_H_
#define _MAX30101_H_

#include "mbed.h"

#define MAX30101_RAW_DATA_SIZE 3 * 4 * 32
#define MAX30101_PROC_DATA_SIZE 4 * 32

#define MAX30101_OXIMETER_DATA 0x10

#define CHUNK_SIZE 252

///< MAX30101 Register addresses

#define MAX30101_INT_PORT 4
#define MAX30101_INT_PIN 0
#define MAX30101_MASTER_NUM 2

/**
* Maxim Integrated MAX30101 Oximeter chip
*/
class MAX30101 {
public:
  float max30101_final_temp;                         ///< Global declaration
  uint32_t max30101_buffer[MAX30101_PROC_DATA_SIZE]; ///< final Processed data
  char max30101_rawData[MAX30101_RAW_DATA_SIZE];     ///< raw data from the chip

  typedef enum { ///< MAX30101 Register addresses

    ///< Status
    REG_INT_STAT_1 = 0x00,
    REG_INT_STAT_2 = 0x01,
    REG_INT_EN_1   = 0x02,
    REG_INT_EN_2   = 0x03,

    REG_FIFO_W_PTR   = 0x04,
    REG_FIFO_OVF_CNT = 0x05,
    REG_FIFO_R_PTR   = 0x06,
    REG_FIFO_DATA    = 0x07,
    ///< Configuration
    REG_FIFO_CFG  = 0x08,
    REG_MODE_CFG  = 0x09,
    REG_SPO2_CFG  = 0x0A,
    REG_LED1_PA   = 0x0C,
    REG_LED2_PA   = 0x0D,
    REG_LED3_PA   = 0x0E,
    REG_PILOT_PA  = 0x10,
    REG_SLT2_SLT1 = 0x11,
    REG_SLT4_SLT3 = 0x12,
    ///< Die Temp
    REG_TINT    = 0x1F,
    REG_TFRAC   = 0x20,
    REG_TEMP_EN = 0x21,
    ///< Proximity Func
    REG_PROX_INT_THR = 0x30,
    /* Part ID        */
    REG_REV_ID = 0xFE,
    REG_ID     = 0xFF,
  } MAX30101_REG_map_t;

  /**********/
  /* STATUS */
  /**********/
  /// @brief STATUS1 (0x00)
  typedef union max30101_Interrupt_Status_1_reg {
    char all;
    struct {
      char pwr_rdy  : 1;
      char reserved : 3;
      char prox_int : 1;
      char alc_ovf  : 1;
      char ppg_rdy  : 1;
      char a_full   : 1;
    } bit;
  } max30101_Interrupt_Status_1_t;

  /// @brief STATUS2 (0x01)
  typedef union max30101_Interrupt_Status_2_reg {
    char all;
    struct {
      char reserved1    : 1;
      char die_temp_rdy : 1;
      char reserved2    : 6;
    } bit;
  } max30101_Interrupt_Status_2_t;

  /// @brief INTERRUPT_ENABLE1 (0x02)
  typedef union max30101_Interrupt_Enable_1_reg {
    char all;
    struct {
      char reserved1   : 4;
      char prox_int_en : 1;
      char alc_ovf_en  : 1;
      char ppg_rdy_en  : 1;
      char a_full_en   : 1;
    } bit;
  } max30101_Interrupt_Enable_1_t;

  /// @brief INTERRUPT_ENABLE2 (0x03)
  typedef union max30101_Interrupt_Enable_2_reg {
    char all;
    struct {
      char reserved1       : 1;
      char die_temp_rdy_en : 1;
      char reserved2       : 6;
    } bit;
  } max30101_Interrupt_Enable_2_t;

  /*********/
  /* FIFO  */
  /*********/
  // 0x04
  /// @brief FIFO_WR_PTR (0x04)
  typedef union max30101_fifo_wr_ptr_reg {
    char all;
    struct {
      char fifo_wr_ptr : 5;
      char reserved1   : 3;
    } bit;
  } max30101_fifo_wr_ptr_t;

  /// @brief OVF_COUNTER (0x05)
  typedef union max30101_ovf_counter_reg {
    char all;
    struct {
      char fifo_ovf_counter : 5;
      char reserved1        : 3;
    } bit;
  } max30101_ovf_counter_reg_t;

  /// @brief FIFO_READ_PTR (0x06)
  typedef union max30101_fifo_rd_ptr_reg {
    char all;
    struct {
      char fifo_rd_ptr : 5;
      char reserved1   : 3;
    } bit;
  } max30101_fifo_rd_ptr_t;

  /********************/
  /* Configuration    */
  /********************/
  // 0x08
  /// @brief FIFO_CONFIGURATION (0x08)
  typedef union max30101_fifo_configuration_reg {
    char all;
    struct {
      char fifo_a_full       : 4;
      char fifo_roll_over_en : 1;
      char smp_ave           : 3;
    } bit;
  } max30101_fifo_configuration_t;

  /// @brief MODE_CONFIGURATION (0x09)
  typedef union max30101_mode_configuration_reg {
    char all;
    struct {
      char mode      : 3;
      char reserved1 : 3;
      char reset     : 1;
      char shdn      : 1;
    } bit;
  } max30101_mode_configuration_t;

  /// @brief SPO2_CONGIGURATION (0x0A)
  typedef union max30101_spo2_configuration_reg {
    char all;
    struct {
      char led_pw       : 2;
      char spo2_sr      : 3;
      char spo2_adc_rge : 2;
      char reserved1    : 1;
    } bit;
  } max30101_spo2_configuration_t;

  typedef union max30101_multiLED_mode_ctrl_1_reg {
    char all;
    struct {
      char slot1     : 3;
      char reserved  : 1;
      char slot2     : 3;
      char reserved1 : 1;
    } bit;
  } max30101_multiLED_mode_ctrl_1_t;

  typedef union max30101_multiLED_mode_ctrl_2_reg {
    char all;
    struct {
      char slot3     : 3;
      char reserved  : 1;
      char slot4     : 3;
      char reserved1 : 1;
    } bit;
  } max30101_multiLED_mode_ctrl_2_t;

  /********************/
  /* Die Temperature  */
  /********************/

  char max30101_tinit;

  char max30101_tfrac;

  typedef union max30101_die_temp_config {
    char all;
    struct {
      char temp_en  : 1;
      char reserved : 7;
    } bit;
  } max30101_die_temp_config_t;

  /***** Function Prototypes *****/

  char max30101_prox_int_thresh;

  /**
  * @brief MAX30101 constructor.
  * @param sda mbed pin to use for SDA line of I2C interface.
  * @param scl mbed pin to use for SCL line of I2C interface.
  */
  MAX30101(PinName sda, PinName scl, int slaveAddress);

  /**
  * @brief MAX30101 constructor.
  * @param i2c I2C object to use.
  */
  MAX30101(I2C *i2c, int slaveAddress);

  /**
  * @brief MAX30101 destructor.
  */
  ~MAX30101(void);

  /**
   * @brief Allows reading from MAX30101 register
   * @param reg: is the register address, to read from (look at max30101.h and the
   *             data sheet for details)
   * @param value: is the pointer to the value read from the register
   * @returns  0-if no error.  A non-zero value indicates an error.
   */
  int i2c_reg_read(MAX30101_REG_map_t reg, char *value);

  /**
   * @brief Allows writing to MAX30101 register
   * @param reg: is the register address, to read from (look at max30101.h and
   * the data sheet for details)
   * @param value: is the value to write to the register
   * @returns  0-if if no error.  A non-zero value indicates an error.
   */
  int i2c_reg_write(MAX30101_REG_map_t reg, char value);

  /**
   * @brief This function sets up for the SpO2 mode.  The data is returned in
   * the callback function
   * @brief max30101_int_handler in global array: buffer[].  SP mode handles two LED (Red,IR) data.  Hence it
   * @brief can fill up the FIFO up to a maximum of 3bytes/sample x 32 x 2 = 192bytes.
   * @param fifo_waterlevel_mark: corresponds to FIFO_A_FULL, In FIFO Configuration Register (0x08)
   * @param sample_avg: corresponds to SMP_AVE, in FIFO Configuration Register (0x08)
   * @param sample_rate: corresponds to SPO2_SR, IN SpO2 Configuration Register (0x0A)
   * @param pulse_width: corresponds to LED_PW in SpO2 Configuration register(0x0A)
   * @param red_led_current: corresponds to LED1_PA register (0x0C).  Please see data sheet for values
   * @param ir_led_current: corresponds to LED2_PA register (0x0D).  Please see data sheet for values
   * @returns 0-if everything is good.  A non-zero value indicates an error.
   */
  int SpO2mode_init(uint8_t fifo_waterlevel_mark, uint8_t sample_avg,
                    uint8_t sample_rate, uint8_t pulse_width,
                    uint8_t red_led_current, uint8_t ir_led_current);

  /**
   * @brief This function will stop the SpO2 mode and turn off all operating LED�s.
   * @return  0-if if no error.  A non-zero value indicates an error.
   */
  int SpO2mode_stop(void);

  /**
   * @brief This function sets up for the HR mode.  The data is returned in thecallback function
   * @brief max30101_int_handler in global array: buffer[].HR mode handles one LED (Red) data.  Hence it can fill
   * @brief up the FIFO up to a maximum of 3bytes/sample x 32 = 96bytes.
   * @brief fifo_waterlevel_mark: corresponds to FIFO_A_FULL, In FIFO Configuration Register (0x08)
   * @param sample_avg: corresponds to SMP_AVE, in FIFO Configuration Register (0x08)
   * @param sample_rate:corresponds to SPO2_SR, IN SpO2 Configuration Register (0x0A)
   * @param pulse_width: corresponds to LED_PW in SpO2 Configuration Register(0x0A)
   * @param red_led_current: corresponds to LED1_PA register (0x0C).  Please see data sheet for values
   * @returns  0-if if no error.  A non-zero value indicates an error.
   */
  int HRmode_init(uint8_t fifo_waterlevel_mark, uint8_t sample_avg,
                  uint8_t sample_rate, uint8_t pulse_width,
                  uint8_t red_led_current);

  /**
   * @brief This function will stop the HR mode and turn off all operating
   * LED’s.
   * @return  0-if if no error.  A non-zero value indicates an error.
   */
  int HRmode_stop(void);

  /**
   *@brief This function sets up for the Multi-mode.  The data is returned in the callback function max30101_int_handler in global array:
   *@brief buffer[].  Multi-LED mode can handle 1 to 4 LED combinations.  Hence it can fill up the FIFO up to a maximum of
   *@brief 3bytes/sample x 32 x 4 = 384bytes.
   *@param fifo_waterlevel_mark: corresponds to FIFO_A_FULL, In FIFO Configuration Register (0x08)
   *@param sample_avg: corresponds to SMP_AVE, in FIFO Configuration Register (0x08)
   *@param sample_rate:corresponds to SPO2_SR, IN SpO2 Configuration Register (0x0A)
   *@param pulse_width: corresponds to LED_PW in SpO2 Configuration register(0x0A)
   *@param red_led_current: corresponds to LED1_PA register (0x0C).  Please see data sheet for values
   *@param ir_led_current: corresponds to LED2_PA register (0x0D).  Please see data sheet for values
   *@param green_led_current: corresponds to LED3_PA register (0x0E).  Please see data sheet for values
   *@param slot_1,…,slot_4: corresponds to Multi-LED Mode control Registers (0x11-0x12).
   *@returns  0-if if no error.  A non-zero value indicates an error.
   */
  int Multimode_init(uint8_t fifo_waterlevel_mark, uint8_t sample_avg,
                     uint8_t sample_rate, uint8_t pulse_width,
                     uint8_t red_led_current, uint8_t ir_led_current,
                     uint8_t green_led_current, uint8_t slot_1, uint8_t slot_2,
                     uint8_t slot_3, uint8_t slot_4);

  /**
   * @brief This function will stop the Multi-mode and turn off all operating LED’s.
   * @returns  0-if if no error.  A non-zero value indicates an error.
   */
  int Multimode_stop(void);

  /**
   * @brief This is a function that sets up for temperature read and should be called after one of the mode
   * @brief has been setup.  The data is returned in the callback function max30101_int_handler.  This
   * @brief function needs to be called every time temperature reading is required.
   * @brief Call the temp function after one of the MODES have been started
   * @brief Note that the temp is disabled after one read... also, it is not necessary
   * @brief to read the temp frequently...
   * @returns  0-if if no error.  A non-zero value indicates an error.
   */
  int tempread(void);

  /**
   *@brief This is a callback function which collects the data from the FIFO of the MAX30101 in a 32-bit
   *@brief unsigned global array called max30101_buffer[].  Upon every interrupt from the MAX30101, this
   *@brief function is called to service the FIFO of the MAX30101.  This callback function also services the
   *@brief interrupt for the temp data.  The temp data is collected in a floating point global variable
   *@brief final_temp.
   *@param max30101_buffer[], global uint32_t
   *@returns  0-if everything is good.  A non-zero value indicates an error.
   */
  int int_handler(void);
  
   /**
  * @brief encapsulates the int_handler above
  */
  static void MidIntHandler(void);
  
  /**
  * @brief type definition for data interrupt
  */
  typedef void (*DataCallbackFunction)(uint32_t id, uint32_t *buffer,
                                       uint32_t length);
  /**
  * @brief type definition for general interrupt
  */
  typedef void (*InterruptFunction)();

  /**
  * @brief Used to connect a callback for when interrupt data is available
  */
  void onInterrupt(InterruptFunction _onInterrupt);

  /**
  * @brief Used to connect a callback for when interrupt data is available
  */
  void onDataAvailable(DataCallbackFunction _onDataAvailable);

  static MAX30101 *instance;

private:
  /**
   * @brief Used to notify an external function that interrupt data is available
   * @param id type of data available
   * @param buffer 32-bit buffer that points to the data
   * @param length length of 32-bit elements available
   */
  void dataAvailable(uint32_t id, uint32_t *buffer, uint32_t length);


  /**
   * @brief Executed on interrupt (callback function at the end of the interrupt)
   * @param id type of data available
   * @param buffer 32-bit buffer that points to the data
   * @param length length of 32-bit elements available
   */
  void interruptPostCallback(void);

  /// callback function when interrupt data is available
  DataCallbackFunction onDataAvailableCallback;

  /// callback function when interrupt data is available
  InterruptFunction onInterruptCallback;

  /**
   * @brief Read from an I2C device (Read I2c wrapper method)
   * @param slaveAddress slave address to use with transaction
   * @param writeData pointer of data to write
   * @param writeCount number of data to write
   * @param readData pointer to buffer to read to
   * @param readCount number of bytes to read
   */

  int I2CM_Read(int slaveAddress, char *writeData, char writeCount, char *readData, char readCount);
  
  /**
   * @brief Write to an I2C device (I2C wrapper method)
   * @param slaveAddress slave address to use with transaction
   * @param writeData1 pointer of data to write
   * @param writeCount1 number of data to write
   * @param writeData2 pointer to buffer to read to
   * @param writeCount2 number of bytes to read
   */
  int I2CM_Write(int slaveAddress, char *writeData1, char writeCount1, char *writeData2, char writeCount2);

  /// @brief pointer to I2C object
  I2C *i2c;

  /// @brief flag to track if this object is the owner (created) the I2C object
  bool i2c_owner;

  /// @brief Device slave address
  int slaveAddress;
};

  /**
   *  @brief Resets the I2C block, when needed
   */
extern void I2CM_Init_Reset(uint8_t index, int speed);



#endif /* _MAX30101_H_ */
