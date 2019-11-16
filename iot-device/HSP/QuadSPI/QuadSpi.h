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
#ifndef _QUADSPI_H_
#define _QUADSPI_H_

#include "mbed.h"




/**
* This class provides a Quad SPI interface for quad spi devices
* the class also allows single (4-Wire) communication
*/
class QuadSPI : SPI {

public:
  /** Create a QuadSPI master connected to the specified pins
   *
   *  mosi or miso can be specfied as NC if not used
   *
   *  @param mosi QuadSPI Master Out, Slave In pin
   *  @param miso QuadSPI Master In, Slave Out pin
   *  @param sclk QuadSPI Clock pin
   *  @param ssel QuadSPI chip select pin
   */
  QuadSPI(PinName mosi, PinName miso, PinName sclk, PinName ssel = NC);

  /** Write to the Quad SPI Slave and return the response
   *
   *  @param value Data to be sent to the SPI slave
   *
   *  @returns
   *    none
  */
  virtual int write(int value);

  /** Read from the Quad SPI Slave and return the response
   *
   *  @param none
   *
   *  @returns
   *    Response from the SPI slave
  */
  int read(void);

  /** Allow switching to and from Single SPI and Quad SPI
   *
   *  @param none
   *
   *  @returns
   *    Response from the SPI slave
  */
  void setSingleMode();
  void setQuadMode();
};

#endif /* _QUADSPI_H_ */
