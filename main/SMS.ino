void readSMS() { 
  String res;
  modem.sendAT("+CMGL=\"ALL\"");  
  modem.waitResponse(500L, res);
  res.replace("\"", "");
  res.replace("\r", "");
  res.replace("\n", ",");
  res.replace(",,", ",");
  res.replace("OK", "");
  res.replace("ok", "");
  res.replace(",,", "");
  Serial.println("SMS:"+res);

  if(res.indexOf("CMGL")>=0) {
    String command = res.substring(res.lastIndexOf(",")+1);
    Serial.println("Command: "+command);
  }    
  
  modem.sendAT("+CMGD=,4");
  modem.waitResponse(1000L);
}

/*
void readSMS2(){
  String sms = modem.getSMS(1);  // Read SMS from slot 1
  Serial.println("Received SMS: " + sms);
}
*/