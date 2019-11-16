
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
#include "S25FS512.h"
#include "QuadSpiInterface.h"
//#include "DebugRoutines.h"

//******************************************************************************
S25FS512::S25FS512(QuadSpiInterface *_quadSpiInterface) {
	this->quadSpiInterface = _quadSpiInterface;
}

//******************************************************************************
S25FS512::~S25FS512()
{
}

//******************************************************************************
void S25FS512::init(void) {
  setQuadMode();
}

//******************************************************************************
int S25FS512::wren4Wire(void) {
  uint8_t cmdArray[8];
  // Send WREN
  cmdArray[0] = 0x06;
  //wait_ms(1);
  return reg_write_read_multiple_4Wire(cmdArray, 1, flashBuffer, 1);
}

//******************************************************************************
uint8_t S25FS512::wren(void) {
  uint8_t cmdArray[8];
  // Send WREN
  cmdArray[0] = 0x06;
  //wait_ms(1);
  return reg_write_read_multiple_quad(cmdArray, 1, flashBuffer, 0);
}

//******************************************************************************
int8_t S25FS512::reg_write_read_multiple_quad_last(uint8_t *bufferOut, uint8_t numberOut, uint8_t *bufferIn, uint8_t numberIn, uint8_t last) {
	int32_t  success = 0;
	//if (spiSlave.port != 1) {
	//	while (1);
	//}
//	success = SPI_Transmit(&spiSlave, dataIn, numberIn, dataOut, numberOut, MXC_E_SPI_UNIT_BYTES, MXC_E_SPI_MODE_QUAD, 0, last);//MXC_E_SPI_MODE_QUAD

	success = quadSpiInterface->SPI_Transmit(
			bufferOut, numberOut,
			bufferIn, numberIn,
			(int)last);

	if (success != 0) return -1;
	return 0;
}

//******************************************************************************
int8_t S25FS512::reg_write_read_multiple_4Wire(uint8_t *bufferOut,
                                               uint8_t numberOut,
                                               uint8_t *bufferIn,
                                               uint8_t numberIn) {
  int32_t success = 0;
  success = quadSpiInterface->SPI_Transmit4Wire(bufferOut, numberOut, bufferIn,
                                                numberIn, (int)1);

  if (success != 0) return -1;
  return 0;
}

//******************************************************************************
int8_t S25FS512::reg_write_read_multiple_quad(uint8_t *bufferOut, uint8_t numberOut, uint8_t *bufferIn, uint8_t numberIn) {
	int8_t ret;
	ret = reg_write_read_multiple_quad_last(bufferOut,numberOut,bufferIn,numberIn,1);
	return ret;
}

//******************************************************************************
void S25FS512::readID(uint8_t *id) {
    uint8_t cmd = 0x9F;
    reg_write_read_multiple_quad(&cmd, 1, id, 4);
}

//******************************************************************************
int8_t S25FS512::writeAnyRegister(uint32_t address, uint8_t data) {
	uint8_t cmdArray[5];
	cmdArray[0] = 0x71;
	cmdArray[1] = (address >> 16) & 0xFF;
	cmdArray[2] = (address >> 8) & 0xFF;
	cmdArray[3] = (address >> 0) & 0xFF;
	cmdArray[4] = data;
	return reg_write_read_multiple_quad(cmdArray,5,flashBuffer,0);
}

//******************************************************************************
int8_t S25FS512::writeAnyRegister4Wire(uint32_t address, uint8_t data) {
  uint8_t cmdArray[5];
  cmdArray[0] = 0x71;
  cmdArray[1] = (address >> 16) & 0xFF;
  cmdArray[2] = (address >> 8) & 0xFF;
  cmdArray[3] = (address >> 0) & 0xFF;
  cmdArray[4] = data;
  return reg_write_read_multiple_4Wire(cmdArray, 5, flashBuffer, 5);
}

//******************************************************************************
int8_t 	S25FS512::writeRegisters(void) {
	uint8_t cmdArray[3];
	wait_ms(1);
	cmdArray[0] = 0x01;
	cmdArray[1] = 0x00;
	cmdArray[2] = 0x02; // set Quad to 1
	reg_write_read_multiple_quad(cmdArray,3,flashBuffer,0);
	return 0;
}

