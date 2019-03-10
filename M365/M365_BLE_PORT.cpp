#include "M365_BLE_PORT.h"
#include <cQueue.h>

#define  IMPLEMENTATION  FIFO

static bool doConnect;
static BLEAddress *pServerAddress;
Queue_t    q;

M365_BLE_PORT::M365_BLE_PORT()
{
  doConnect = false;
}

static void notifyCallback(
  BLERemoteCharacteristic *pBLERemoteCharacteristic,
  uint8_t *pData,
  size_t length,
  bool isNotify)
{
  uint8_t error;
  Serial.print("Notify callback  ");

  for (int i = 0; i < length; i++)
  {
    q_push(&q, &pData[i]);
  }
  //    error = ninebot_parse(pData, length, &NB_BLE_data_in);

  //    if (error == 0)
  //    {
  //        Serial.println("Command respond : " + String(NB_BLE_data_in.command));
  //        for (int i = 0; i < NB_BLE_data_in.len; i++)
  //        {
  //            Serial.printf("%02X", NB_BLE_data_in.data[i]);
  //        }
  //        Serial.println(" ");
  //
  //        memcpy(dataIn, NB_BLE_data_in.data, NB_BLE_data_in.len);
  //        //    dataIn = NB_BLE_data_in.data;
  //    }
  //    dataAccepted = true;
}

void M365_BLE_PORT::MyAdvertisedDeviceCallbacks::onResult(BLEAdvertisedDevice advertisedDevice)
{
  /**
      Called for each advertising BLE server.
  */
  //    void onResult(BLEAdvertisedDevice advertisedDevice)
  //    {
  Serial.print("BLE Advertised Device found with RSSI : " + String(advertisedDevice.getRSSI()) + ": ");
  Serial.println(advertisedDevice.toString().c_str());

  if (advertisedDevice.haveName() && advertisedDevice.getName().compare(scooterName) == 0)
  {
    //
    Serial.print("Found our device!  address: ");
    advertisedDevice.getScan()->stop();

    pServerAddress = new BLEAddress(advertisedDevice.getAddress());
    doConnect = true;

  } // Found our server
  //    }     // onResult
};        // MyAdvertisedDeviceCallbacks
void M365_BLE_PORT::startScanningForScooters()
{
  BLEDevice::init("");
  BLEScan *pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(30);
}
bool M365_BLE_PORT::connectToServer(BLEAddress pAddress)
{
  Serial.print("Forming a connection to ");
  Serial.println(pAddress.toString().c_str());

  pClient = BLEDevice::createClient();
  // Connect to the remove BLE Server.
  pClient->connect(pAddress);
  Serial.println(" - Connected to server");
  BLERemoteService *pRemoteService = pClient->getService(serviceUUID);
  pRemoteCharacteristicR = pRemoteService->getCharacteristic(charUUIDREAD);

  pBLERemoteDescriptor = pRemoteCharacteristicR->getDescriptor(charREADDescriptor);
  uint8_t descriptorNew[2];
  descriptorNew[0] = 1;
  descriptorNew[1] = 0;
  pBLERemoteDescriptor->writeValue(descriptorNew, 2);

  pRemoteCharacteristicW = pRemoteService->getCharacteristic(charUUIDWRITE);
  pRemoteCharacteristicR->registerForNotify(notifyCallback);
}

void M365_BLE_PORT::disconnectFromServer()
{
  pClient->disconnect();
  Serial.println(" - disconnect from server");
}



void M365_BLE_PORT::initi()
{

  q_init(&q, 1, 50, IMPLEMENTATION, false);

  startScanningForScooters();

  while (doConnect == false)
  {
    delay(100);
  }
  if (connectToServer(*pServerAddress))
  {
    Serial.println("We are now connected to the M365.");
    connected = true;
  }
  else
  {
    Serial.println("We have failed to connect to the server; there is nothin more we will do.");
  }
}

uint16_t M365_BLE_PORT::available()
{
  return q_getCount(&q);
  //  return _available;
}
bool M365_BLE_PORT::write(uint8_t* data, size_t length)
{
  pRemoteCharacteristicW->writeValue(data, length, false);
}

bool M365_BLE_PORT::isConnected() {
  return connected;
}


unsigned char M365_BLE_PORT::read() {
  unsigned char results;
  q_pop(&q, &results);
  return results;
}
unsigned char M365_BLE_PORT::peek() {
  unsigned char results;
  q_peek(&q, &results);
  return results;
}

