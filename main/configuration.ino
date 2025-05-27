void readConfiguration(){
  preferences.begin(CONFIGURATION_NAMESPACE, false);

  // APN config
  String apnUrl = preferences.getString("apn", DEFAULT_APN_URL);
  String apnUser = preferences.getString("user", DEFAULT_APN_USER);
  String apnPass = preferences.getString("pass", DEFAULT_APN_PASS);

  apnUrl.toCharArray(apn, sizeof(apn));
  apnUser.toCharArray(gprsUser, sizeof(gprsUser));
  apnPass.toCharArray(gprsPass, sizeof(gprsPass));

  // Server config  
  String serverUrl = preferences.getString("server", DEFAULT_SERVER_URL);
  port = preferences.getInt("port", DEFAULT_SERVER_PORT);
  String serverResource = preferences.getString("resource", DEFAULT_SERVER_RESOURCE);
  serverUrl.toCharArray(server, sizeof(server));
  serverResource.toCharArray(resource, sizeof(resource));

  // Period config
  reportPeriod = preferences.getInt("reportPeriod", DEFAULT_REPORT_PERIOD);
  // Phone config
  phoneNumber = preferences.getString("phoneNumber", DEFAULT_PHONE_NUMBER);

  preferences.end();

  Serial.println("APN: "+apnUrl+" | user:"+apnUser+" | pass:"+apnPass);
  Serial.println("Server: "+serverUrl+":"+String(port)+serverResource);
  Serial.println("Period: each "+String(reportPeriod)+" mins");
  delay(5000);
}

void setAPN(String apnUrl, String apnUser, String apnPass){
  preferences.begin(CONFIGURATION_NAMESPACE, false);
  preferences.putString("apn", apnUrl);
  preferences.putString("user", apnUser);
  preferences.putString("pass", apnPass);
  preferences.end();

  // Setting local variables
  apnUrl.toCharArray(apn, sizeof(apn));
  apnUser.toCharArray(gprsUser, sizeof(gprsUser));
  apnPass.toCharArray(gprsPass, sizeof(gprsPass));  

  Serial.println("NEW APN: "+apnUrl+" | user:"+apnUser+" | pass:"+apnPass);
}

void setServer(String serverUrl, int serverPort, String serverResource){
  preferences.begin(CONFIGURATION_NAMESPACE, false);
  preferences.putInt("port", serverPort);
  preferences.putString("server", serverUrl);
  preferences.putString("resource", serverResource);
  preferences.end();

  // Setting local variables
  port = serverPort;  
  serverUrl.toCharArray(server, sizeof(server));
  serverResource.toCharArray(resource, sizeof(resource));
  Serial.println("NEW Server: "+serverUrl+":"+String(port)+serverResource);
}

void setReportPeriod(int period){
  preferences.begin(CONFIGURATION_NAMESPACE, false);
  preferences.putInt("reportPeriod", period);  
  preferences.end();

  // Setting local variables
  reportPeriod = period;
  Serial.println("NEW Period: each "+String(reportPeriod)+" mins");
}

void setPhoneNumber(String phone){
  preferences.begin(CONFIGURATION_NAMESPACE, false);  
  preferences.putString("phoneNumber", phone);
  preferences.end();

  // Setting local variables
  phoneNumber = phone;  
  Serial.println("NEW Phone Number: "+ phoneNumber);
}
