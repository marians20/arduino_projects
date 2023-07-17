/*
**********************************************************************
*
  Stereo VU Meter for 1 or 2 LED rings or strips build by ericBcreator
* Designed
  to be used with an Arduino UNO, Nano or compatible device.
**********************************************************************
*  Last updated 20171105 by ericBcreator
*
* This code is free for personal use, not for commercial purposes.
* Please leave this header intact.
*
* contact: ericBcreator@gmail.com
**********************************************************************
*/

//#define  DEBUG                                             // for debugging

//
//  include the NeoPixel library:
//

#include <Adafruit_NeoPixel.h>

//
//  uncomment the definition for the connected strip or ring(s)
//

//#define  led_ring_60
//#define led_strip_60
//#define led_strip_30
//#define led_2_rings_24
//#define  led_2_rings_30
//#define led_strip_200
#define led_strip_144

//
//  important setting: using potentiometer sensor values or not
//  This setting has  to be set right or the script will not work correctly:
// - set this to true  if using potentiometers
// - set this to false if not using potentiometers
//

const  int useSensorValues = true;                         

//
// setup pins
//

int  leftPin = A0, rightPin = A1;                          // left audio in on analog 0, right on analog 1
int brightnessPin = A4, sensitivityPin = A5;               // potentiometers for brightness and sensitivity on analog 4 and 5
int stripPin  = 6;                                         // DIN of leds on digital pin 6
int  showPeaksPin = 7;                                     // switch to toggle peaks on or off on digital pin 7
int momentarySwitch = false;                               // set false for an on/off toggle switch

//
// setup variables for the number of leds and led strip or 2 rings
//

#if defined (led_ring_60) //settings  for a 60 led ring

  int stripNumOfLeds = 60;                                  // the total number of leds
  uint32_t stripColor[31];                                  // half of the number of leds + 1
  int displayMiddleLed = false;                             // display the middle led (blue). set to true for one strip, false for two strips or rings
  int splitStrip = true;                                    // set to true when using 2 strips or rings, false for one strip
  int middleOffset = 0;                                     // offset for the middle led when using one strip
  int startupAnimationDelay =  6;                           // delay for the startup animation
  int orangeLimitAmount  = 0;                               // limit the amount of green of the middle LEDs  to make them more orange
  int swapLeftRight = false;                                // swap the left and right input values or not
  
  int dropDelay = 4;                                        // hold time before dropping the leds
  float dropFactor = .92;                                   // value for dropping the leds
  
  int peakTimeNoDropDelay = 250;                            // peak hold time when not dropping the peaks (set droppingPeak true or false)

  int peakTimeFirstDropDelay = 130;                         // peak hold time when  dropping the first peak
  int peakTimeDropDelay = 7;                                // peak hold time when dropping the rest of the peaks
  float peakDropFactor = .93;                               // value for dropping the peaks
  int droppingPeakFade  = false;                            // display the dropping peak fading to black  or not
  
  int bouncingPeaksNumOfLeds = 6;                           // how  many leds to bounce up (max)
  int bouncingPeakDelay = 4;                                // delay between peak bounce updates
  int bouncingPeakCounterInc = 10;                          // increase counter for each bounce update. note: it uses a 0-180 sin function for  the bouncing

#elif defined (led_strip_60) //settings for a 60 led ring


  int stripNumOfLeds = 60;                                  // the total number  of leds
  uint32_t stripColor[31];                                  // half of  the number of leds + 1
  int displayMiddleLed = true;                              // display the middle led (blue). set to true for one strip, false for two strips or  rings
  int splitStrip = false;                                   // set to true  when using 2 strips or rings, false for one strip
  int middleOffset = 1;                                     // offset for the middle led when using one strip
  int startupAnimationDelay =  6;                           // delay for the startup animation
  int orangeLimitAmount  = 0;                               // limit the amount of green of the middle LEDs  to make them more orange
  int swapLeftRight = false;                                // swap the left and right input values or not
  
  int dropDelay = 4;                                        // hold time before dropping the leds
  float dropFactor = .92;                                   // value for dropping the leds
  
  int peakTimeNoDropDelay = 250;                            // peak hold time when not dropping the peaks (set droppingPeak true or false)

  int peakTimeFirstDropDelay = 130;                         // peak hold time when  dropping the first peak
  int peakTimeDropDelay = 7;                                // peak hold time when dropping the rest of the peaks
  float peakDropFactor = .93;                               // value for dropping the peaks
  int droppingPeakFade  = false;                            // display the dropping peak fading to black  or not
  
  int bouncingPeaksNumOfLeds = 6;                           // how  many leds to bounce up (max)
  int bouncingPeakDelay = 4;                                // delay between peak bounce updates
  int bouncingPeakCounterInc = 10;                          // increase counter for each bounce update. note: it uses a 0-180 sin function for  the bouncing

