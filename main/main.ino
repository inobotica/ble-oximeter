// Original code: https://github.com/Xinyuan-LilyGO/LilyGO-T-SIM7000G/blob/master/examples/Arduino_NetworkTest/Arduino_NetworkTest.ino
/*
Requirements:
- RTClib: Adafruit

SIMs:
- 3189088129

*/
#define TINY_GSM_MODEM_SIM7000
#define TINY_GSM_RX_BUFFER 1024  // Set RX buffer to 1Kb
#define TINY_GSM_MODEM_SIM7000SSL
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false
#define CONFIGURATION_NAMESPACE "configuration"
#define CMD_APN 1
#define CMD_SERVER 2

#define SerialAT Serial1
// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// set GSM PIN, if any
#define GSM_PIN ""

#include <TinyGsmClient.h>
#include <Ticker.h>
#include <SPI.h>
#include <SD.h>
#include <ArduinoJson.h>
#include <Preferences.h>

// RTC
#include "RTClib.h"
RTC_DS3231 rtc;
bool isTimeSet = false;
String RTCTimestamp = "";

// BLE variables
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEClient.h>

int scanTime = 5;  //In seconds
BLEScan* pBLEScan;
BLEClient* pClient;
BLERemoteCharacteristic* remoteCharacteristic;
BLEAdvertisedDevice* myDevice;

// BLE Device variables
static BLEUUID serviceUUID("0000fee0-0000-1000-8000-00805f9b34fb");
static BLEUUID charUUID("0000fee1-0000-1000-8000-00805f9b34fb");
bool device_scanned = false;
bool device_connected = false;
int bleSignalQuality = -1;

// APN credentials
Preferences preferences;
char apn[40] = "";
char gprsUser[15] = "";
char gprsPass[15] = "";

// Server details
char server[100] = "";
char resource[20] = "";
int port = 0;

// Message details
int heartRate = 0;
int saturation = 0;
int battLevel = 0;
bool isPowered = false;
float lat = 0.0f;
float lon = 0.0f;
String timestamp = "";
String deviceAddress = "";
String imei = "";
int gsmSignalQuality = -1;
int gpsSignalQuality = -1;
bool requestResult = false;

// SD variables
bool isThereSD = false;
String filename = "/data.log";
int numberOfLines = 0;

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

//TinyGsmClientSecure client(modem);
TinyGsmClient client(modem);

// LilyGO T-SIM7000G Pinout
#define UART_BAUD 115200
#define PIN_DTR 25
#define PIN_TX 27
#define PIN_RX 26
#define PWR_PIN 4
#define LED_PIN 12
#define BATT_PIN 35

#define SD_MISO 2
#define SD_MOSI 15
#define SD_SCLK 14
#define SD_CS 13

// Timer variables
hw_timer_t* timer = NULL;  // Create a hardware timer object
int periodCounter = 0;
bool sendData = true;
int reportPeriod = 0;

// Timer interrupt to blink when ble is disconnected
void IRAM_ATTR onTimer() {
  periodCounter++;
  if (periodCounter >= (reportPeriod*60)) {
    periodCounter = 0;
    sendData = true;
  }
}

// Callback for BLE Scanner results
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
      Serial.println("Found target BLE device!");
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
    }
  }
};

void setup() {
  // Set console baud rate
  SerialMon.begin(115200);

  delay(10);

  pinMode(BATT_PIN, INPUT);

  // Set LED OFF
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  // Read configuration
  readConfiguration();

  // Setting RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  // Turn on GPRS modem
  modemPowerOn();

  // Init SD card
  initSDCard();
  
  SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);
  delay(10000);

  // Restarts modem if needed
  if (!modem.init()) {
    modemRestart();
    delay(2000);
    Serial.println("Failed to restart modem, attempting to continue without restarting");
    return;
  }

  // GSM setup
  setupGSM();
  enableGPS();
  connectToGPRS();

  // Setup timer to blink when BLE is not conected
  timer = timerBegin(1000000); // Configure timer: timer 0, prescaler of 80 (1 µs per tick), count up
  timerAttachInterrupt(timer, &onTimer); // Attach the timer to the interrupt
  timerAlarm(timer, 1000000, true, 0); // Set the timer to trigger every 1000ms (1.000.000 µs)

  // BLE setup
  BLEDevice::init(F("BLE Client"));
  pBLEScan = BLEDevice::getScan();  //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);  //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
}

void loop() {
  String res;
  Serial.println("================");

  // Restarts modem if needed
  if (!modem.init()) {
    modemRestart();
    delay(2000);
    Serial.println("Failed to restart modem, attempting to continue without restarting");
    return;
  }

  getLocation();
  readBatteryLevel();

  Serial.println("Scanning ble...");
  // Validates BLE state and reconnects if needed
  if (!device_connected) {
    BLEScanResults* foundDevices = pBLEScan->start(scanTime, false);
    pBLEScan->clearResults();
  }

  
  if (myDevice && !device_connected) {
    Serial.println("connecting ble...");
    connectToDevice();
  }

  
  // Send data when device is connected
  if ((modem.isGprsConnected() && device_connected) || sendData) {
    Serial.println("Sending data...");
    requestResult = sendPostRequest();
    if(requestResult && isThereSD){
      appendReportToFile();
    }
    sendData = false;
  }

  Serial.println("Reading time...");
  getTime();
  readSMS();
}