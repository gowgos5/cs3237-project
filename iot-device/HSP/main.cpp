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
#include "MAX14720.h"
#include "MAX30101.h"
#include "MAX30205.h"
#include "LIS2DH.h"
#include "USBSerial.h"
#include "RpcServer.h"
#include "StringInOut.h"
#include "Peripherals.h"
#include "BMP280.h"
#include "MAX30001.h"
#include "DataLoggingService.h"
#include "MAX30101_helper.h"
#include "S25FS512.h"
#include "QuadSpiInterface.h"
#include "PushButton.h"
#include "BLE.h"
#include "HspBLE.h"
#include "USBSerial.h"
#include "Streaming.h"

/// define the HVOUT Boost Voltage default for the MAX14720 PMIC
#define HVOUT_VOLTAGE 4500 // set to 4500 mV

/// define all I2C addresses
#define MAX30205_I2C_SLAVE_ADDR_TOP (0x92)
#define MAX30205_I2C_SLAVE_ADDR_BOTTOM (0x90)
#define MAX14720_I2C_SLAVE_ADDR (0x54)
#define BMP280_I2C_SLAVE_ADDR (0xEC)
#define MAX30101_I2C_SLAVE_ADDR (0xAE)
#define LIS2DH_I2C_SLAVE_ADDR (0x32)

///
/// wire Interfaces
///
/// Define with Maxim VID and a Maxim assigned PID, set to version 0x0001 and non-blocking
USBSerial usbSerial(0x0b6a, 0x0100, 0x0001, false);
/// I2C Master 1
I2C i2c1(I2C1_SDA, I2C1_SCL); // used by MAX30205 (1), MAX30205 (2), BMP280
/// I2C Master 2
I2C i2c2(I2C2_SDA, I2C2_SCL); // used by MAX14720, MAX30101, LIS2DH
/// SPI Master 0 with SPI0_SS for use with MAX30001
SPI spi(SPI0_MOSI, SPI0_MISO, SPI0_SCK, SPI0_SS); // used by MAX30001
/// SPI Master 1
QuadSpiInterface quadSpiInterface(SPI1_MOSI, SPI1_MISO, SPI1_SCK,
                                  SPI1_SS); // used by S25FS512

///
/// Devices
///
/// Pressure Sensor
BMP280 bmp280(&i2c1, BMP280_I2C_SLAVE_ADDR);
/// Top Local Temperature Sensor
MAX30205 MAX30205_top(&i2c1, MAX30205_I2C_SLAVE_ADDR_TOP);
/// Bottom Local Temperature Sensor
MAX30205 MAX30205_bottom(&i2c1, MAX30205_I2C_SLAVE_ADDR_BOTTOM);
/// Accelerometer
LIS2DH lis2dh(&i2c2, LIS2DH_I2C_SLAVE_ADDR);
InterruptIn lis2dh_Interrupt(P4_7);
/// PMIC
MAX14720 max14720(&i2c2, MAX14720_I2C_SLAVE_ADDR);
/// Optical Oximeter
MAX30101 max30101(&i2c2, MAX30101_I2C_SLAVE_ADDR);
InterruptIn max30101_Interrupt(P4_0);
/// External Flash
S25FS512 s25fs512(&quadSpiInterface);
/// ECG device
MAX30001 max30001(&spi);
InterruptIn max30001_InterruptB(P3_6);
InterruptIn max30001_Interrupt2B(P4_5);
/// PWM used as fclk for the MAX30001
PwmOut pwmout(P1_7);
/// HSP platform LED
HspLed hspLed(LED_RED);
/// Packet TimeStamp Timer, set for 1uS
Timer timestampTimer;
/// HSP Platform push button
PushButton pushButton(SW1);

/// BLE instance
static BLE ble;

/// HSP BluetoothLE specific functions
HspBLE hspBLE(&ble);
USBSerial *usbSerialPtr;

int main() {
  // hold results for returning funtcoins
  int result;

  // initialize HVOUT on the MAX14720 PMIC
  printf("Init MAX14720...\n");
  fflush(stdout);
  result = max14720.init();
  if (result == MAX14720_ERROR)
    printf("Error initializing MAX14720");
  max14720.boostEn = MAX14720::BOOST_ENABLED;
  max14720.boostSetVoltage(HVOUT_VOLTAGE);

  // turn on red led
  printf("Init HSPLED...\n");
  fflush(stdout);
  hspLed.on();

  // set NVIC priorities for GPIO to prevent priority inversion
  printf("Init NVIC Priorities...\n");
  fflush(stdout);
  NVIC_SetPriority(GPIO_P0_IRQn, 5);
  NVIC_SetPriority(GPIO_P1_IRQn, 5);
  NVIC_SetPriority(GPIO_P2_IRQn, 5);
  NVIC_SetPriority(GPIO_P3_IRQn, 5);
  NVIC_SetPriority(GPIO_P4_IRQn, 5);
  NVIC_SetPriority(GPIO_P5_IRQn, 5);
  NVIC_SetPriority(GPIO_P6_IRQn, 5);
  // used by the MAX30001
  NVIC_SetPriority(SPI1_IRQn, 0);

  // Be able to statically reference these devices anywhere in the application
  Peripherals::setS25FS512(&s25fs512);
  Peripherals::setMAX30205_top(&MAX30205_top);
  Peripherals::setMAX30205_bottom(&MAX30205_bottom);
  Peripherals::setBMP280(&bmp280);
  Peripherals::setLIS2DH(&lis2dh);
  //Peripherals::setUSBSerial(&usbSerial);
  Peripherals::setTimestampTimer(&timestampTimer);
  Peripherals::setHspLed(&hspLed);
  Peripherals::setMAX30101(&max30101);
  Peripherals::setI2c1(&i2c1);
  Peripherals::setI2c2(&i2c2);
  Peripherals::setPushButton(&pushButton);
  Peripherals::setBLE(&ble);
  Peripherals::setMAX14720(&max14720);
  Peripherals::setMAX30001(&max30001);
  Peripherals::setHspBLE(&hspBLE);
  usbSerialPtr = &usbSerial;
  
  // Initialize BLE base layer
  printf("Init HSPBLE...\n");
  fflush(stdout);
  hspBLE.init();

  // start blinking led1
  printf("Init HSPLED Blink...\n");
  fflush(stdout);
  hspLed.blink(1000);

  // Initialise MAX30101
  // fifo_a_full, sample_avg, sample_rate, pulse_width, red_led_current
  // ir_led_current, green_led_current, slot_1, slot_2, slot_3, slot_4
  // 17 samples, 16 sample avg, 400 Hz => 25 Hz
  max30101.Multimode_init(0x0F, 0x04, 0x03, 0x03, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00);

  // MAX30101 initialize interrupt
  printf("Init MAX30101 callbacks, interrupt...\n");
  fflush(stdout);
  max30101.onInterrupt(&MAX30101_OnInterrupt);
  max30101.onDataAvailable(&StreamPacketUint32);
  max30101_Interrupt.fall(&MAX30101::MidIntHandler);

  // initialize the LIS2DH accelerometer and interrupts
  printf("Init LIS2DH interrupt...\n");
  fflush(stdout);
  lis2dh.init();
  lis2dh.initStart(0x03, 0x00);     // data_rate = 25 Hz, fifo_threshold = 0
  lis2dh_Interrupt.fall(&LIS2DHIntHandler);
  lis2dh_Interrupt.mode(PullUp);
  
  RPC_init();
  
  while (1) {
      // wait for BLE events
      ble.waitForEvent();
  }
}