#elif defined (led_strip_30) //settings for a 30 led strip

  int stripNumOfLeds = 30;                                  // the total number of leds
  uint32_t stripColor[16];                                  // half of the number of leds + 1
  int displayMiddleLed = true;                              // display the middle led (blue). set to true for one strip, false for two strips or rings
  int splitStrip = false;                                   // set to true when using 2 strips or rings, false for one strip
  int middleOffset = 1;                                     // offset for the middle led when using one strip
  int startupAnimationDelay = 10;                           // delay for the startup animation
  int orangeLimitAmount = 0;                                // limit the amount of green of the middle LEDs to make them more orange
  int swapLeftRight = false;                                // swap the left and right input values or not
  
  int dropDelay = 10;                                       // hold time before dropping the leds
  float dropFactor = .9;                                    // value for dropping the leds
  
  int peakTimeNoDropDelay = 250;                            // peak hold time when not dropping the peaks (set droppingPeak true or false)

  int peakTimeFirstDropDelay = 150;                         // peak hold time when dropping the first peak
  int peakTimeDropDelay = 15;                               // peak hold time when dropping the rest of the peaks
  float peakDropFactor = .94;                               // value for dropping the peaks
  int droppingPeakFade = false;                             // display the dropping peak fading to black or not
  
  int bouncingPeaksNumOfLeds = 3;                           // how many leds to bounce up (max)
  int bouncingPeakDelay = 4;                                // delay between peak bounce updates
  int bouncingPeakCounterInc = 9;                           // increase counter for each bounce update. note: it uses a 0-180 sin function for the bouncing

#elif defined (led_2_rings_24) //settings for 2 24 led rings

  int stripNumOfLeds = 48;
  uint32_t stripColor[25];
  int displayMiddleLed = false;
  int splitStrip = true;
  int middleOffset = 0;
  int startupAnimationDelay = 5;
  int orangeLimitAmount = 0;
  int swapLeftRight = false;
  
  int dropDelay = 2;
  float dropFactor = .96;
  
  int peakTimeNoDropDelay = 250;
  int peakTimeFirstDropDelay = 100;
  int peakTimeDropDelay = 10;

  float peakDropFactor = .94;
  int droppingPeakFade = false;
  
  int bouncingPeaksNumOfLeds = 3;
  int bouncingPeakDelay = 4;
  int bouncingPeakCounterInc = 9;

#elif defined(led_2_rings_30) //settings for 2 30 led rings


  int stripNumOfLeds = 60;
  uint32_t stripColor[31];
  int displayMiddleLed = false;
  int splitStrip = true;
  int middleOffset = 0;
  int startupAnimationDelay = 5;
  int orangeLimitAmount = 0;
  int swapLeftRight = false;
  
  int dropDelay = 2;
  float dropFactor = .96;
  
  int peakTimeNoDropDelay = 250;
  int peakTimeFirstDropDelay = 100;
  int peakTimeDropDelay = 10;

  float peakDropFactor = .94;
  int droppingPeakFade = false;
  
  int bouncingPeaksNumOfLeds = 3;
  int bouncingPeakDelay = 4;
  int bouncingPeakCounterInc = 9;

#elif defined (led_strip_200) //settings for a 200 led strip

  int stripNumOfLeds = 200;
  uint32_t stripColor[101];
  int displayMiddleLed = false;
  int splitStrip = true;
  int middleOffset = 0;
  int startupAnimationDelay = 1;
  int orangeLimitAmount = 0;
  int swapLeftRight = false;
  
  int dropDelay = 10;
  float dropFactor = .96;
  
  int peakTimeNoDropDelay = 250;
  int peakTimeFirstDropDelay = 100;
  int peakTimeDropDelay = 30;

  float peakDropFactor = .99;
  int droppingPeakFade = false;
  
  int bouncingPeaksNumOfLeds = 8;
  int bouncingPeakDelay = 4;
  int bouncingPeakCounterInc = 9;

