#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <GyverHTU21D.h>
#include <LiquidCrystal_I2C.h>

/*
  comment following line if using an I2C LCD display
  uncomment following line if using an I2C OLED display
*/
//#define OLED

#ifdef OLED

#define SCREEN_ADDRESS 0x3C
#define SENSOR_ADDRESS 0x40

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#else

#define LCD_ADDRESS 0x27
LiquidCrystal_I2C lcd(LCD_ADDRESS, 20, 4);

#endif

GyverHTU21D htu;

float temp;
float hum;
 
void setup() {
  Wire.begin();
  Serial.begin(115200);
  scanI2C();

  #ifdef OLED
  initOledDisplay();
  #else
  initLcdDisplay();
  #endif

  initHtu();
}
 
void loop() {
  readSensor();

  #ifdef OLED
  displayDataToOled();
  #else
  displayDataToLcd();
  #endif

  delay(5000);          
}

#ifdef OLED
void initOledDisplay() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.display();
  delay(2000); // Pause for 2 seconds
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Hello, world!");
  display.display();
}

#else

void initLcdDisplay() {
  lcd.init();
  lcd.backlight();
}

#endif

void initHtu() {
  if (!htu.begin()) Serial.println(F("HTU21D error"));
}

void scanI2C() {
    byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error == 4) {
      Serial.print("Unknow error at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
}

void readSensor() {
  htu.requestTemperature();
  delay(100);
  if (htu.readTemperature()) {
    temp = htu.getTemperature();
    Serial.print("Temp: ");
    Serial.print(temp);
    Serial.println(" *C");
  }

  htu.requestHumidity();
  delay(100);
  if (htu.readHumidity()) {
    hum = htu.getHumidity();
    Serial.print("Hum: ");
    Serial.print(hum);
    Serial.println(" %");
  }
}

#ifdef OLED

void displayDataToOled() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(temp);
  display.println(" *C");
  display.print(hum);
  display.println(" %");
  display.display();
}

#else

void displayDataToLcd() {
  //lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.print(" *C");
  lcd.setCursor(0, 1);
  lcd.print(" Hum: ");
  lcd.print(hum);
  lcd.print("  %");
}

#endif