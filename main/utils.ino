void readBatteryLevel(){
  int batteryVolts = int(2*analogReadMilliVolts(BATT_PIN));

  if(batteryVolts<2000){
    battLevel = -1;
    isPowered = true;
  } else {
    battLevel = batteryVolts;
    isPowered = false;
  }

  Serial.print(" | BATT:");
  Serial.println(battLevel);
  //battLevel = constrain(battLevel, 3200, 4200);
  //battLevel = map(battLevel, 3200, 4200, 0, 100);
}