#elif defined (led_strip_144) //settings for a 200 led strip

  int stripNumOfLeds = 145;
  uint32_t stripColor[73];
  int displayMiddleLed = true;
  int splitStrip = false;
  int middleOffset = 1;
  int startupAnimationDelay = 1;
  int orangeLimitAmount = 0;
  int swapLeftRight = false;
  
  int dropDelay = 10;
  float dropFactor = .85;
  
  int peakTimeNoDropDelay = 250;
  int peakTimeFirstDropDelay = 100;
  int peakTimeDropDelay = 5;

  float peakDropFactor = .94;
  int droppingPeakFade = false;
  
  int bouncingPeaksNumOfLeds = 10;
  int bouncingPeakDelay = 2;
  int bouncingPeakCounterInc = 10;
#endif

//
// setup other variables, user editable
//

// basic settings
int minValue = 10;                                        // min analog input value
int maxValue = 350;                                       // max analog input value (0-1023 equals 0-5V)
int sensitivityValue = 128;                               // 0 - 255, initial value (value read from the potentiometer if useSensorValues = true)
int maxSensitivity = 2 * 255;                             // let the 'volume' go up to 200%!
int ledBrightness = 30;                                   // 0 - 255, initial value (value read from the potentiometer if useSensorValues = true)
int sensorDeviationBrightness = 1;                        // eliminate fluctuating values
int overflowDelay = 20;                                   // overflow hold time

// peak settings
int displayPeaks = true;                                  // value will be set by the switch if useSensorValues = true
int droppingPeak = true;                                  // display dropping peaks or not. note: displayPeaks has to be true 
int bouncingPeaks = true;                                 // display bouncing peaks or not. note: displayPeaks has to be true 

//
// initialize other variables needed for the sketch
//

int numOfSegments = stripNumOfLeds / 2;
int halfNumOfSegments = numOfSegments / 2; 
int stripMiddle = stripNumOfLeds / 2;
int maxDisplaySegments = stripMiddle - 1;
float sensitivityFactor;

int brightnessValue, prevBrightnessValue;
float ledFactor, ledFactor_div_numOfSegments;

int leftValue = 0, rightValue = 0, maxReadValue = 0;
int leftAnalogValue = 0, rightAnalogValue = 0;

int prevLeftValue = 0, prevRightValue = 0;
int prevLeftAnalogValue = 0, prevRightAnalogValue = 0;

int i, j;
int dropLeft, dropRight;
int leftDropTime, rightDropTime;

int leftPeak = 0, rightPeak = 0;
int leftPeakTime = 0, rightPeakTime = 0;
int leftFirstPeak = true, rightFirstPeak = true;
int readShowPeaksPin, prevReadShowPeaksPin;

uint32_t stripMiddleColor, stripOverflowColor, stripHoldColor;

int leftPeakBouncing = false, rightPeakBouncing = false;
int leftPeakBounce = 0, rightPeakBounce = 0;
int prevLeftPeakBounce = 0, prevRightPeakBounce = 0;
int leftPeakBounceCounter = 0, rightPeakBounceCounter = 0;
int leftPeakBounceDelayCounter = 0, rightPeakBounceDelayCounter = 0;

//
// initialize the strip or rings
//

Adafruit_NeoPixel strip = Adafruit_NeoPixel(stripNumOfLeds, stripPin, NEO_GRB + NEO_KHZ800);

//
// setup
//

void setup() {
  #ifdef DEBUG
    Serial.begin(9600);
  #endif
  
  pinMode(showPeaksPin, INPUT);  

  strip.begin();

  setStripColors();
  startupAnimation();
  
  if (useSensorValues) {
    setInitialDisplayPeaks();
  } else {
    setSensitivityFactor();
  }
}

//
// main loop
//

void loop() {
  if (useSensorValues)  {
    readSensorValues();
  }
  
  readValues();
  drawValues();

  if (displayPeaks) {
    getPeaks();
    drawPeaks();
  }

  storePrevValues();
}

// 
// functions
//

void setInitialDisplayPeaks() {
  readShowPeaksPin = digitalRead(showPeaksPin);

  if (readShowPeaksPin == HIGH) {
    displayPeaks = false;
  } else {
    displayPeaks = true;
  }
  
  prevReadShowPeaksPin = readShowPeaksPin;
}

