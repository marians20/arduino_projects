#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <arduinoFFT.h>

// ====== Hardware Config ======
#define LED_PIN       13
#define LED_COUNT     64
#define ROWS_COUNT    8
#define COLS_COUNT    8
#define BRIGHTNESS    64
#define MIC_PIN       34
#define SAMPLES       256
#define SAMPLING_FREQ 4000

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// ====== FFT Buffers ======
double vReal[SAMPLES];
double vImag[SAMPLES];
ArduinoFFT<double> FFT(vReal, vImag, SAMPLES, SAMPLING_FREQ);

// ====== Visualization Config ======
const int numBands = COLS_COUNT;   // one bar per column
const double bandEdges[numBands+1] = {
  20, 40, 100, 400, 1000, 2000, 4000, 8000, 16000
}; // perceptual bands (Hz)

// Gradient colors
uint32_t greenColor  = strip.Color(0, 128, 64);
uint32_t yellowColor = strip.Color(128, 64, 0);
uint32_t redColor    = strip.Color(128, 0, 0);

// Peak LED color (configurable)
uint32_t peakColor   = strip.Color(255, 255, 255); // white peak dot

// ====== Scaling Config ======
double sensitivity = 2;   // responsiveness
double bandWeight[numBands] = {1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0}; // psychoacoustic weighting
static double maxRMS = 1.0; // adaptive normalization state

// ====== Peak Hold Config ======
#define PEAK_DECAY_SPEED 1    // LEDs per frame
#define PEAK_HOLD_TIME   200  // ms to hold peak before falling

// ====== Helpers ======
int freqToBin(double freq) {
  int bin = (int)(freq * SAMPLES / SAMPLING_FREQ);
  if (bin >= SAMPLES/2) bin = SAMPLES/2 - 1;
  return bin;
}

uint32_t blendColor(uint32_t c1, uint32_t c2, float ratio) {
  uint8_t r1 = (c1 >> 16) & 0xFF;
  uint8_t g1 = (c1 >> 8) & 0xFF;
  uint8_t b1 = c1 & 0xFF;
  uint8_t r2 = (c2 >> 16) & 0xFF;
  uint8_t g2 = (c2 >> 8) & 0xFF;
  uint8_t b2 = c2 & 0xFF;
  uint8_t r = r1 + (r2 - r1) * ratio;
  uint8_t g = g1 + (g2 - g1) * ratio;
  uint8_t b = b1 + (b2 - b1) * ratio;
  return strip.Color(r, g, b);
}

// ====== Scaling Strategy ======
int scaleLevel(double rms, int band) {
  // 1. Aplică ponderi psihoacustice (dacă vrei să accentuezi anumite benzi)
  rms *= bandWeight[band];

  // 2. Noise floor (prag minim pentru semnale slabe)
  double noiseFloor = 1.0; // ajustează în funcție de microfon
  double adjustedRMS = rms + noiseFloor;

  // 3. Actualizare auto-gain (normalizare adaptivă)
  if (adjustedRMS > maxRMS) {
    maxRMS = adjustedRMS;
  } else {
    maxRMS = maxRMS * 0.95 + adjustedRMS * 0.05; // decay mai rapid
  }

  // 4. Conversie în dB
  double dB = 20.0 * log10(adjustedRMS + 1.0);

  // 5. Aplică un prag minim (de ex. -40 dB)
  if (dB < -40.0) dB = -40.0;

  // 6. Mapare dB → LED-uri
  // -40 dB = 0 LED-uri, 0 dB = ROWS_COUNT LED-uri
  int level = map((int)dB, -40, 0, 0, ROWS_COUNT);

  // 7. Constrângere la intervalul valid
  level = constrain(level, 0, ROWS_COUNT);

  return level;
}

// ====== Drawing ======
void drawBar(int row, int level) {
  for (int col = 0; col < level; col++) {
    float pos = (float)col / (ROWS_COUNT - 1);
    uint32_t barColor;
    if (pos < 0.5) {
      float ratio = pos / 0.5;
      barColor = blendColor(greenColor, yellowColor, ratio);
    } else {
      float ratio = (pos - 0.5) / 0.5;
      barColor = blendColor(yellowColor, redColor, ratio);
    }
    int ledIndex = row * ROWS_COUNT + col;
    strip.setPixelColor(ledIndex, barColor);
  }
}

// ====== State Arrays ======
int displayedLevel[numBands] = {0};
int peakLevel[numBands]      = {0};
unsigned long peakHoldTimer[numBands] = {0};

// ====== Spectrum Display ======
void displaySpectrum(double *spectrum) {
  strip.clear();

  for (int band = 0; band < numBands; band++) {
    int startBin = freqToBin(bandEdges[band]);
    int endBin   = freqToBin(bandEdges[band+1]);
    if (endBin >= SAMPLES/2) endBin = SAMPLES/2 - 1;

    double energy = 0;
    for (int i = startBin; i <= endBin; i++) {
      energy += spectrum[i] * spectrum[i];
    }
    double rms = sqrt(energy / (endBin - startBin + 1));

    int targetLevel = scaleLevel(rms, band);

    // Bar slow fall
    if (targetLevel > displayedLevel[band]) {
      displayedLevel[band] = targetLevel;
    } else {
      displayedLevel[band] = max(displayedLevel[band] - 1, targetLevel);
    }

    // Peak hold logic
    if (targetLevel > peakLevel[band]) {
      peakLevel[band] = targetLevel;
      peakHoldTimer[band] = millis(); // reset hold timer
    } else {
      if (millis() - peakHoldTimer[band] > PEAK_HOLD_TIME) {
        peakLevel[band] = max(peakLevel[band] - PEAK_DECAY_SPEED, 0);
      }
    }

    // Draw bar
    drawBar(band, displayedLevel[band]);

    // Draw peak LED
    if (peakLevel[band] > 0) {
      int ledIndex = band * ROWS_COUNT + (peakLevel[band] - 1);
      strip.setPixelColor(ledIndex, peakColor);
    }
  }

  strip.show();
}

// ====== Setup & Loop ======
void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.show();
}

void loop() {
  for (int i = 0; i < SAMPLES; i++) {
    vReal[i] = analogRead(MIC_PIN);
    vImag[i] = 0;
    delayMicroseconds(1000000 / SAMPLING_FREQ);
  }

  FFT.windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.complexToMagnitude(vReal, vImag, SAMPLES);

  displaySpectrum(vReal);
}