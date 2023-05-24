/*
  on the button push, turns the relay on
  then waits estabilished time (DELAY)
  then turns the rely back off
*/

#include "StopwatchLib.h"
#include "deBounce.h"
#include "relay.h"
#include "pins.h"

#define RELAY D2
#define BUTTON D3

#define POTENTIOMETER A7
#define MIN_DELAY_SECONDS 5
#define MAX_DELAY_SECONDS 3600
#define MIN_ANALOG_VALUE 0
#define MAX_ANALOG_VALUE 1023

Stopwatch stopwatch;
Relay* relay;
DebounceButton* startButton(BUTTON, 50, INPUT_PULLUP);

unsigned int delaySeconds;
unsigned int oldElapsedSeconds = 0;

double a = 1. * (MAX_DELAY_SECONDS - MIN_DELAY_SECONDS) / (MAX_ANALOG_VALUE - MIN_ANALOG_VALUE);
double b = 1. * MIN_DELAY_SECONDS - a * MIN_ANALOG_VALUE;

void setup() {
  relay = new Relay(RELAY, LED_BUILTIN);
  relay->setup();
  relay->turnOff();

  startButton = new DebounceButton(BUTTON, 50, INPUT_PULLUP);
  startButton->setupButton();
  Serial.begin(9600);

  pinMode(POTENTIOMETER, INPUT_PULLUP);

  stopwatch.Reset();
}

void loop() {
  //1. Adjust delay
  unsigned int analogicValue = analogRead(POTENTIOMETER);

  delaySeconds = a * analogicValue + b;

  //2. turn relay on when button pushed
  if (startButton->read()) {
    relay->turnOn();
    stopwatch.Reset();
  }

  //3. turn relay off after time elapsed
  stopwatch.Update();
  int elapsed = stopwatch.GetElapsed() / 1e6;
  if (elapsed != oldElapsedSeconds) {
    oldElapsedSeconds = elapsed;
    Serial.print(elapsed);
    Serial.print(" / ");
    Serial.println(delaySeconds);
  }
  if (elapsed >= delaySeconds && elapsed < 4000) {
    relay->turnOff();
    stopwatch.Reset();
  }
}
