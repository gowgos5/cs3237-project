## IoT Gateway Application

This is the Android application that is used for the gateway device.

### Classes

#### FftAnalyzer

Use of FFT to calculate heart rate based on PPG signals.

#### MqttHelper

Used to publish and subscribe to topics using MQTT.

#### DeviceScanActivity

Used to look for MAXREFDES devices.

#### SensorActivity

Overview page for the device which displays Temperature, Accelerometer and PPG Heart Rate data.

#### DetectionActivity

Displays current status, heart rate and coordinates of user.

#### PredictionActivity

Displays current activity user is partaking in and calendar to select date to view vital statistics.

#### LongTermActivity

Display amount of time active for the day and resting heart rate through out the day.

#### BluetoothLeService

Used for managing connection and data communication with a GATT server hosted on a Bluetooth LE device.

#### GattAttributeLookupTable
Contains effective look-up table of Service UUID as a key and the friendly name of the service as the value.
