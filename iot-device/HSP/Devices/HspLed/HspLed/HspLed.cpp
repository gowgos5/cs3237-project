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

#include "HspLed.h"
#include "Peripherals.h"

void hspled_int_handler(void);

//*******************************************************************************
HspLed::HspLed(PinName ledPin) : 
 timerInterval(500), timerIntervalLast(-1), redLed(LED_RED, 0), isStarted(false) {
}

//*******************************************************************************
void HspLed::state(int state) { 
  redLed.write(state); 
}

//*******************************************************************************
void HspLed::toggle(void) { 
  state(!redLed.read()); 
}

//*******************************************************************************
void HspLed::setMode(eMode mode) { 
  this->mode = mode; 
}

//*******************************************************************************
void HspLed::blink(uint32_t mSeconds) {
  mode = eLedPeriod;
  this->timerInterval = (float)mSeconds / 1000.0f;
  start();
}

//*******************************************************************************
void HspLed::pattern(uint32_t bitPattern, uint32_t mSeconds) {
  mode = eLedPattern;
  this->bitPattern = bitPattern;
  this->timerInterval = (float)mSeconds / 1000.0f;
  start();
}

//*******************************************************************************
void HspLed::on(void) {
  mode = eLedOn;
  state(HSP_LED_ON);
  start();
}

//*******************************************************************************
void HspLed::off(void) {
  mode = eLedOff;
  state(HSP_LED_OFF);
  start();
}

//*******************************************************************************
void HspLed::patternToLed(void) {
  uint32_t bit;
  bit = bitPattern & 1;
  state(bit);
  // rotate the pattern
  bitPattern = bitPattern >> 1;
  bitPattern = bitPattern | (bit << 31);
}

//*******************************************************************************
void HspLed::service(void) {
  switch (mode) {
  case eLedOn:
    state(HSP_LED_ON);
    break;
  case eLedOff:
    state(HSP_LED_OFF);
    break;
  case eLedPeriod:
    toggle();
    break;
  case eLedPattern:
    patternToLed();
    break;
  }
}

//*******************************************************************************
void HspLed::start(void) {
  if (timerInterval != timerIntervalLast && isStarted == true) {
    stop();
  }
  ticker.attach(&hspled_int_handler, timerInterval);
  timerIntervalLast = timerInterval;

  isStarted = true;
}

//*******************************************************************************
void HspLed::stop(void) {
  ticker.detach();
  isStarted = false;
}

//*******************************************************************************
void hspled_int_handler(void) {
  HspLed *hspLed = Peripherals::hspLed();
  hspLed->service();
}
