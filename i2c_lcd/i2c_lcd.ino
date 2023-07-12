#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <WiFi.h>
#include <WebServer.h>

#include "thermometer.h"
#include "Progress.h"

const char *SSID = "GoAway";
const char *PWD = "D@n1elSiAnca";

#define DHTPIN 23
#define DHTTYPE DHT22

Thermometer thermometer(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define SCREEN_WIDTH 128  // OLED width,  in pixels
#define SCREEN_HEIGHT 32  // OLED height, in pixels
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
// create an OLED display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

StaticJsonDocument<250> jsonDocument;
char buffer[250];
void create_json(char *tag, float value, char *unit) {
  jsonDocument.clear();
  jsonDocument["type"] = tag;
  jsonDocument["value"] = value;
  jsonDocument["unit"] = unit;
  serializeJson(jsonDocument, buffer);
}

void add_json_object(char *tag, float value, char *unit) {
  JsonObject obj = jsonDocument.createNestedObject();
  obj["type"] = tag;
  obj["value"] = value;
  obj["unit"] = unit;
}

WebServer server(80);

bool _wiFiConnected = false;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  searchI2c();

  setupLcd();
  setupOled();
  setupWiFi();

  Serial.println("Begining thermometer");
  thermometer.begin();

  setup_routing();
}

void setup_routing() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  Serial.println("Setup Routing");
  server.on("/env", getEnv);

  // start server
  server.begin();
}

void setupLcd() {
  lcd.begin();
  lcd.backlight();
  lcd.print("Loading");
}

void setupOled() {
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("failed to start SSD1306 OLED"));
    while (1)
      ;
  }
  oled.display();
  delay(2000);
  oled.clearDisplay();       // clear display
  oled.setTextSize(2);       // set text size
  oled.setTextColor(WHITE);  // set text color
  oled.setCursor(0, 0);      // set position to display
  oled.display();            // display on OLED
}

void setupWiFi() {
  int status = WL_IDLE_STATUS;
  int retriesCount = 10;
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PWD);
  while (isWiFiConnected() && retriesCount >= 0) {
    retriesCount--;
    status = WiFi.status();
    Serial.print("WiFi Status: ");
    Serial.print(status);
    Serial.print(" ");
    Serial.println(get_wifi_status(status));
    delay(500);
  }

  if (retriesCount >= 0) {
    Serial.println("Successfully connected to WiFi");
    get_network_info();
  } else {
    Serial.println("Wifi Connection Failed");
  }

  showIpAddressOnOled();
}

void get_network_info() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  Serial.print("[*] Network information for ");
  Serial.println("SSID");

  Serial.println("[+] BSSID : " + WiFi.BSSIDstr());
  Serial.print("[+] Gateway IP : ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("[+] Subnet Mask : ");
  Serial.println(WiFi.subnetMask());
  Serial.println((String) "[+] RSSI : " + WiFi.RSSI() + " dB");
  Serial.print("[+] ESP32 IP : ");
  Serial.println(WiFi.localIP());
}

void isWiFiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

String get_wifi_status(int status) {
  switch (status) {
    case WL_IDLE_STATUS:
      return "WL_IDLE_STATUS";
    case WL_SCAN_COMPLETED:
      return "WL_SCAN_COMPLETED";
    case WL_NO_SSID_AVAIL:
      return "WL_NO_SSID_AVAIL";
    case WL_CONNECT_FAILED:
      return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST:
      return "WL_CONNECTION_LOST";
    case WL_CONNECTED:
      return "WL_CONNECTED";
    case WL_DISCONNECTED:
      return "WL_DISCONNECTED";
    default:
      return "NO_WIFI_SHIELD";
  }
}

void loop() {
  delay(2000);
  auto td = thermometer.read();
  jsonDocument.clear();  // Clear json buffer
  add_json_object("temperature", td.getTemperature(), "°C");
  add_json_object("humidity", td.getHumidity(), "%");
  add_json_object("feelsLike", td.getHeatIndex(), "°C");
  printThermometerData(td);
}

void printThermometerData(ThermometerData &td) {
  lcd.setCursor(0, 0);
  lcd.print(td.getTemperature());
  lcd.print((char)223);
  lcd.print("C ");

  lcd.print(td.getHumidity());
  lcd.print("%");
  lcd.print("     ");

  lcd.setCursor(0, 1);
  lcd.print("Feels like ");
  lcd.print(td.getHeatIndex());

  Serial.print("Temp:");
  Serial.print(td.getTemperature());
  Serial.print("°");
  Serial.print("C ");
  Serial.print(" Hum: ");
  Serial.print(td.getHumidity());
  Serial.print("%");
  Serial.print(" Feels Like: ");
  Serial.print(td.getHeatIndex());
  Serial.print("°");
  Serial.println("C ");
}

void searchI2c() {
  byte error, address;
  int nDevices;

  nDevices = 0;
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {                                 /*if I2C device found*/
      Serial.print("I2C device found at address 0x"); /*print this line if I2C device found*/
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX); /*prints the HEX value of I2C address*/
      nDevices++;
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
    }
  }

  if (nDevices == 0) {
    Serial.println("No I2C devices found\n"); /*If no I2C device attached print this message*/
  }
}

void showIpAddressOnOled() {
  oled.clearDisplay();
  oled.setCursor(0, 0);
  oled.print(WiFi.localIP());
  oled.display();
}

void getEnv() {
  Serial.println("Get env");
  server.send(200, "application/json", buffer);
}
