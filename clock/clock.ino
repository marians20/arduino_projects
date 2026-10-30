#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <time.h>
#include <Preferences.h>           // Biblioteca pentru salvarea în memoria Flash
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

// ==========================================
// CONFIGURARE HARDWARE & CONSTANTE
// ==========================================
const int BUTTON_PIN = 13;
const int NEOPIXEL_PIN = 14;
const int RESET_TIMEOUT = 3000;
const int SHORT_PRESS_TIME = 50;

// Configurare Matrice NeoPixel (32x8)
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, 4, 1, NEOPIXEL_PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);

// Configurare Fus Orar România
const char* TZ_INFO = "EET-2EEST,M3.5.0/3,M10.5.0/4"; 
const char* NTP_SERVER = "pool.ntp.org";

// Obiectul Preferences pentru memorie non-volatilă
Preferences preferences;

// Variabile globale pentru stări
unsigned long lastDisplayUpdate = 0;
bool separatorClipeste = true;

// Variabile pentru gestiunea butonului
unsigned long buttonPressedMillis = 0;
bool buttonStateLongPressed = false;
bool lastButtonState = HIGH;

// Listă de culori predefinite pentru ceas
uint32_t culori[] = {
    matrix.Color(255, 0, 0),     // 0: Roșu
    matrix.Color(0, 255, 0),     // 1: Verde
    matrix.Color(0, 0, 255),     // 2: Albastru
    matrix.Color(255, 255, 0),   // 3: Galben
    matrix.Color(0, 255, 255),   // 4: Cyan
    matrix.Color(255, 0, 255),   // 5: Magenta
    matrix.Color(255, 255, 255)  // 6: Alb
};
const int NUMAR_CULORI = sizeof(culori) / sizeof(culori);
int indexCuloareCurenta = 0;     
uint32_t culoareCeas;

// ==========================================
// 1. MODULUL AFIȘAJ & MEMORIE
// ==========================================
void initializareAfisaj() {
    matrix.begin();
    matrix.setTextWrap(false);
    matrix.setBrightness(30); 

    // Deschidem spațiul de stocare numit "ceas_setari" în mod Read-Only (true)
    preferences.begin("ceas_setari", true);
    // Citim indexul salvat. Dacă nu există (e prima pornire), valoarea implicită va fi 0 (Roșu)
    indexCuloareCurenta = preferences.getInt("index_culoare", 0);
    preferences.end();

    // Validăm indexul citit ca să fim siguri că nu e în afara vectorului
    if (indexCuloareCurenta < 0 || indexCuloareCurenta >= NUMAR_CULORI) {
        indexCuloareCurenta = 0;
    }

    culoareCeas = culori[indexCuloareCurenta]; 
    Serial.printf("Culoare încărcată din memorie! Index: %d\n", indexCuloareCurenta);
}

void afiseazaMesajBoot(String mesaj, uint32_t culoare) {
    matrix.clear();
    matrix.setTextColor(culoare);
    matrix.setCursor(0, 0);
    matrix.print(mesaj);
    matrix.show();
}

void schimbaCuloareCeas() {
    indexCuloareCurenta = (indexCuloareCurenta + 1) % NUMAR_CULORI;
    culoareCeas = culori[indexCuloareCurenta];
    
    // Deschidem spațiul de stocare în mod Read-Write (false) pentru a salva noul index
    preferences.begin("ceas_setari", false);
    preferences.putInt("index_culoare", indexCuloareCurenta);
    preferences.end();
    
    Serial.printf("Culoare schimbată și salvată! Index nou: %d\n", indexCuloareCurenta);
}

void actualizeazaCeasPeEcran() {
    struct tm timeinfo;
    
    if (!getLocalTime(&timeinfo)) {
        afiseazaMesajBoot("--:--", matrix.Color(100, 100, 100));
        Serial.println("Se așteaptă sincronizarea NTP...");
        return;
    }

    matrix.clear();
    matrix.setTextColor(culoareCeas);
    matrix.setCursor(2, 0); 

    char buffer[6];
    if (separatorClipeste) {
        sprintf(buffer, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    } else {
        sprintf(buffer, "%02d %02d", timeinfo.tm_hour, timeinfo.tm_min);
    }

    matrix.print(buffer);
    matrix.show();
}

// ==========================================
// 2. MODULUL WI-FI (WiFiManager)
// ==========================================
void ConectareWiFi() {
    afiseazaMesajBoot("Wi-Fi", matrix.Color(0, 0, 255)); 

    WiFiManager wm;
    wm.setConfigPortalTimeout(180); 

    if(!wm.autoConnect("Ceas_NeoPixel_AP")) {
        Serial.println("Portalul Wi-Fi a expirat. Restart...");
        delay(3000);
        ESP.restart();
    }

    Serial.println("Wi-Fi Conectat cu succes!");
}

// ==========================================
// 3. MODULUL TIMP (NTP)
// ==========================================
void initializareNTP() {
    configTzTime(TZ_INFO, NTP_SERVER, "time.nist.gov");
    Serial.println("Configurare NTP finalizată.");
}

// ==========================================
// 4. MODULUL CONTROL (Buton Fizic Inteligent)
// ==========================================
void verificaButon() {
    bool currentButtonState = digitalRead(BUTTON_PIN);

    if (currentButtonState == LOW && lastButtonState == HIGH) {
        buttonPressedMillis = millis();
        buttonStateLongPressed = false;
    }

    if (currentButtonState == LOW) {
        if ((millis() - buttonPressedMillis > RESET_TIMEOUT) && !buttonStateLongPressed) {
            buttonStateLongPressed = true;
            Serial.println("Resetare setări Wi-Fi declanșată...");
            afiseazaMesajBoot("RESET", matrix.Color(255, 165, 0)); 
            
            WiFiManager wm;
            wm.resetSettings();
            delay(1500);
            ESP.restart();
        }
    }

    if (currentButtonState == HIGH && lastButtonState == LOW) {
        if (!buttonStateLongPressed && (millis() - buttonPressedMillis > SHORT_PRESS_TIME)) {
            schimbaCuloareCeas();
            actualizeazaCeasPeEcran(); 
        }
    }

    lastButtonState = currentButtonState;
}

// ==========================================
// STRUCTURA PRINCIPALĂ ARDUINO
// ==========================================
void setup() {
    Serial.begin(115200);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    initializareAfisaj(); // Încarcă și culoarea salvată anterior
    ConectareWiFi();
    initializareNTP();
}

void loop() {
    verificaButon();

    if (millis() - lastDisplayUpdate >= 500) {
        lastDisplayUpdate = millis();
        separatorClipeste = !separatorClipeste; 
        
        actualizeazaCeasPeEcran();
    }
}