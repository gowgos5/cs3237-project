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
#ifndef S25FS512_H_
#define S25FS512_H_

#include "mbed.h"
#include "QuadSpiInterface.h"


#define ADDRESS_INC_4K      0x1000
#define ADDRESS_INC_32K     0x8000
#define ADDRESS_INC_64K     0x10000

#define ADDRESS_4K_START        0x0
#define ADDRESS_4K_END          0x8000  
  
#define ADDRESS_32K_START       0x8000
#define ADDRESS_32k_END         0x10000  
  
#define ADDRESS_64K_START       0x10000
#define ADDRESS_64k_END         0x2000000  

#define SIZE_OF_EXTERNAL_FLASH  0x2000000 // 33,554,432 Bytes
#define SIZE_OF_PAGE            0x100

#define IOMUX_IO_ENABLE              1

#define S25FS512_SPI_PORT            1
#define S25FS512_CS_PIN              0
#define S25FS512_CS_POLARITY         0
#define S25FS512_CS_ACTIVITY_DELAY   0
#define S25FS512_CS_INACTIVITY_DELAY 0
#define S25FS512_CLK_HI              4
#define S25FS512_CLK_LOW             4
#define S25FS512_ALT_CLK             0
#define S25FS512_CLK_POLARITY        0
#define S25FS512_CLK_PHASE           0
#define S25FS512_WRITE               1
#define S25FS512_READ                0

///< Byte addresses for flash configuration
#define Address_CR1NV   0x00000002
#define Address_CR1V    0x00800002
#define Address_CR2NV   0x00000003
#define Address_CR2V    0x00800003

#define INT_PORT_B 3
#define INT_PIN_B 6


class S25FS512 {
public:

  ///< @detail S25FS512 Commands
  typedef enum{ 
    Write_Reg              = 0x01,
    Read_Status_Reg1       = 0x05,
    Write_Enable           = 0x06,
    Erase_4k               = 0x20,
    Bulk_Erase             = 0x60,
    Read_Any_Reg           = 0x65,
    Software_Reset_Enable  = 0x66,
    Write_Any_Reg          = 0x71,
    Software_Reset         = 0x99,
    Read_Id                = 0x9F,
    Erase_64k_256k         = 0xD8,
    Quad_IO_Read           = 0xEB,
  } S25FS512_Commands_t;

  S25FS512(QuadSpiInterface *_quadSpiInterface);
  ~S25FS512(void);

  QuadSpiInterface *quadSpiInterface;

  /** @brief Initialize the driver
  */
  void init(void);

  /** @brief Detect the presence of the flash device
  */
  uint8_t detect(void);

  /** @brief Read the identification of the flash
  */
  int8_t readIdentification(uint8_t *dataArray, uint8_t length);

  /** @brief Bulk erase the flash device
  */
  int8_t bulkErase_Helper(void);

  /** @brief Erase Parameter Sectors
  */
  int8_t parameterSectorErase_Helper(uint32_t address);

  /** @brief Write a Page
  */
  int8_t writePage_Helper(uint32_t pageNumber, uint8_t *buffer, uint32_t offset);
  
  /** @brief Read a Page 
  */
  int8_t readPages_Helper(uint32_t startPageNumber,uint32_t endPageNumber, uint8_t *buffer, uint32_t offset);

  /** @brief Erase a Sector 
  @param address Address of sector to erase
  */
  int8_t sectorErase_Helper(uint32_t address);

  /** @brief Scans through byte pointer for a page worth of data to see if the page is all FFs
  @param ptr Byte pointer to buffer to scan
  @return Returns a 1 if the page is empty, 0 if it is not all FFs
  */
  bool isPageEmpty(uint8_t *ptr);

  /** @brief Issue a software reset to the flash device 
  */
  uint8_t reset(void);

  /** @brief Enable a hardware reset
  */
  uint8_t enableHWReset(void);
  
  /** @brief Read the id byte of this device 
  */
  void readID(uint8_t *id);

  void test_verifyPage0Empty(uint8_t *ptr, int currentPage, int pagesWrittenTo);

  int8_t readPartialPage_Helper(uint32_t pageNumber, uint8_t *buffer, uint32_t count);

private:
  void disableInterrupt(uint8_t state);
  int8_t reg_write_read_multiple_quad_last(uint8_t *dataIn, uint8_t numberIn, uint8_t *dataOut, uint8_t numberOut, uint8_t last);
  int8_t reg_write_read_multiple_quad(uint8_t *dataIn, uint8_t numberIn, uint8_t *dataOut, uint8_t numberOut);
  int8_t reg_write_read_multiple_4Wire(uint8_t *bufferOut, uint8_t numberOut, uint8_t *bufferIn, uint8_t numberIn);
  uint8_t spiWriteRead (uint8_t writeNumber,uint8_t *writeData, uint8_t readNumber, uint8_t *readData);
  uint8_t spiWriteRead4Wire(uint8_t writeNumber,uint8_t *writeData, uint8_t readNumber, uint8_t *readData);
  int8_t writeAnyRegister(uint32_t address, uint8_t data);
  int8_t writeAnyRegister4Wire(uint32_t address, uint8_t data);
  int8_t writeRegisters(void);
  uint8_t wren(void);
  int setQuadMode(void);
  int wren4Wire(void);
  // int8_t setQuadMode();
  int8_t readAnyRegister(uint32_t address, uint8_t *data, uint32_t length);
  int8_t bulkErase(void);
  int8_t pageProgram(uint32_t address, uint8_t *buffer);
  int8_t quadIoRead_Pages(uint32_t address, uint8_t *buffer, uint32_t numberOfPages);
  int8_t checkBusy(void);
  void waitTillNotBusy(void);
  int8_t sectorErase(uint32_t address);
  int8_t parameterSectorErase(uint32_t address);
  int8_t quadIoRead_PartialPage(uint32_t address, uint8_t *buffer, uint32_t numberOfBytesInPage);

  uint8_t flashBuffer[257 + 10];
};
#endif /* S25FS512_H_ */
