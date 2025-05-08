void readConfiguration(){
  preferences.begin(CONFIGURATION_NAMESPACE, false);

  // APN config
  String apnUrl = preferences.getString("apn", "internet.movistar.com.co");
  String apnUser = preferences.getString("user", "movistar");
  String apnPass = preferences.getString("pass", "movistar");

  apnUrl.toCharArray(apn, sizeof(apn));
  apnUser.toCharArray(gprsUser, sizeof(gprsUser));
  apnPass.toCharArray(gprsPass, sizeof(gprsPass));

  // Server config
  int serverPort = preferences.getInt("port", 80);
  String serverUrl = preferences.getString("server", "69.164.197.239");
  String serverResource = preferences.getString("resource", "/devices/");

  preferences.end();
}

void setAPN(String apnUrl, String apnUser, String apnPass){
  preferences.begin(CONFIGURATION_NAMESPACE, false);
  preferences.putString("apn", apnUrl);
  preferences.putString("user", apnUser);
  preferences.putString("pass", apnPass);
  preferences.end();
}

void setServer(String serverUrl, String serverPort, String serverResource){
  preferences.begin(CONFIGURATION_NAMESPACE, false);
  preferences.putInt("port", serverPort);
  preferences.putString("server", serverUrl);
  preferences.putString("resource", serverResource);
  preferences.end();
}