void readSensorValues() {
  readShowPeaksPin = digitalRead(showPeaksPin);

  if (momentarySwitch) {
    if (readShowPeaksPin == LOW && prevReadShowPeaksPin == HIGH) {
      if (displayPeaks == true) {
        displayPeaks = false;
        clearLeftPeak();
        clearRightPeak();
        
        if (momentarySwitch) {
          while (digitalRead(showPeaksPin) == LOW) {}
        }
      } else {
        displayPeaks = true;
      }
    }
  } else {
    if (readShowPeaksPin == LOW && prevReadShowPeaksPin == HIGH) {
      displayPeaks = true;
    } else if (readShowPeaksPin == HIGH && prevReadShowPeaksPin == LOW) {
      displayPeaks = false;
      clearLeftPeak();
      clearRightPeak();        
    }
  }

  prevReadShowPeaksPin = readShowPeaksPin;
  brightnessValue = analogRead(brightnessPin);
  brightnessValue = map(brightnessValue, 0, 1023, 0, 255);
  
  if (abs(brightnessValue - prevBrightnessValue) > sensorDeviationBrightness) {
    ledBrightness = brightnessValue;
    setStripColors();
    prevBrightnessValue = brightnessValue;
  }

  sensitivityValue = map(analogRead(sensitivityPin), 0, 1023, 0, 255);
  setSensitivityFactor();
}

void setSensitivityFactor() {
  //sensitivityValue_div_numOfSegments = sensitivityValue / numOfSegments;
  sensitivityFactor = ((float) sensitivityValue / 255 * (float)maxSensitivity / 255);
}

void readValues() {
  leftAnalogValue = analogRead(leftPin);
  rightAnalogValue = analogRead(rightPin);

  if (swapLeftRight) {
    int tempValue = leftAnalogValue;
    leftAnalogValue = rightAnalogValue;
    rightAnalogValue = tempValue;
  }

  if (leftAnalogValue < prevLeftAnalogValue) {
    leftDropTime++;

    if (leftDropTime > dropDelay) {
      leftAnalogValue = prevLeftAnalogValue * dropFactor;
      leftDropTime = 0;
    } else {
      leftAnalogValue = prevLeftAnalogValue;
    }
  }
   
  if (rightAnalogValue < prevRightAnalogValue) {
    rightDropTime++;
    if (rightDropTime > dropDelay) {
      rightAnalogValue = prevRightAnalogValue * dropFactor;
      rightDropTime = 0;
    } else {
      rightAnalogValue = prevRightAnalogValue;
    }
  }

  #ifdef DEBUG
    Serial.print(leftAnalogValue);
    Serial.print(" ");
    Serial.println(rightAnalogValue);
  #endif  

  // map values  
  leftValue = map(leftAnalogValue * sensitivityFactor, minValue, maxValue, 0, maxDisplaySegments);
  rightValue = map(rightAnalogValue * sensitivityFactor, minValue, maxValue, 0, maxDisplaySegments);

  if (leftValue > maxDisplaySegments) {
      leftValue = maxDisplaySegments;
      drawOverflow();
  }
  
  if (rightValue > maxDisplaySegments) {
      rightValue = maxDisplaySegments;
      drawOverflow();
  }
}

void storePrevValues() {
  prevLeftAnalogValue = leftAnalogValue;
  prevRightAnalogValue = rightAnalogValue;

  prevLeftValue = leftValue;
  prevRightValue = rightValue;
}

