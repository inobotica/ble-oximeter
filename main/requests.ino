void sendPostRequest(){
  long ti = millis();

  // Validates if client is already connected
  if(!client.connected()){
    if (!client.connect(server, port)) {
      Serial.println("Connection to server");
      reconnectToGPRS();
      return;
    } else {
      Serial.println("Connected to server!");      
    }
  } else {
    Serial.println("Already connected to server!");      
  }

  // Making an HTTP POST request
  Serial.println("POST request...");
  // Prepare your HTTP POST request data
  String httpRequestData = composeMessage();
  deviceAddress.replace(":", "-");
  String resourcePath = String(resource) + deviceAddress;
  
  client.print(String("POST ") + resourcePath + " HTTP/1.1\r\n");
  client.print(String("Host: ") + server + ":"+ String(port) + "\r\n");
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
  }

  Serial.println("Response:");
  Serial.println(response);

  // Close client and disconnect
  client.stop();
  Serial.print("Delta on response: ");
  Serial.println(millis()-ti);

}