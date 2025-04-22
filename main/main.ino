// Original code: https://github.com/Xinyuan-LilyGO/LilyGO-T-SIM7000G/blob/master/examples/Arduino_NetworkTest/Arduino_NetworkTest.ino

#define TINY_GSM_MODEM_SIM7000
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb
#define TINY_GSM_MODEM_SIM7000SSL
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false

#define SerialAT Serial1
// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// See all AT commands, if wanted
// #define DUMP_AT_COMMANDS

// set GSM PIN, if any
#define GSM_PIN ""

#include <TinyGsmClient.h>
#include <Ticker.h>
#include <ArduinoJson.h>

// APN credentials
const char apn[]  = "internet.movistar.com.co";
const char gprsUser[] = "movistar";
const char gprsPass[] = "movistar";

// Server details
const char server[]   = "69.164.197.239";
const char resource[] = "/devices/id_1";
const int  port       = 5000;

// Message details
int heartRate = 0;
int saturation = 0;
int battLevel = 0;
float lat = 0.0f;
float lon = 0.0f;
String timestamp = "";

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

void modemPowerOn(){
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, LOW);
  delay(1000);
  digitalWrite(PWR_PIN, HIGH);
}

void modemPowerOff(){
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, HIGH);
  delay(1500);
  digitalWrite(PWR_PIN, LOW);
}


void modemRestart(){
  modemPowerOff();
  delay(1000);
  modemPowerOn();
}

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

  setupGSM();
  enableGPS();
  connectToGPRS();
  //printLatLon();
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

  if (modem.isGprsConnected()){
    sendPostRequest();
    delay(5000);
  } else {
    reconnectToGPRS();
  }

}