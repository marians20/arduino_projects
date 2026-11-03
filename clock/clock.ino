#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <time.h>
#include <Preferences.h> // Biblioteca pentru salvarea în memoria Flash
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

// ==========================================
// CONFIGURARE HARDWARE & CONSTANTE
// ==========================================
const int BUTTON_PIN = 14;
const int NEOPIXEL_PIN = 13;
const int RESET_TIMEOUT = 3000;
const int SHORT_PRESS_TIME = 50;

// FLAG PENTRU ACTIVARE/DEZACTIVARE BUTON
// Setează pe 'true' când cablezi butonul, 'false' pentru a-l dezactiva complet
const bool UTILIZEAZA_BUTON = false;

// Configurare Matrice NeoPixel Nativă (32x8 - Un singur panel)
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(32, 8, NEOPIXEL_PIN,
  NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);

// Configurare Fus Orar România
const char* TZ_INFO = "EET-2EEST,M3.5.0/3,M10.5.0/4";
const char* NTP_SERVER = "pool.ntp.org";

// Obiectul Preferences pentru memorie non-volatilă
Preferences preferences;

// Variabile globale pentru stări
unsigned long lastDisplayUpdate = 0;
unsigned long lastModeSwitch = 0;
bool afiseazaData = false;      // true = Dată, false = Ceas
bool separatorClipeste = true;  // Controlează clipirea punctelor la ceas

// Variabile pentru gestiunea butonului
unsigned long buttonPressedMillis = 0;
bool buttonStateLongPressed = false;
bool lastButtonState = HIGH;

// Variabile pentru controlul scroll-ului în portalul Wi-Fi
bool portalConfiguratieActiv = false;
String textScrollPortal = "";
int pozitieScrollX = 32; // Începe din marginea dreaptă a ecranului
unsigned long ultimaActualizareScroll = 0;
const int VITEZA_SCROLL_MS = 60; // Cu cât e mai mic, cu atât textul curge mai repede

// Listă de culori predefinite pentru ceas
uint32_t culori[] = {
  matrix.Color(0, 96, 128),  // 4: Cyan
  matrix.Color(64, 96, 0),    // 0: Roșu
  matrix.Color(0, 255, 0),    // 1: Verde
  matrix.Color(0, 0, 255),   // 2: Albastru
  matrix.Color(255, 255, 0),  // 3: Galben
  matrix.Color(255, 0, 255),  // 5: Magenta
  matrix.Color(255, 255, 255) // 6: Alb
};
const int NUMAR_CULORI = sizeof(culori) / sizeof(culori[0]);
int indexCuloareCurenta = 0;
uint32_t culoareCeas;

// ==========================================
// 1. MODULUL AFIȘAJ & MEMORIE
// ==========================================
void initializareAfisaj() {
  matrix.begin();
  matrix.setTextWrap(false); // Dezactivat wrap pentru a permite scroll-ul fluid pe un singur rând
  matrix.setBrightness(25);
  
  // Deschidem spațiul de stocare numit "ceas_setari" în mod Read-Only (true)
  preferences.begin("ceas_setari", true);
  indexCuloareCurenta = preferences.getInt("index_culoare", 0);
  preferences.end();

  // Validăm indexul citit
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

  // Salvăm noul index în Flash
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
  
  char buffer[6]; // Alocare corectă pentru HH:MM sau DD MM
  
  if (afiseazaData) {
    auto _culoareCeas = culori[(indexCuloareCurenta + 1) % NUMAR_CULORI];
    matrix.setTextColor(_culoareCeas);
    matrix.setCursor(2, 0); // Centrare aproximativă pentru DD MM
    sprintf(buffer, "%02d-%02d", timeinfo.tm_mday, timeinfo.tm_mon + 1); // tm_mon este 0-11
  } else {
    matrix.setTextColor(culoareCeas);
    matrix.setCursor(2, 0); // Centrare aproximativă pentru HH:MM
    if (separatorClipeste) {
      sprintf(buffer, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    } else {
      sprintf(buffer, "%02d %02d", timeinfo.tm_hour, timeinfo.tm_min);
    }
  }

  matrix.print(buffer);
  matrix.show();
}

// ==========================================
// 2. MODULUL WI-FI (WiFiManager + Callback + Scroll)
// ==========================================
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Configurare Wi-Fi necesară. Activare portal...");
  // Generăm textul dinamic care conține numele rețelei (Access Point-ului)
  textScrollPortal = "Conecteaza-te cu telefonul la retea: " + myWiFiManager->getConfigPortalSSID() + " ";
  pozitieScrollX = 32; // Resetăm poziția textului în dreapta ecranului
  portalConfiguratieActiv = true; // Activăm animația de scroll
}

