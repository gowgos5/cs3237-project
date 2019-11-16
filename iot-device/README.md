MAXREFDES100 Code
===================================

This code is based off the HSP-RPC-GUI sample code provided by Maxim. 

Changes made to the code to suit our use case are the following:

- Initialised pulse oximeter data to MultiMode: Green LED enabled and Red and IR LED disabled
- Implemented code to transmit pulse oximeter data via BLE
- Modified HspBle.cpp to only transmit accelerometer and pulse oximeter data 
- Synchronized data rate of accelerometer with pulse oximeter
- Removed initialisiation of sensors not utilised for power management

Sensors Utilised 
--------------

- MAX30101 (Pulse Oximeter)
- LIS2DH (Accelerometer)

Getting Started
---------------

- Import code to OS MBED Online Compiler
- Compile code at online compiler and download .bin file
- Connect edge device to programming board and connect programming board to laptop
- Drag and drop .bin file