void readBatteryLevel(){
  int batteryVolts = int(2*analogReadMilliVolts(BATT_PIN));
  bool power = false;

  if(batteryVolts<2000){
    battLevel = -1;
    power = true;
  } else {
    battLevel = batteryVolts;
    power = false;
  }

  if(power && !isPowered){
    txReason = TX_REASON_POWER_ON;
    sendData = true;
    Serial.println("BATT: device powered ON!");
  } else if (!power && isPowered){
    txReason = TX_REASON_POWER_OFF;
    sendData = true;
    Serial.println("BATT: device powered OFF!");
  }

  isPowered = power;

  Serial.print("BATT:");
  if (battLevel == -1){
    Serial.println(" Charging");
  } else {
    Serial.println(battLevel);
  }
  
  battLevel = constrain(batteryVolts, 2500, 4100);
  battLevel = map(battLevel, 2500, 4100, 0, 100);
}

void readHallSensor(){  
  long average_reading = 0;
  
  for(int i=0;i<5;i++){
    average_reading = average_reading + long(analogReadMilliVolts(HALL_PIN));
    delay(100);
  }
  
  average_reading = average_reading/5;
  int tempCurrent = abs(int(1000*(((float)average_reading - 2500.0f)/sensibility))) - 400;
  current = int(current + tempCurrent)/2;

  aggregatedCurrent = aggregatedCurrent + current;
  currentCounter++;

  if(currentCounter>=numberOfSamples){
    currentCounter = 0;
    tempCurrent = aggregatedCurrent/numberOfSamples;
    aggregatedCurrent = 0;
    Serial.println("ENTER!AAAAAA");

    if(tempCurrent>=currentThreshold && !isMachineOn){
      txReason = TX_REASON_TURNED_ON;
      sendData = true;
      isMachineOn = true;
      Serial.println("O2: oxygen powered ON!");
    } else if(tempCurrent<currentThreshold && isMachineOn){
      txReason = TX_REASON_TURNED_OFF;
      sendData = true;
      isMachineOn = false;
      Serial.println("O2: oxygen powered OFF!");
    }
  }

  Serial.print("Hall sensor (mA): ");
  Serial.println(current);
}

String waitForResponse(String command, unsigned long timeout) {
  String message = "";
  String line = "";
  unsigned long startTime = millis();

  // Enviar el command antes de escuchar
  if(command.length()>0){
    SerialAT.println(command);
  }
  
  while (millis() - startTime < timeout) {
    while (SerialAT.available()) {
      char c = SerialAT.read();

      if (c == '\n' || c == '\r') {
        if (line.length() > 0) {
          if (message.length() > 0) {
            message += "\n";  // separador entre respuestas
          }
          message += line;
          line = "";  // limpiar buffer temporal
        }
      } else {
        line += c;
      }
    }
  }

  // Si quedó algo pendiente sin ENTER al final
  if (line.length() > 0) {
    if (message.length() > 0) {
      message += "\n";
    }
    message += line;
  }

  return message;
}