//******************************************************************************
int8_t S25FS512::readAnyRegister(uint32_t address, uint8_t *data, uint32_t length) {
	uint8_t cmdArray[4];
	cmdArray[0] = 0x65;
	cmdArray[1] = (address >> 16) & 0xFF;
	cmdArray[2] = (address >> 8) & 0xFF;
	cmdArray[3] = (address >> 0) & 0xFF;
	return reg_write_read_multiple_quad(cmdArray,4,data,length);
}

//******************************************************************************
int8_t S25FS512::bulkErase(void) {
	uint8_t cmdArray[1];
	cmdArray[0] = 0x60;
	return reg_write_read_multiple_quad(cmdArray,1,flashBuffer,0);
}

//******************************************************************************
int8_t S25FS512::pageProgram(uint32_t address, uint8_t *buffer) {
	uint32_t i;
	uint8_t cmdArray[6 + 256];
	uint8_t *ptr;

	//for (i = 0; i < 256; i++) {
	//	dataArray[i] = i;
	//}
	cmdArray[0] = 0x12; //0x02; //0x71;
	cmdArray[1] = (address >> 24) & 0xFF;
	cmdArray[2] = (address >> 16) & 0xFF;
	cmdArray[3] = (address >> 8) & 0xFF;
	cmdArray[4] = (address >> 0) & 0xFF;
	for (i = 0; i < 256; i++) {
		cmdArray[5 + i] = buffer[i];
	}
//	reg_write_read_multiple_quad(cmdArray,256 + 4,flashBuffer,256 + 4);

	ptr = cmdArray;
	reg_write_read_multiple_quad_last(ptr,5 + 64,flashBuffer,0,0);
	//wait_ms(1);
	ptr += (5 + 64);
	reg_write_read_multiple_quad_last(ptr,64,flashBuffer,0,0);
	//wait_ms(1);
	ptr += 64;
	reg_write_read_multiple_quad_last(ptr,64,flashBuffer,0,0);
	//wait_ms(1);
	ptr += 64;
	reg_write_read_multiple_quad_last(ptr,64,flashBuffer,0,1);
	//wait_ms(1);
	return 0;
}

//
// read page 0 and assure that it is empty
//
void S25FS512::test_verifyPage0Empty(uint8_t *ptr, int currentPage, int pagesWrittenTo) {
    uint8_t data[512];
    bool pageEmpty;   
    readPages_Helper(0, 0, data, 0);
    pageEmpty = isPageEmpty(data);
    if (pageEmpty == false) {
      //PRINTD_2("ERROR: Page 0 is no longer empty!!!!!!!\r\n");
	  //PRINTD_2("Page 0->\r\n");
      //DebugRoutines::dumpBuffer(data, 256);
	  //PRINTD_2("Page 0->\r\n");
      readPages_Helper(0, 0, data, 0);
      //DebugRoutines::dumpBuffer(data, 256);
	  //PRINTD_2("Page 0->\r\n");
      readPages_Helper(0, 0, data, 0);
      //DebugRoutines::dumpBuffer(data, 256);
	  if (ptr != NULL) {
		//PRINTD_2("currentPage=%d ", (int)currentPage);
		//PRINTD_2("pagesWrittenTo=%d\r\n", (int)pagesWrittenTo);
		//PRINTD_2("Writing page data->\r\n");
		//DebugRoutines::dumpBuffer(ptr, 256);
	  }
      while (1) ;
    }
}

//******************************************************************************
int8_t S25FS512::quadIoRead_Pages(uint32_t address, uint8_t *buffer, uint32_t numberOfPages) {
	uint8_t cmdArray[5];
	uint8_t *ptr;
	uint8_t last;
	uint32_t i;
	cmdArray[0] = 0xEC; //0xEB;
	cmdArray[1] = (address >> 24) & 0xFF;
	cmdArray[2] = (address >> 16) & 0xFF;
	cmdArray[3] = (address >> 8) & 0xFF;
	cmdArray[4] = (address >> 0) & 0xFF;
	//reg_write_read_multiple_quad(cmdArray,4,flashBuffer,32);
/*	last = 0;
	num = 0;
	inc = 32;
	while (num < number) {*/
	ptr = buffer;
	last = 0;
	// only send the command
	reg_write_read_multiple_quad_last(cmdArray, 5, ptr, 0, 0);
	//wait_ms(1);
	reg_write_read_multiple_quad_last(cmdArray, 0, ptr, 5, 0);
	//wait_ms(1);
	for (i = 0; i < numberOfPages; i++) {
		reg_write_read_multiple_quad_last(cmdArray,0,ptr,64, 0);
		//wait_ms(1);
		ptr += 64;
		reg_write_read_multiple_quad_last(cmdArray,0,ptr,64, 0);
		//wait_ms(1);
		ptr += 64;
		reg_write_read_multiple_quad_last(cmdArray,0,ptr,64, 0);
		//wait_ms(1);
		ptr += 64;
		// check if this is the last page
		if ((i + 1) == numberOfPages) {
			last = 1;
		}
		reg_write_read_multiple_quad_last(cmdArray,0,ptr,64, last);
		//wait_ms(1);
		ptr += 64;
	}
	return 0;
}

