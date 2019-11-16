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
#ifndef _BLUETOOTHLE_H_
#define _BLUETOOTHLE_H_

#include "mbed.h"
#include "BLE.h"
#include "Characteristic.h"

class BluetoothLE {
public:
    BluetoothLE(BLE *ble, int numberOfCharacteristics);
    ~BluetoothLE(void);
    void addCharacteristic(Characteristic *characteristic);
    void initService(uint8_t *serialNumber, uint8_t *deviceName, int nameSize, uint8_t *serviceUUID);
    void notifyCharacteristic(int index, uint8_t *data);


 	/**
	* @brief Return a characteristic based on incoming index
    * @param index index into an array of characteristics
	*/
   Characteristic *getCharacteristic(int index) {
        return characteristics[index];
    }

	/**
	* @brief Get the payload for this characteristic
	*/
    uint8_t *getCharacteristicPayload(int index) {
    	return characteristics[index]->getPayloadPtr();
    }
    typedef void (*PtrFunction)(int index);
    
	/**
	* @brief Used to connect a connection callback
	*/
    template<typename T>
    void onConnect(T *object, void (T::*member)(void)) {
        _onConnect.attach( object, member ); 
    }
	/**
	* @brief Used to connect a disconnection callback
	*/
    template<typename T>
    void onDisconnect(T *object, void (T::*member)(void)) {
        _onDisconnect.attach( object, member );
    }
	/**
	* @brief Used to connect a characteristic written callback
	*/
    void onDataWritten(PtrFunction _onDataWritten) {
        this->_onDataWritten = _onDataWritten;
    }
    /**
	* @brief Get the connection state of the BLE
	* @return true if connection BLE connection is present, false if no connection
	*/
    bool isConnected(void);
    
    /**
	* @brief Perform a notification test
	* @param index Index into an array of characteristic objects
	*/
    void notifyCharacteristicTest(int index);
    
    /**
	* @brief Get the data that was written to the indexed characteristic
	* @return true if connection BLE connection is present, false if no connection
	*/
	uint8_t *getDataWritten(int index, int *length);
    
private:
	/**
	* @brief Disconnection callback
	*/
    void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params);
	/**
	* @brief Connection callback
	*/
    void connectionCallback(const Gap::ConnectionCallbackParams_t *params);
	/**
	* @brief Characteristic written callback
	*/
    void onDataWritten(const GattWriteCallbackParams *params);

	/// array of characteristic class pointers
    Characteristic **characteristics;
	/// array of gatt characteristic pointers
    GattCharacteristic **gattCharacteristics;
	/// pointer to mbed BLE layer
    BLE *ble;
	/// total number of characteristics
    int numberOfCharacteristics;    
	/// flag to keep track of BLE connection state
    bool _isConnected;    
	/// running index for building characteristic array
    int runningIndex;
	/// callback function for when a connection is made
    FunctionPointer _onConnect;
	/// callback function for when a connection is disconnected
    FunctionPointer _onDisconnect; 
	/// callback function for when characteristic data is written
    PtrFunction _onDataWritten;
};

#endif // _BLUETOOTHLE_H_