void actualizeazaScrollPortal() {
  if (!portalConfiguratieActiv) return;

  // Actualizăm animația fără blocaj (non-blocking)
  if (millis() - ultimaActualizareScroll >= VITEZA_SCROLL_MS) {
    ultimaActualizareScroll = millis();
    matrix.clear();
    matrix.setTextColor(matrix.Color(255, 0, 0)); // Culoare roșie pentru atenționare
    matrix.setCursor(pozitieScrollX, 0);
    matrix.print(textScrollPortal);
    matrix.show();
    
    pozitieScrollX--; // Deplasăm textul spre stânga cu 1 pixel

    // Lungimea textului în pixeli (fontul standard are 6 pixeli lățime per caracter)
    int lungimeTextPixeli = textScrollPortal.length() * 6;
    
    // Resetăm textul la capătul din dreapta când a ieșit complet din ecran
    if (pozitieScrollX < -lungimeTextPixeli) {
      pozitieScrollX = 32;
    }
  }
}

void ConectareWiFi() {
  afiseazaMesajBoot("CONN", matrix.Color(0, 0, 255));
  WiFiManager wm;
  
  wm.setAPCallback(configModeCallback);
  wm.setConfigPortalTimeout(180);

  if(!wm.autoConnect("Ceas_NeoPixel_AP")) {
    Serial.println("Portalul Wi-Fi a expirat. Restart...");
    portalConfiguratieActiv = false;
    afiseazaMesajBoot("ERR", matrix.Color(255, 0, 0));
    delay(3000);
    ESP.restart();
  }

  // Dezactivăm portalul dacă s-a conectat cu succes
  portalConfiguratieActiv = false;
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
  if (!UTILIZEAZA_BUTON) return;
  
  bool currentButtonState = digitalRead(BUTTON_PIN);

  if (currentButtonState == LOW && lastButtonState == HIGH) {
    buttonPressedMillis = millis();
    buttonStateLongPressed = false;
  }

  if (currentButtonState == LOW) {
    if ((millis() - buttonPressedMillis > RESET_TIMEOUT) && !buttonStateLongPressed) {
      buttonStateLongPressed = true;
      Serial.println("Resetare setări Wi-Fi declanșată...");
      portalConfiguratieActiv = false; // Ne asigurăm că oprim scroll-ul înainte de wipe
      afiseazaMesajBoot("WIPE", matrix.Color(255, 165, 0));
      WiFiManager wm;
      wm.resetSettings();
      delay(1500);
      ESP.restart();
    }
  }

  if (currentButtonState == HIGH && lastButtonState == LOW) {
    if (!buttonStateLongPressed && (millis() - buttonPressedMillis > SHORT_PRESS_TIME)) {
      // Schimbă culoarea doar dacă ceasul este cel afișat (nu în modul portal)
      if (!portalConfiguratieActiv) {
        schimbaCuloareCeas();
        actualizeazaCeasPeEcran();
      }
    }
  }
  lastButtonState = currentButtonState;
}

// ==========================================
// STRUCTURA PRINCIPALĂ ARDUINO
// ==========================================
void setup() {
  Serial.begin(115200);
  
  if (UTILIZEAZA_BUTON) {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
  }
  
  initializareAfisaj();
  ConectareWiFi();
  initializareNTP();
}

void loop() {
  // Verificăm constant butonul fizic
  verificaButon();

  // Decidem ce randăm pe ecran în funcție de starea conexiunii
  if (portalConfiguratieActiv) {
    actualizeazaScrollPortal();
  } else {
    unsigned long currentMillis = millis();

    // 1. Alternează între modurile Ceas și Dată la fiecare 2000 ms (2 secunde)
    if (currentMillis - lastModeSwitch >= 2000) {
      lastModeSwitch = currentMillis;
      afiseazaData = !afiseazaData;
      
      // Când trecem pe modul Ceas, ne asigurăm că separatorul pornește aprins
      if (!afiseazaData) {
        separatorClipeste = true;
      }
      actualizeazaCeasPeEcran();
    }

    // 2. Rutină pentru clipirea separatorului ":" (la fiecare 500 ms) când este afișat Ceasul
    if (!afiseazaData) {
      if (currentMillis - lastDisplayUpdate >= 500) {
        lastDisplayUpdate = currentMillis;
        separatorClipeste = !separatorClipeste;
        actualizeazaCeasPeEcran();
      }
    }
  }
}
