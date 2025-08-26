void readSMS() { 
  String res;
  //modem.sendAT("+CMGL=\"REC UNREAD\",0");  
  res = waitForResponse("AT+CMGL=\"REC UNREAD\"", 2000);
  
  /*res.replace("\"", "");
  
  res.replace(",,", ",");
  
  res.replace(",,", "");*/
  Serial.println("read SMS:"+res);

  if(res.indexOf("CMGL")>=0) {
    res = res.substring(res.indexOf("\n")+1);
    res.replace("OK", "");
    res.replace("ok", "");
    res.replace("\r", "");
    res.replace("\n", "");
    
    String command = res.substring(res.lastIndexOf(",")+1);
    Serial.println("Command: "+command);
    processCommand(command);
    clearSMS();  
  }    

}

void clearSMS(){
  modem.sendAT("+CMGD=0,4");
  if(modem.waitResponse(5000L)==1){
    Serial.println("SMSs deleted");
  }  
}

void processCommand(String command){
  String commandLower = command;
  commandLower.toLowerCase();

  if(commandLower.indexOf("apn:")>=0){
    // Sets apn parameters
    Serial.println("Processing APN command!");
    parseCommand(command, CMD_APN);

  } else if(commandLower.indexOf("server:")>=0){
    // Sets server parameters
    Serial.println("Processing SERVER command!");
    parseCommand(command, CMD_SERVER);

  } else if(commandLower.indexOf("reset")>=0){
    // Resets device
    Serial.println("Processing RESET command!");
    clearSMS();
    ESP.restart();

  } else if(commandLower.indexOf("status")>=0){
    // Sends status report
    Serial.println("Processing STATUS command!");
    sendData = true;
    txReason = TX_REASON_NOW;
  } else if(commandLower.indexOf("report")>=0){
    // Sends status report
    Serial.println("Processing REPORT command!");
    int period = command.substring(command.indexOf(":")+1).toInt();
    setReportPeriod(period);
    sendData = true;
    txReason = TX_REASON_PERIOD_SET;
  } else if(commandLower.indexOf("phone")>=0){
    // Sends status report
    Serial.println("Processing PHONE command!");
    String phone = command.substring(command.indexOf(":")+1);
    setPhoneNumber(phone);
    sendData = true;
    txReason = TX_REASON_PHONE_SET;
  }
}

void parseCommand(String command, int type){
  int commaNumber = numberOfOcurrences(command, ';');

  if (commaNumber==2){
    String inputString = command.substring(command.indexOf(":")+1);
    char buffer[40]; 
    String parts[3];
    inputString.toCharArray(buffer, sizeof(buffer));

    char *token = strtok(buffer, ";");
    int index = 0;

    while (token != NULL && index < 3) {
      parts[index] = String(token);
      token = strtok(NULL, ";");
      index++;
    }

    for (int i = 0; i < 3; i++) {
      Serial.print("Part ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.println(parts[i]);
    }

    // Parse command according to instruction
    if(type==CMD_APN){
      setAPN(parts[0], parts[1], parts[2]);
    } else if(type==CMD_SERVER){
      setServer(parts[0], parts[1].toInt(), parts[2]);
    }

  }

}

void parseServerCommand(String command){
  int commaNumber = numberOfOcurrences(command, ';');

  if (commaNumber==2){
    String inputString = command.substring(command.indexOf(":"));
    char buffer[40]; 
    String parts[3];
    inputString.toCharArray(buffer, sizeof(buffer));

    char *token = strtok(buffer, ";");
    int index = 0;

    while (token != NULL && index < 3) {
      parts[index] = String(token);
      token = strtok(NULL, ";");
      index++;
    }

    for (int i = 0; i < 3; i++) {
      Serial.print("Part ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.println(parts[i]);
    }

  }

}

int numberOfOcurrences(String data, char targetChar){
  int count = 0;
  for (int i = 0; i < data.length(); i++) {
    if (data[i] == targetChar) {
      count++;
    }
  }

  return count;
}