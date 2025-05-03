class connectionCallback : public BLEClientCallbacks {
  void onConnect(BLEClient *pclient) {}

  void onDisconnect(BLEClient *pclient) {
    device_connected = false;
    myDevice = nullptr;
    deviceAddress = "none";
    // Reactivates scanning
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setActiveScan(true);
  }
};

// Callback triggered when new ble message is received
void notifyCallback(
  BLERemoteCharacteristic* characteristic,
  uint8_t* data,
  size_t length,
  bool isNotify) {

    // Checks for correct message payload 
    if(length==4){
      deviceAddress = myDevice->getAddress().toString();
      heartRate = (int) data[1];
      saturation = (int) data[2];
      Serial.print("MAC:");
      Serial.print(deviceAddress);
      Serial.print(" | Heart rate:");
      Serial.print(heartRate);
      Serial.print(" | Saturation:");
      Serial.println(saturation);
    }
}

// Callback to connect to BLE device
void connectToDevice() {
  // Stops scanning after successfull connection
  pBLEScan = BLEDevice::getScan();
  pBLEScan->stop();

  pClient = BLEDevice::createClient();  
  pClient->setClientCallbacks(new connectionCallback());

  if (pClient->connect(myDevice)) {
    device_connected = true;

    BLERemoteService* remoteService = pClient->getService(serviceUUID);
    if (remoteService) {
      remoteCharacteristic = remoteService->getCharacteristic(charUUID);
      if (remoteCharacteristic->canNotify()) {
        remoteCharacteristic->registerForNotify(notifyCallback);
      }
    }
  } else {
    Serial.print(" UPS!");
  }
}


