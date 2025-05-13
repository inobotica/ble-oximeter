void readBatteryLevel(){
  int batteryVolts = int(2*analogReadMilliVolts(BATT_PIN));

  if(batteryVolts<2000){
    battLevel = -1;
    isPowered = true;
  } else {
    battLevel = batteryVolts;
    isPowered = false;
  }

  Serial.print("BATT:");
  if (battLevel == -1){
    Serial.println(" Charging");
  } else {
    Serial.println(battLevel);
  }
  
  battLevel = constrain(batteryVolts, 2500, 4100);
  battLevel = map(battLevel, 2500, 4100, 0, 100);
}