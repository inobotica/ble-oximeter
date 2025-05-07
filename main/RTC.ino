void setTime(int year, int month, int date, int hour, int minute, int second){
  Serial.println("Setting time...");
  rtc.adjust(DateTime(year, month, date, hour, minute, second));
}

void getTime(){
  
  DateTime now = rtc.now();
  char buffer[30];
  sprintf(buffer, "%d-%02d-%02d %02d:%02d:%02d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
  Serial.print("RTC time: ");
  Serial.println(buffer);
  RTCTimestamp = String(buffer);

  /*
  Serial.print(now.year(), DEC);
  Serial.print("-");
  Serial.print(now.month(), DEC);
  Serial.print("-");
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(":");
  Serial.print(now.minute(), DEC);
  Serial.print(":");
  Serial.print(now.second(), DEC);
  Serial.println();
  */
}