//******************************************************************************
int8_t S25FS512::quadIoRead_PartialPage(uint32_t address, uint8_t *buffer, uint32_t numberOfBytesInPage) {
	uint8_t cmdArray[5];
	uint8_t *ptr;
	uint8_t last;
	uint32_t packetSize;
	uint32_t xferCount;
	cmdArray[0] = 0xEC; //0xEB;
	cmdArray[1] = (address >> 24) & 0xFF;
	cmdArray[2] = (address >> 16) & 0xFF;
	cmdArray[3] = (address >> 8) & 0xFF;
	cmdArray[4] = (address >> 0) & 0xFF;
	//reg_write_read_multiple_quad(cmdArray,4,flashBuffer,32);
	ptr = buffer;
	last = 0;
	// only send the command
	reg_write_read_multiple_quad_last(cmdArray, 5, ptr, 0, 0);
	//wait_ms(1);
	reg_write_read_multiple_quad_last(cmdArray, 0, ptr, 5, 0);
	//wait_ms(1);

	xferCount = 0;
	do { 
		packetSize = 64;
		if ((xferCount + packetSize) > numberOfBytesInPage) {
			packetSize = numberOfBytesInPage;
			last = 1;
		}
		reg_write_read_multiple_quad_last(cmdArray,0,ptr,packetSize, 0);
		xferCount += packetSize;
		ptr += packetSize;
	} while (last != 1);
	return 0;
}

//******************************************************************************
int8_t S25FS512::checkBusy(void) {
	uint8_t cmdArray[5];
	cmdArray[0] = 0x05;
	reg_write_read_multiple_quad(cmdArray,1,flashBuffer,2);
	return flashBuffer[1] & 0x1;
}

//******************************************************************************
void S25FS512::waitTillNotBusy(void) {
	while (checkBusy() == 1) {		}
}

//******************************************************************************
int8_t S25FS512::sectorErase(uint32_t address) {
	uint8_t cmdArray[5];
	cmdArray[0] = 0xDC; //0xD8;
	cmdArray[1] = (address >> 24) & 0xFF;
	cmdArray[2] = (address >> 16) & 0xFF;
	cmdArray[3] = (address >> 8) & 0xFF;
	cmdArray[4] = (address >> 0) & 0xFF;
	return reg_write_read_multiple_quad(cmdArray,5,flashBuffer,0);
}

//******************************************************************************
int8_t S25FS512::parameterSectorErase(uint32_t address) {
	uint8_t cmdArray[5];	
	cmdArray[0] = 0x21;     //0x20 (P4E), 0x21 (4P4E)
	cmdArray[1] = (address >> 24) & 0xFF;
	cmdArray[2] = (address >> 16) & 0xFF;
	cmdArray[3] = (address >> 8) & 0xFF;
	cmdArray[4] = (address >> 0) & 0xFF;
	reg_write_read_multiple_quad(cmdArray,5,flashBuffer,0);
	return 0;
}

//******************************************************************************
int8_t S25FS512::readIdentification(uint8_t *dataArray, uint8_t length) {
	// 4QIOR = 0x9F
	uint8_t cmdArray[1];
	cmdArray[0] = 0x9F;	// read ID command
	return reg_write_read_multiple_quad(cmdArray,1,dataArray,length);
}

//******************************************************************************
uint8_t S25FS512::reset(void) {
	uint8_t cmdArray[8];
	wait_ms(1);
	cmdArray[0] = 0x66;
	reg_write_read_multiple_quad(cmdArray,1,flashBuffer,0);
	wait_ms(1);
	cmdArray[0] = 0x99;
	reg_write_read_multiple_quad(cmdArray,1,flashBuffer,0);
	return 0;
}

