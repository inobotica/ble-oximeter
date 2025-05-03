void modemPowerOn(){
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, LOW);
  delay(1000);
  digitalWrite(PWR_PIN, HIGH);
}

void modemPowerOff(){
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, HIGH);
  delay(1500);
  digitalWrite(PWR_PIN, LOW);
}

void modemRestart(){
  modemPowerOff();
  delay(1000);
  modemPowerOn();
}

void setupGSM(){
  String res = "";
  Serial.println("========SIMCOMATI======");
  modem.sendAT("+SIMCOMATI");
  modem.waitResponse(1000L, res);
  //res.replace(GSM_NL "OK" GSM_NL, "");
  Serial.println(res);
  res = "";
  Serial.println("=======================");

  Serial.println("=====Preferred mode selection=====");

  // Set to AUTO network selection
  modem.sendAT("+CNMP=2");
  if (modem.waitResponse(1000L, res) == 1) {
    //res.replace(GSM_NL "OK" GSM_NL, "");
    Serial.println(res);
  }
  res = "";

  modem.sendAT("+CNMP?");
  if (modem.waitResponse(1000L, res) == 1) {
    //res.replace(GSM_NL "OK" GSM_NL, "");
    Serial.println(res);
  }
  res = "";
  Serial.println("=======================");


  Serial.println("=====Preferred selection between CAT-M and NB-IoT=====");
  // Set to CAT-M band
  modem.sendAT("+CMNB=1");
  if (modem.waitResponse(1000L, res) == 1) {
    //res.replace(GSM_NL "OK" GSM_NL, "");
    Serial.println(res);
  }
  res = "";

  modem.sendAT("+CMNB?");
  if (modem.waitResponse(1000L, res) == 1) {
    //res.replace(GSM_NL "OK" GSM_NL, "");
    Serial.println(res);
  }
  res = "";
  Serial.println("=======================");
}

void printModemInfo(){
  String name = modem.getModemName();
  Serial.println("Modem Name: " + name);

  String modemInfo = modem.getModemInfo();
  Serial.println("Modem Info: " + modemInfo);

  String cop = modem.getOperator();
  Serial.println("Operator: " + cop);
}

void connectToGPRS(){  
  modem.gprsConnect(apn, gprsUser, gprsPass);
}

void reconnectToGPRS(){
  for (int i = 0; i <= 4; i++) {
    uint8_t network[] = {
        2,  /*Automatic*/
        13, /*GSM only*/
        38, /*LTE only*/
        51  /*GSM and LTE only*/
    };
    Serial.printf("Try %d method\n", network[i]);
    modem.setNetworkMode(network[i]);
    delay(3000);
    bool isConnected = false;
    int tryCount = 60;
    while (tryCount--) {
      int16_t signal =  modem.getSignalQuality();
      Serial.print("Signal: ");
      Serial.print(signal);
      Serial.print(" ");
      Serial.print("isNetworkConnected: ");
      isConnected = modem.isNetworkConnected();
      Serial.println( isConnected ? "CONNECT" : "NO CONNECT");
      if (isConnected) {
        break;
      }
      delay(1000);
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
    if (isConnected) {
        break;
    }
  }
}
void disconnectFromGPRS(){
  modem.gprsDisconnect();
}

void getGPRSInfo(){
  String res = "";
  modem.sendAT("+CPSI?");

  if (modem.waitResponse(1000L, res) == 1) {
    Serial.println(res);
  }
}

void unlockSIM(){
  // Unlock your SIM card with a PIN if needed
  String sim = String(modem.getSimStatus());
  Serial.println("SIM Status: " + sim);

  if ( GSM_PIN && modem.getSimStatus() != 3 ) {
    bool result = modem.simUnlock(GSM_PIN);
    Serial.println("SIM: " + String(result));
  }
}
