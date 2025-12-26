#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <arduinoFFT.h>

// ====== Hardware Config ======
#define LED_PIN       13
#define LED_COUNT     64
#define ROWS_COUNT     8
#define COLS_COUNT     8
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
const int numBands = COLS_COUNT;   // rows on matrix
const double bandEdges[numBands+1] = {
  40, 80, 200, 400, 1000, 2000, 4000, 8000, 16000
}; // perceptual bands (Hz)

// Gradient colors
uint32_t greenColor  = strip.Color(0, 128, 64);
uint32_t yellowColor = strip.Color(128, 64, 0);
uint32_t redColor    = strip.Color(128, 0, 0);

// ====== Scaling Config ======
double sensitivity = 10;   // responsiveness
double bandWeight[numBands] = {1.0, 1.0, 0.9, 0.8, 0.8, 1.0, 1.0, 1.9}; // psychoacoustic weighting
static double maxRMS = 1.0; // adaptive normalization state

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
  // Apply psychoacoustic weighting
  rms *= bandWeight[band];

  // Adaptive normalization
  if (rms > maxRMS) {
    maxRMS = rms;
  } else {
    maxRMS = maxRMS * 0.99 + rms * 0.01; // smooth decay
  }
  double norm = rms / (maxRMS + 1e-6);

  // Logarithmic compression
  double scaled = log10(norm * sensitivity + 1.0);

  // Map to LED levels
  int level = constrain((int)(scaled * 8.0), 0, ROWS_COUNT);
  return level;
}

// ====== Drawing ======
void drawBar(int row, int level) {
  for (int col = 0; col < level; col++) {
    float pos = (float)col / 7.0; // normalize 0..1
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

int displayedLevel[numBands] = {0};  // persistent bar levels

// ====== Spectrum Display ======
void displaySpectrum(double *spectrum) {
  strip.clear();
  for (int band = 0; band < numBands; band++) {
    int startBin = freqToBin(bandEdges[band]);
    int endBin   = freqToBin(bandEdges[band+1]);
    if (endBin >= SAMPLES/2) {
      endBin = SAMPLES/2 - 1;
    }

    // Compute RMS energy in band
    double energy = 0;
    for (int i = startBin; i <= endBin; i++) {
      energy += spectrum[i] * spectrum[i];
    }

    double rms = sqrt(energy / (endBin - startBin + 1));

    // Scale to LED level
    int targetLevel = scaleLevel(rms, band);

    // Apply slow falling effect
    if (targetLevel > displayedLevel[band]) {
      // rise immediately
      displayedLevel[band] = targetLevel;
    } else {
      // fall slowly (tune decay speed here)
      displayedLevel[band] = max(displayedLevel[band] - 1, targetLevel);
    }

    // Draw bar with smoothed level
    drawBar(band, displayedLevel[band]);
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
  // Sample audio
  for (int i = 0; i < SAMPLES; i++) {
    vReal[i] = analogRead(MIC_PIN);
    vImag[i] = 0;
    delayMicroseconds(1000000 / SAMPLING_FREQ);
  }

  // FFT
  FFT.windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.complexToMagnitude(vReal, vImag, SAMPLES);

  // Visualize
  displaySpectrum(vReal);
}