void getPeaks() {
  if (leftValue > leftPeak) {
    leftPeak = leftValue;
    leftPeakTime = 0;
    leftFirstPeak = true;

    if (bouncingPeaks) {
      leftPeakBouncing = true;
      leftPeakBounceCounter = 0;
      leftPeakBounceDelayCounter = 0;
    }
  }
  else {
    leftPeakTime++;
    if (droppingPeak) {
      if (leftFirstPeak) {
        if (leftPeakTime > peakTimeFirstDropDelay) {
          clearLeftPeak();
          leftFirstPeak = false;
        }
      } else {
        if (leftPeakTime > peakTimeDropDelay) {
          clearLeftPeak();
        }
      }
    } else {
      if (leftPeakTime > peakTimeNoDropDelay) {
        clearLeftPeak();
      }
    }
  }

  if (leftPeakBouncing) {
    if (leftFirstPeak) {
      leftPeakBounceDelayCounter++;
      if (leftPeakBounceDelayCounter >= bouncingPeakDelay) {
        leftPeakBounceDelayCounter = 0;
        leftPeakBounceCounter += bouncingPeakCounterInc;
        if (leftPeakBounceCounter >= 180) {
          clearLeftBouncePeak();
          clearLeftBounce();
        } else {        
          leftPeakBounce = min((sin(leftPeakBounceCounter * 0.0174532925) * bouncingPeaksNumOfLeds), (maxDisplaySegments - leftPeak));
          if (leftPeakBounce != prevLeftPeakBounce) {
            clearLeftBouncePeak();
          }

          prevLeftPeakBounce = leftPeakBounce;
        }
      }
    }
  }

  if (rightValue > rightPeak) {
    rightPeak = rightValue;
    rightPeakTime = 0;
    rightFirstPeak = true;

    if (bouncingPeaks) {
      rightPeakBouncing = true;
      rightPeakBounceCounter = 0;
      rightPeakBounceDelayCounter = 0;
    }
  } else {
    rightPeakTime++;
    if (droppingPeak) {
      if (rightFirstPeak) {
        if (rightPeakTime > peakTimeFirstDropDelay) {
          clearRightPeak();
          rightFirstPeak = false;
        }
      } else {
        if (rightPeakTime > peakTimeDropDelay) {
          clearRightPeak();
        }
      }
    } else {
      if (rightPeakTime > peakTimeNoDropDelay) {
        clearRightPeak();
      }
    }
  }

  if (rightPeakBouncing) {
    if (rightFirstPeak) {
      rightPeakBounceDelayCounter++;
      if (rightPeakBounceDelayCounter >= bouncingPeakDelay) {
        rightPeakBounceDelayCounter = 0;
        rightPeakBounceCounter += bouncingPeakCounterInc;
        if (rightPeakBounceCounter >= 180) {
          clearRightBouncePeak();
          clearRightBounce();
        } else {        
          rightPeakBounce = min((sin(rightPeakBounceCounter * 0.0174532925) * bouncingPeaksNumOfLeds), (maxDisplaySegments - rightPeak));
          if (rightPeakBounce != prevRightPeakBounce) {
            clearRightBouncePeak();
          }

          prevRightPeakBounce = rightPeakBounce;
        }
      }
    }
  }
}

void drawValues() {
  if (splitStrip) {
    for (i = middleOffset; i < leftValue; i++) {
      strip.setPixelColor(i, stripColor[i]);
    }

    for (i = prevLeftValue; i > leftValue; i--) {
      strip.setPixelColor(i, 0);
    }

    for (i = middleOffset; i < rightValue; i++) {
      strip.setPixelColor(stripMiddle + i, stripColor[i]);
    }
  
    for (i = prevRightValue; i > rightValue; i--) {
      strip.setPixelColor(stripMiddle + i, 0);
    }
  } else {
    for (i = middleOffset; i < leftValue; i++) {
      strip.setPixelColor(stripMiddle + i, stripColor[i]);
    }

    for (i = prevLeftValue; i > leftValue; i--) {
      strip.setPixelColor(stripMiddle + i, 0);
    }

    for (i = middleOffset; i < rightValue; i++) {
      strip.setPixelColor(stripMiddle - i, stripColor[i]);
    }

    for (i = prevRightValue; i > rightValue; i--) {
      strip.setPixelColor(stripMiddle - i, 0);
    }
  }

  if (displayMiddleLed) {
    strip.setPixelColor(stripMiddle, stripMiddleColor);
  }

  strip.show();
}

void drawPeaks() {
  if (leftPeak > 0) {
    if (droppingPeakFade && leftPeakBouncing == false) {
      stripHoldColor = strip.Color(max(1, (255 * leftPeak * ledFactor_div_numOfSegments)), 0, 0);
    } else {
      stripHoldColor = stripColor[numOfSegments];
    }

    if (splitStrip) {
      strip.setPixelColor((leftPeak + leftPeakBounce), stripHoldColor);
    } else {
      strip.setPixelColor(stripMiddle + (leftPeak + leftPeakBounce), stripHoldColor);
    }
  } 
  
  if (rightPeak > 0) {
    if (droppingPeakFade && rightPeakBouncing == false) {
      stripHoldColor = strip.Color(max(1, (255 * rightPeak * ledFactor_div_numOfSegments)), 0, 0);    
    } else {
      stripHoldColor  = stripColor[numOfSegments];
    }

    if (splitStrip) {
      strip.setPixelColor(stripMiddle + rightPeak + prevRightPeakBounce, stripHoldColor);
    } else { 
      strip.setPixelColor(stripMiddle - (rightPeak + prevRightPeakBounce), stripHoldColor);
    }
  }

  if (leftPeak > 0 || rightPeak > 0) {
    strip.show();
  }
}

