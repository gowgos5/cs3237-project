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
#ifndef _LED_H_
#define _LED_H_

#include "mbed.h"

/**
 * Driver for the HSP Led, supports different blink rates and patterns
 *
 */

class HspLed {
public:
  static const int LED_ON = 0;
  static const int LED_OFF = 1;

  /// define all of the modes the LED can support
  typedef enum eMode {
    eLedOn,
    eLedOff,
    eLedPeriod,
    eLedPattern
  } eMode_t;
  /// define the values that turn the LED on or off at the pin
  #define HSP_LED_ON 0
  #define HSP_LED_OFF 1

  /*
  * @brief Constructor where you specify the LED pin name
  */
  HspLed(PinName ledPin);
  
  /**
  * @brief Blink the HSP LED at a set time interval
  * @param mSeconds Number of seconds to set the timer interval
  */
  void blink(uint32_t mSeconds);
  
  /**
  * @brief Start rotating the LED through a 32-bit pattern at a mS rate specified
  * @param pattern 32-bit pattern to rotate through
  * @param mSeconds the amount of time to take per bit in the pattern
  */ 
  void pattern(uint32_t pattern, uint32_t mSeconds);
  
  /**
  * @brief Turn the LED on
  */
  void on(void);
  
  /**
  * @brief Turn the LED off
  */
  void off(void);

  /**
  * @brief Update the LED
  */ 
  void service(void);

private:

  /**
  * @brief Set the mode of the LED, the mode include blinking at a set rate or blinking
  * @brief according to a pattern
  * @param mode Mode to set the LED to
  */
  void setMode(eMode_t state);
  
  /**
  * @brief Toggle the state of the LED
  */
  void toggle(void);

  /**
  * @brief Start the LED blinking or rotating through a pattern
  */
  void start(void);

  /**
  * @brief Stop blinking or rotating through a pattern
  */
  void stop(void);
  
  /**
  * @brief Write the LED pin to a state
  * @param state A one or zero value to write to the LED pin
  */  
  void state(int state);

  /**
  * @brief Single step through the pattern and output to the LED
  */
  void patternToLed(void);

  /**
   * @brief timer interval in mS
   */
  float timerInterval;
  
  /**
   * @brief last timer interval set to... used to prevent resetting the timer to the same value
   */
  float timerIntervalLast;
 
  /** 
   * @brief local state of the pattern to rotate through
   */ 
  uint32_t bitPattern;
 
  /**
   * @brief current mode of the LED
   */
  eMode_t mode;
 
  /**
   * @brief the LED digital output
   */
  DigitalOut redLed;
 
  /**
   * @brief Timer service used to update the LED
   */
  Ticker ticker;
 
  /**
   * @brief Flag to indicate if the timer has been started
   */
  bool isStarted;
};

#endif /* _LED_H_ */
