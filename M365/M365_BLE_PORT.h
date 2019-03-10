#ifndef M365_BLE_PORT_H
#define M365_BLE_PORT_H
#include "BLEDevice.h"
#include <Arduino.h>

static BLEUUID serviceUUID("6e400001-b5a3-f393-e0a9-e50e24dcca9e"); //6e400001-b5a3-f393-e0a9-e50e24dcca9e
static std::string scooterName("MIScooter4615");

// The characteristic of the remote service we are interested in.
static BLEUUID charUUIDWRITE("6e400002-b5a3-f393-e0a9-e50e24dcca9e");
static BLEUUID charUUIDREAD("6e400003-b5a3-f393-e0a9-e50e24dcca9e");
static BLEUUID charREADDescriptor("00002902-0000-1000-8000-00805f9b34fb");

class M365_BLE_PORT
{
  private:
    uint16_t _available;
    BLERemoteCharacteristic *pRemoteCharacteristicR;
    BLERemoteCharacteristic *pRemoteCharacteristicW;
    BLERemoteDescriptor *pBLERemoteDescriptor;

   
   
    bool connected = false;
    BLEClient *pClient;

    void startScanningForScooters();
    bool connectToServer(BLEAddress pAddress);
    void disconnectFromServer();
    class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
      public:
        void onResult(BLEAdvertisedDevice advertisedDevice);
    };

  public:
    M365_BLE_PORT();
    void initi();
    uint16_t available();
    bool write(uint8_t* data, size_t length);
    bool isConnected();
    unsigned char read();
    unsigned char peek();
};

#endif
