void readSMS() { 
  String res;
  modem.sendAT("+CMGL=\"ALL\"");  
  modem.waitResponse(500L, res);
  res.replace("\"", "");
  res.replace("\r", "");
  res.replace("\n", ",");
  res.replace(",,", ",");
  res.replace("OK", "");
  res.replace("ok", "");
  res.replace(",,", "");
  Serial.println("SMS:"+res);

  if(res.indexOf("CMGL")>=0) {
    String command = res.substring(res.lastIndexOf(",")+1);
    Serial.println("Command: "+command);
    processCommand(command);
  }    
  
  modem.sendAT("+CMGD=,4");
  modem.waitResponse(1000L);
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
    ESP.restart();

  } else if(commandLower.indexOf("status")>=0){
    // Sends status report
    Serial.println("Processing STATUS command!");
    sendData = true;
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