/*!
  @brief on the button push, turns the relay on
  then waits estabilished time (DELAY)
  then turns the rely back off
*/
#include <LiquidCrystal.h>
#include <StopwatchLib.h>
#include <deBounce.h>
#include <relay.h>
#include "usedPins.h"
#include "idurationResolver.h"
#include "iThermometer.h"
#include "iDisplay.h"
#include "potentiometerDurationResolver.h"
#include "thermometer.h"
#include "lcdDisplay.h"

#define MIN_DELAY_SECONDS 5
#define MAX_DELAY_SECONDS 3600
#define DHTTYPE DHT22

Stopwatch stopwatch;
IDurationResolver* durationResolver;
Relay* relay;
DebounceButton* startButton(BUTTON, 50, INPUT_PULLUP);
IDisplay* display;
IThermometer* thermometer;

unsigned int delaySeconds = 0;
unsigned int oldElapsedSeconds = 0;
unsigned int elapsedSeconds = 0;

void createComponents() {
  relay = new Relay(RELAY, LED_BUILTIN);
  startButton = new DebounceButton(BUTTON, 50, INPUT_PULLUP);
  durationResolver = new PotentiometerDurationResolver(POTENTIOMETER, MIN_DELAY_SECONDS, MAX_DELAY_SECONDS);
  display = new LcdDisplay(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
  thermometer = new Thermometer(DHTPIN, DHTTYPE);
}

void setup() {
  createComponents();

  relay->setup();
  relay->turnOff();

  startButton->setupButton();
  Serial.begin(9600);

  durationResolver->begin();

  stopwatch.Reset();
  display->begin();
  thermometer->begin();
}

void loop() {
  delaySeconds = durationResolver->getDuration();

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
    display->dispalyTimerStatus(relay->isOn(), delaySeconds, elapsedSeconds);
    display->showTemperatureAndHumidity(thermometer->read());
  }
  // 4000 comes from a strange behaviour:
  // At first loop, stopwatch.GetElapsed() returns a value greater than 4e10.
  if (elapsedSeconds >= delaySeconds && elapsedSeconds < 4000) {
    relay->turnOff();
    stopwatch.Reset();
  }
}