//******************************************************************************
uint8_t S25FS512::enableHWReset(void) {
	uint8_t data[8];
	wait_ms(1);
	// CR2V Configuration Register-2 Volatile
	// bit 5
	readAnyRegister(0x00800003,data, 8);
	writeAnyRegister(0x00800003,0x64);
	return 0;
}

//******************************************************************************
uint8_t S25FS512::detect(void) {
	uint8_t array[8];
	uint8_t array2[8];

	// Send WREN
	wren();
	// Send WREN
	wren();
	// delay
	wait_ms(1);
	// Put into quad mode
	//setQuadMode();

	// Send WDIS
//	array[0] = 0x04;
//	test_reg_write_multiple_4wire(array,1);

	// Send WREN
	wren();
	// delay
	wait_ms(1);

	// Send write any register cmd
	writeAnyRegister(0x0003, 0x48);
/*
	//
	// try reading the ID by using 4 wire spi for the command and quad for reading
	//

	// send read id cmd
	array[0] = 0x9F;
	test_reg_write_multiple_4wire(array,1);
	// send read id via quad
	test_reg_read_multiple_quad(array,7);
*/
	// delay
	wait_ms(1);
	array[0] = 0x9F;	// read ID command
	reg_write_read_multiple_quad(array,1,array2,7);

/*
	//uint8_t val[5];
	uint32_t result;
	test_reg_write(0x9F);
	test_reg_read2(0x9F,&result);
	test_reg_read3(0x9F,&result);
	*/
	return 0;
}

//******************************************************************************
int S25FS512::setQuadMode(void) {
  wait_ms(1);
  wren4Wire();
  wait_ms(1);
  writeAnyRegister4Wire(0x800002, 0x02); // set Quad = 1
  wait_ms(1);
  wren4Wire();
  wait_ms(1);
  writeAnyRegister4Wire(0x800003, 0x48); // set 8 latency, set QPI 4-4-4
  return 0;
}

//******************************************************************************
bool S25FS512::isPageEmpty(uint8_t *ptr) {
	int i;
	for (i = 0; i < 256; i++) {
		if (ptr[i] != 0xFF) return false;
	}
	return true;
}

//******************************************************************************
int8_t S25FS512::parameterSectorErase_Helper(uint32_t address) {
	waitTillNotBusy();
	wait_us(100);
	wren();
	wait_us(200);
	parameterSectorErase(address);
	wait_us(100);
	waitTillNotBusy();
	return 0;
}

//******************************************************************************
int8_t S25FS512::sectorErase_Helper(uint32_t address) {
	waitTillNotBusy();
	wait_us(100);
	wren();
	wait_us(200);
    sectorErase(address);
	wait_us(100);
	waitTillNotBusy();
	return 0;
}

//******************************************************************************
int8_t S25FS512::bulkErase_Helper(void) {
	waitTillNotBusy();
	wait_us(100);
	wren();
	wait_us(100);
	bulkErase();
	wait_us(100);
	waitTillNotBusy();
	wait_us(100);
	return 0;
}

//******************************************************************************
// write a page worth of data (256 bytes) from buffer, offset defined where in the buffer to begin write
int8_t S25FS512::writePage_Helper(uint32_t pageNumber, uint8_t *buffer, uint32_t offset) {
	uint8_t *ptr;
	waitTillNotBusy();
	//wait_ms(1);
	wren();
	ptr = &buffer[offset];
	//wait_ms(1);
	pageProgram(pageNumber << 8,ptr);
	wait_us(100);
	waitTillNotBusy();

	//wait_ms(1);
	return 0;
}

//******************************************************************************
// read pages from flash into buffer, offset defined where in the buffer use
int8_t S25FS512::readPages_Helper(uint32_t startPageNumber,uint32_t endPageNumber, uint8_t *buffer, uint32_t offset) {
	int8_t status = 0;
	uint8_t *ptr;
	uint32_t page;
	ptr = &buffer[offset];
	waitTillNotBusy();
	for (page = startPageNumber; page <= endPageNumber; page++) {
		wait_us(100);
		status = quadIoRead_Pages((uint32_t)(page << 8), (uint8_t *)ptr, 1);
		ptr += 0x100;
	}
	return status;
}

//******************************************************************************
int8_t S25FS512::readPartialPage_Helper(uint32_t pageNumber, uint8_t *buffer, uint32_t count) {
	int8_t status = 0;
	waitTillNotBusy();
	status = quadIoRead_PartialPage((uint32_t)(pageNumber << 8), (uint8_t *)buffer, count);
	return status;
}


