/*
  on the button push, turns the relay on
  then waits estabilished time (DELAY)
  then turns the rely back off
*/
#include <LiquidCrystal.h>
#include <DHT.h>
#include <StopwatchLib.h>
#include "deBounce.h"
#include <relay.h>
#include "usedPins.h"
#include "durationResolver.h";

#define MIN_DELAY_SECONDS 5
#define MAX_DELAY_SECONDS 3600

Stopwatch stopwatch;
DurationResolver durationResolver(POTENTIOMETER, MIN_DELAY_SECONDS, MAX_DELAY_SECONDS);
Relay* relay;
DebounceButton* startButton(BUTTON, 50, INPUT_PULLUP);
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

unsigned int delaySeconds = 0;
unsigned int oldElapsedSeconds = 0;
unsigned int elapsedSeconds = 0;

#define DHTTYPE DHT22      // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);  //// Initialize DHT sensor for normal 16mhz Arduino


void setup() {
  
  relay = new Relay(RELAY, LED_BUILTIN);
  relay->setup();
  relay->turnOff();

  startButton = new DebounceButton(BUTTON, 50, INPUT_PULLUP);
  startButton->setupButton();
  Serial.begin(9600);

  durationResolver.begin();

  stopwatch.Reset();
  lcd.begin(16, 4);
  lcd.print("Arduino");

  dht.begin();
}

void loop() {
  delaySeconds = durationResolver.getDuration();

  //2. turn relay on when button pushed
  if (startButton->read()) {
    Serial.println("Button pushed!!!");
    relay->turnOn();
    stopwatch.Reset();
  }

  //3. turn relay off after time elapsed
  do {
    stopwatch.Update();
    elapsedSeconds = stopwatch.GetElapsed() / 1e6;
  } while (elapsedSeconds > MAX_DELAY_SECONDS);

  // one per second
  if (elapsedSeconds != oldElapsedSeconds) {
    oldElapsedSeconds = elapsedSeconds;
    dispalyTimerStatus(relay->isOn(), delaySeconds, elapsedSeconds);
    displayTemperature();
  }
  // 4000 comes from a strange behaviour:
  // At first loop, stopwatch.GetElapsed() returns a value greater than 4e10.
  if (elapsedSeconds >= delaySeconds && elapsedSeconds < 4000) {
    relay->turnOff();
    stopwatch.Reset();
  }
}

void dispalyTimerStatus(bool isRelayOn, int delaySeconds, int elapsedSeconds) {
  lcd.setCursor(0,0);
  if(!isRelayOn) {
    lcd.print("Ready(");
    lcd.print(delaySeconds);
    lcd.print(")");
  } else {
     lcd.print("Running(");
    lcd.print(delaySeconds-elapsedSeconds);
    lcd.print(")");   
  }
  lcd.print("                ");
}

void displayTemperature() {
    auto hum = dht.readHumidity();
    auto temp = dht.readTemperature();

    lcd.setCursor(0,1);
    lcd.print(temp);
    lcd.print((char)223);
    lcd.print("C ");
    lcd.print(hum);
    lcd.print("%");
    lcd.print("     ");

    //Print temp and humidity values to serial monitor
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print(" %, Temp: ");
    Serial.print(temp);
    Serial.println(" Celsius");
}
