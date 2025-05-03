// Original code: https://github.com/Xinyuan-LilyGO/LilyGO-T-SIM7000G/blob/master/examples/Arduino_NetworkTest/Arduino_NetworkTest.ino

#define TINY_GSM_MODEM_SIM7000
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb
#define TINY_GSM_MODEM_SIM7000SSL
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false

#define SerialAT Serial1
// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// set GSM PIN, if any
#define GSM_PIN ""

#include <TinyGsmClient.h>
#include <Ticker.h>
#include <ArduinoJson.h>

// BLE variables
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEClient.h>

int scanTime = 5;  //In seconds
BLEScan *pBLEScan;
BLEClient* pClient;
BLERemoteCharacteristic* remoteCharacteristic;
BLEAdvertisedDevice* myDevice;

// BLE Device variables
static BLEUUID serviceUUID("0000fee0-0000-1000-8000-00805f9b34fb");
static BLEUUID charUUID("0000fee1-0000-1000-8000-00805f9b34fb");

bool device_scanned = false;
bool device_connected = false;

// APN credentials
const char apn[]  = "internet.movistar.com.co";
const char gprsUser[] = "movistar";
const char gprsPass[] = "movistar";

// Server details
const char server[]   = "69.164.197.239";
const char resource[] = "/devices/";
const int  port       = 5000;

// Message details
int heartRate = 0;
int saturation = 0;
int battLevel = 0;
bool isPowered = false;
float lat = 0.0f;
float lon = 0.0f;
String timestamp = "";
String deviceAddress = "";

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
#define UART_BAUD           115200
#define PIN_DTR             25
#define PIN_TX              27
#define PIN_RX              26
#define PWR_PIN             4
#define LED_PIN             12
#define BATT_PIN            35

// Callback for BLE Scanner results
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {    
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
      Serial.println("Found target BLE device!");
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
    }
  }
};

void setup(){
  // Set console baud rate
  SerialMon.begin(115200);

  delay(10);

  pinMode(BATT_PIN, INPUT);

  // Set LED OFF
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  // Turn on GPRS modem
  modemPowerOn();

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

  // BLE setup
  BLEDevice::init(F("BLE Client"));
  pBLEScan = BLEDevice::getScan();  //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);  //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
}

void loop(){
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

  // Validates BLE state and reconnects if needed
  if (!device_connected) {
    BLEScanResults *foundDevices = pBLEScan->start(scanTime, false);
    pBLEScan->clearResults();
  }
  
  if(myDevice && !device_connected){
    connectToDevice();
  }

  // Send data
  if (modem.isGprsConnected() && device_connected){
    sendPostRequest();
    delay(10000);
  }

}