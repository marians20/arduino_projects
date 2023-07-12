//https://www.survivingwithandroid.com/esp32-rest-api-esp32-api-server/
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WebServer.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"

#include "thermometer.h"
#include "network.h"
#include "Progress.h"

char *SSID = "GoAway";
char *PWD = "D@n1elSiAnca";

#define DHTPIN 23
#define DHTTYPE DHT22

Thermometer thermometer(DHTPIN, DHTTYPE);
ThermometerData td(0, 0);

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define SCREEN_WIDTH 128  // OLED width,  in pixels
#define SCREEN_HEIGHT 32  // OLED height, in pixels
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
// create an OLED display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Network network(SSID, PWD);

WebServer server(80);

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

bool _wiFiConnected = false;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  searchI2c();

  setupLcd();
  setupOled();
  network.Connect(OnNetworkConnected);

  Serial.println("Begining thermometer");
  thermometer.begin();
  //setupTask();
  ReadSensorData(0);
  setupRouting();
}

void OnNetworkConnected() {
  Serial.println("=============== Network Connected! ========================");
  network.PrintNetworkInfo();
  showIpAddressOnOled();
}

void setupRouting() {
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
  oled.setTextSize(1);       // set text size
  oled.setTextColor(WHITE);  // set text color
  oled.setCursor(0, 0);      // set position to display
  oled.display();            // display on OLED
}

void setupTask() {	 	 
  xTaskCreate(	 	 
  ReadSensorData, 	 	 
  "Read sensor data", 	 	 
  1000, 	 	 
  NULL, 	 	 
  1, 	 	 
  NULL 	 	 
  );	 	 
}

void loop() {
  delay(2000);

  ReadSensorData(0);
  if (!network.IsWiFiConnected()) {
    network.Connect(OnNetworkConnected);
  }

  showIpAddressOnOled();
  server.handleClient();
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
  oled.print(network.GetIpAddress());
  oled.display();
}

void ReadSensorData(void * parameter) {
  //for (;;) {
    td = thermometer.read();
    printThermometerData(td);
    // delay the task
    //vTaskDelay(60000 / portTICK_PERIOD_MS);
  //}
}

void getEnv() {
  Serial.println("Get env");
  jsonDocument.clear();
  add_json_object("temperature", td.getTemperature(), "°C");
  add_json_object("humidity", td.getHumidity(), "%");
  add_json_object("feelsLike", td.getHeatIndex(), "°C");
  serializeJson(jsonDocument, buffer);
  server.send(200, "application/json", buffer);
}