void clearLeftPeak() {
  if (splitStrip) {
    strip.setPixelColor((leftPeak + prevLeftPeakBounce), 0);
  } else {
    strip.setPixelColor(stripMiddle + (leftPeak + prevLeftPeakBounce), 0);
  }

  if (droppingPeak) {
    leftPeak = leftPeak * peakDropFactor;
  } else {
    leftPeak = 0;
  }

  leftPeakTime = 0;
}

void clearLeftBounce() {
  leftPeakBouncing = false;
  leftPeakBounceCounter = 0;
  leftPeakBounce = 0;
  prevLeftPeakBounce = 0;
}

void clearLeftBouncePeak() {
  if (splitStrip) {
    strip.setPixelColor((leftPeak + prevLeftPeakBounce), 0);
  } else {
    strip.setPixelColor(stripMiddle + (leftPeak + prevLeftPeakBounce), 0);
  }
}

void clearRightPeak() {
  if (splitStrip) {
    strip.setPixelColor(stripMiddle + rightPeak + prevRightPeakBounce, 0);
  } else { 
    strip.setPixelColor(stripMiddle - (rightPeak + prevRightPeakBounce), 0);
  }
  if (droppingPeak) {
    rightPeak = rightPeak * peakDropFactor;
  } else {
    rightPeak = 0;
  }

  rightPeakTime = 0;
}

void clearRightBounce() {
  rightPeakBouncing = false;
  rightPeakBounceCounter = 0;
  rightPeakBounce = 0;
  prevRightPeakBounce = 0;
}

void clearRightBouncePeak() {
  if (splitStrip) {
    strip.setPixelColor((stripMiddle + rightPeak + prevRightPeakBounce), 0);
  } else {
    strip.setPixelColor(stripMiddle - (rightPeak + prevRightPeakBounce), 0);
  }
}

void drawOverflow() {
  for (i = 0; i <= numOfSegments; i++) {
    strip.setPixelColor(stripMiddle + i, stripOverflowColor);
    strip.setPixelColor(stripMiddle - i, stripOverflowColor);
  }

  strip.show();
  delay(overflowDelay);

  for (i = 0; i <= numOfSegments; i++) {
    strip.setPixelColor(stripMiddle + i, 0);
    strip.setPixelColor(stripMiddle - i, 0);
  }

  strip.show();
}

void setStripColors() {
  int orangeLimit;
  ledFactor = (float)ledBrightness / 255;

  float orangeFactor = orangeLimitAmount / halfNumOfSegments;
  ledFactor_div_numOfSegments = ledFactor / numOfSegments;
  
  stripOverflowColor = strip.Color(min(255, 255 * ledFactor * 1.5), 0, 0);
  stripMiddleColor = strip.Color(0, 0, 255 * ledFactor);

  stripColor[0] = strip.Color(0, 255 * ledFactor, 0);
  for (i = 1; i <= numOfSegments; i++) {
    if (i <= halfNumOfSegments) {
      orangeLimit = (i * orangeFactor);
    } else {
      orangeLimit = ((numOfSegments - i) * orangeFactor);
    }

    stripColor[i] = strip.Color((255 * i * ledFactor_div_numOfSegments), ((255 - orangeLimit) * (numOfSegments - i) * ledFactor_div_numOfSegments), 0); 
  }

  stripHoldColor = stripColor[numOfSegments];
}

void startupAnimation() {  
  for (j = 0; j < 2; j++) {
    for (i = 0; i <= numOfSegments; i++) {
      strip.setPixelColor(stripMiddle - i, stripColor[i]);
      strip.setPixelColor(stripMiddle + i, stripColor[i]);
      strip.show();
      delay(startupAnimationDelay);
    }
    
    for (i = 0; i <= numOfSegments; i++) {
      strip.setPixelColor(stripMiddle + i, 0);
      strip.setPixelColor(stripMiddle - i, 0);
      strip.show();
      delay(startupAnimationDelay);
    }
  }
}