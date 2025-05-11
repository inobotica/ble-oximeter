class connectionCallback : public BLEClientCallbacks {
  void onConnect(BLEClient *pclient) {
    txReason = TX_REASON_BLE_CONNECTED;
  }

  void onDisconnect(BLEClient *pclient) {
    device_connected = false;
    myDevice = nullptr;
    deviceAddress = "none";
    bleSignalQuality = -1;
    txReason = TX_REASON_BLE_DISCONNECTED;
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
      bleSignalQuality = myDevice->getRSSI();
      bleSignalQuality = constrain(bleSignalQuality,-100,-50);
      bleSignalQuality = map(bleSignalQuality,-100,-50,0,100);

      heartRate = (int) data[1];
      saturation = (int) data[2];
      Serial.print("MAC:");
      Serial.print(deviceAddress);
      Serial.print(" | Heart rate:");
      Serial.print(heartRate);
      Serial.print(" | Saturation:");
      Serial.print(saturation);
      Serial.print(" | signal:");
      Serial.println(bleSignalQuality);

      txReason = TX_REASON_BLE;
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


