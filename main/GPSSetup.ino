void enableGPS(){
  Serial.println("\n---Enabling GPS---\n");
  // Set SIM7000G GPIO4 HIGH ,turn on GPS power
  // CMD:AT+SGPIO=0,4,1,1
  // Only in version 20200415 is there a function to control GPS power
  modem.sendAT("+SGPIO=0,4,1,1");
  if (modem.waitResponse(10000L) != 1) {
    DBG(" SGPIO=0,4,1,1 false ");
  }
  modem.enableGPS();
}

void disableGPS(){
  modem.disableGPS();

  // Set SIM7000G GPIO4 LOW ,turn off GPS power
  // CMD:AT+SGPIO=0,4,1,0
  // Only in version 20200415 is there a function to control GPS power
  modem.sendAT("+SGPIO=0,4,1,0");
  if (modem.waitResponse(10000L) != 1) {
    DBG(" SGPIO=0,4,1,0 false ");
  }
  Serial.println("\n---GPS disabled---\n");
}

void printLatLon(){  

  while (1) {
    if (modem.getGPS(&lat, &lon)) {
      Serial.printf("lat:%f lon:%f\n", lat, lon);
      break;
    } else {
      Serial.print("getGPS ");
      Serial.println(millis());
    }
    delay(2000);
  }
}

void getLocation(){
  float gps_latitude  = 0;
  float gps_longitude = 0;
  float gps_speed     = 0;
  float gps_altitude  = 0;
  int   gps_vsat      = 0;
  int   gps_usat      = 0;
  float gps_accuracy  = 0;
  int   gps_year      = 0;
  int   gps_month     = 0;
  int   gps_day       = 0;
  int   gps_hour      = 0;
  int   gps_minute    = 0;
  int   gps_second    = 0;
  
  if (modem.getGPS(&gps_latitude, &gps_longitude, &gps_speed, &gps_altitude,
                    &gps_vsat, &gps_usat, &gps_accuracy, &gps_year, &gps_month,
                    &gps_day, &gps_hour, &gps_minute, &gps_second)) { 
    lat = gps_latitude;
    lon = gps_longitude; 
    timestamp = String(gps_year)+"-"+String(gps_month)+"-"+String(gps_day)+" "+String(gps_hour)+":"+String(gps_minute)+":"+String(gps_second);
  } else {
    Serial.println("Couldn't get GPS/GNSS/GLONASS location, retrying in 15s.");
  }
}