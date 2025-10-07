bool sendPostRequest(){
  bool requestStatus = false;
  long ti = millis();

  // Checks for GSM network connection
  if (!modem.isNetworkConnected()) {
    Serial.println("Not connected to network");
    modem.waitForNetwork();
  }

  // Checks for GPRS network connection
  if (!modem.isGprsConnected()) {
      Serial.println("Connecting to GPRS...");
      if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
          Serial.println("GPRS failed");
      }
  }

  // Checks for server connection  
  Serial.println("GPRS OK, trying TCP...");

  if (client.connect(server, port)) {
      Serial.println("TCP Connected!");
  } else {
      Serial.println("TCP Connection failed");
      return false;
  }

  // Making an HTTP POST request  
  String httpRequestData = composeMessage();
  String resourcePath = String(resource) + imei;
  String req = String("POST ") + resourcePath + " HTTP/1.1\r\n";
  client.print(req);
  Serial.print(req);

  if (port!=80 && port!=443){
    req = String("Host: ") + server + ":"+ String(port) + "\r\n";
    client.print(req);
  } else {
    client.print(String("Host: ") + server + "\r\n");
  }
  
  client.println("Connection: keep-alive");
  //client.println("Connection: close");
  client.println("Content-Type: application/json");
  client.print("Content-Length: ");
  client.println(httpRequestData.length());
  client.println();
  client.println(httpRequestData);

  // Response data
  String response = "";
  unsigned long timeout = millis();
  while (client.connected() && millis() - timeout < 10000L) {
    // Print available data (client response from server)
    while (client.available()>0) {
      char c = client.read();
      response.concat(c);
      timeout = millis();
    }
  }

  if(response.indexOf("OK") > 0 || response.indexOf("200") > 0 || response.indexOf("201") > 0){
    Serial.println("OK!!!!!");    
    requestStatus = true;
  } else {
    requestStatus = false;
  }

  Serial.println("Response:");
  Serial.println(response);

  // Close client and disconnect
  client.stop();
  Serial.print("Delta on response: ");
  Serial.println(millis()-ti);

  return requestStatus;
}
