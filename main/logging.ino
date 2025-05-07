void initSDCard(){
  SPI.begin(SD_SCLK, SD_MISO, SD_MOSI);
  if (!SD.begin(SD_CS)) {
      Serial.println("> It looks like you haven't inserted the SD card..");
      isThereSD = false;
  } else {
      uint32_t cardSize = SD.cardSize() / (1024 * 1024);
      String str = "> SDCard Size: " + String(cardSize) + "MB";
      Serial.println(str);
      isThereSD = true;
  }

  // Creates new file if doesn't exists
  if(isThereSD){
    if (!SD.exists(filename)) {
      Serial.println(F("Log file doesn't exist. Creating log file..."));      
      File file = SD.open(filename, FILE_WRITE);
      file.close();
    }
  }
}

void createFile(){
  File file = SD.open(filename, FILE_WRITE);
  if (file && isThereSD) {
      file.println("Hello, ESP32 SD Card!");
      file.close();
      Serial.println("File written successfully.");
  } else {
      Serial.println("Failed to create file.");
  }
}

void readFile(){
  File file = SD.open(filename);
  if (file && isThereSD) {
      Serial.println("Reading file:");
      while (file.available()) {
          Serial.write(file.read());
      }
      file.close();
  } else {
      Serial.println("Failed to open file.");
  }
}

void appendReportToFile() {
  if(!isThereSD){
    Serial.println("No SD card!");
    return;
  }

  File file = SD.open(filename, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  String message = composeMessage();
  if (file.println(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}