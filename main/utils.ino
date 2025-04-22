void readBatteryLevel(){
  battLevel = int(2000*analogReadMilliVolts(BATT_PIN));
}