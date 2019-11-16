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
#include "QuadSpi.h"
#include "spi_multi_api.h"

/** Initialize a SPI master for Quad SPI
 *
 * @param mosi   Pin used for Master Out Slave In
 * @param miso   Pin used for Master In Slave Out
 * @param sclk   Pin used for Clock
 * @param ssel   Pin used for Chip Select
 */
QuadSPI::QuadSPI(PinName mosi, PinName miso, PinName sclk, PinName ssel)
    : SPI(mosi, miso, sclk, ssel) {
}

/******************************************************************************/
void QuadSPI::setQuadMode() { spi_master_width(&_spi, WidthQuad); }
/******************************************************************************/
void QuadSPI::setSingleMode() { spi_master_width(&_spi, WidthSingle); }

/** Write a byte out in master mode and receive a value
 *
 * @param value   Byte Value to send
 * @return Returns Zero
 */
int QuadSPI::write(int value) {
  aquire();
  spi_master_write(&_spi, value);
  return 0;
}

/** Read a byte in master mode using Quad SPI simplex transfer
 *
 * @return Returns the value received from Quad SPI
 */
int QuadSPI::read(void) {
  aquire();
  return spi_master_read(&_spi);
}
