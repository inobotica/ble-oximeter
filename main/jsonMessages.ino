String composeMessage(){
  
  StaticJsonDocument<200> jsonDoc;
  StaticJsonDocument<200> stateJsonDoc;
  StaticJsonDocument<200> gpsJsonDoc;
  StaticJsonDocument<200> rtcJsonDoc;

  if(RTCTimestamp.length()>0){    
    rtcJsonDoc["timestamp"] = RTCTimestamp;
  }else{    
    rtcJsonDoc["timestamp"] = nullptr;
  }  
  
  if(timestamp.length()>0){    
    gpsJsonDoc["timestamp"] = timestamp;
  }else{    
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
  stateJsonDoc["gsm_signal"] = gsmSignalQuality;
  stateJsonDoc["ble_signal"] = bleSignalQuality;
  stateJsonDoc["gps_signal"] = gpsSignalQuality;
  stateJsonDoc["imei"] = imei;
  stateJsonDoc["mac"] = deviceAddress;
  stateJsonDoc["tx_reason"] = txReason;
  stateJsonDoc["hall_sensor"] = current;
  stateJsonDoc["uptime"] = uptime;
  stateJsonDoc["phone_number"] = phoneNumber;
  stateJsonDoc["is_machine_on"] = isMachineOn;
  
  jsonDoc["state"] = stateJsonDoc;
  jsonDoc["gps"] = gpsJsonDoc;
  jsonDoc["rtc"] = rtcJsonDoc;

  String jsonString;
  serializeJson(jsonDoc, jsonString);

  return jsonString;
}