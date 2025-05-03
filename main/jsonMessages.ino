String composeMessage(){
  
  StaticJsonDocument<200> jsonDoc;
  StaticJsonDocument<200> stateJsonDoc;
  StaticJsonDocument<200> gpsJsonDoc;
  StaticJsonDocument<200> rtcJsonDoc;

  if(timestamp.length()>0){
    rtcJsonDoc["timestamp"] = timestamp;
    gpsJsonDoc["timestamp"] = timestamp;
  }else{
    rtcJsonDoc["timestamp"] = nullptr;
    gpsJsonDoc["timestamp"] = nullptr;
  }
  
  if (int(lat)>0) {
    gpsJsonDoc["lat"] = lat;
    gpsJsonDoc["lon"] = lon;
  } else {
    gpsJsonDoc["lat"] = nullptr;
    gpsJsonDoc["lon"] = nullptr;
  }

  // Nested json with oximeter data
  if(heartRate==127 && saturation==127){
    stateJsonDoc["heart_rate"] = nullptr;
    stateJsonDoc["o_saturation"] = nullptr;
  } else {
    stateJsonDoc["heart_rate"] = heartRate;
    stateJsonDoc["o_saturation"] = saturation;
  }
  
  stateJsonDoc["batt_level"] = battLevel;
  stateJsonDoc["is_powered"] = isPowered;
  
  jsonDoc["state"] = stateJsonDoc;
  jsonDoc["gps"] = gpsJsonDoc;
  jsonDoc["rtc"] = rtcJsonDoc;

  String jsonString;
  serializeJson(jsonDoc, jsonString);

  return jsonString;
}