#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>          // Biblioteca pentru portalul web

const int BUTTON_PIN = 13;        // Pinul pentru butonul de reset Wi-Fi
unsigned long buttonPressTime = 0;
const int RESET_TIMEOUT = 3000;   // Trebuie să ții apăsat 3 secunde pentru reset

void setup() {
    Serial.begin(115200);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // Initializare WiFiManager
    WiFiManager wm;

    // Optional: setezi un timeout pentru portal (daca nu se conecteaza nimeni in 3 minute, da restart)
    wm.setConfigPortalTimeout(180); 

    // Incearca conectarea sau deschide Access Point-ul "Ceas_NeoPixel" fara parola
    // Daca vrei parola la AP, adaugi al doilea parametru: wm.autoConnect("Ceas_NeoPixel", "parola123");
    if(!wm.autoConnect("Ceas_NeoPixel")) {
        Serial.println("Nu s-a putut conecta si a expirat timpul de configurare. Restart...");
        delay(3000);
        ESP.restart();
    }

    // Daca a ajuns aici, inseamna ca s-a conectat cu succes la Wi-Fi-ul tau!
    Serial.println("Conectat cu succes la Wi-Fi!");
    Serial.print("Adresa IP primita: ");
    Serial.println(WiFi.localIP());

    // Aici adaugi initializarea pentru NTP (ora de pe internet) si matricea NeoPixel
}

void loop() {
    // Verificam daca butonul de reset este apasat (valoarea este LOW datorita INPUT_PULLUP)
    if (digitalRead(BUTTON_PIN) == LOW) {
        if (buttonPressTime == 0) {
            buttonPressTime = millis(); // Salveaza momentul in care a fost apasat
        }

        // Daca butonul este tinut apasat mai mult de 3 secunde
        if (millis() - buttonPressTime > RESET_TIMEOUT) {
            Serial.println("Resetare date Wi-Fi si repornire...");
            WiFiManager wm;
            wm.resetSettings(); // Sterge Wi-Fi-ul salvat din memoria ESP32
            delay(1000);
            ESP.restart();      // Reporneste ESP32 (va intra automat in modul Access Point)
        }
    } else {
        buttonPressTime = 0; // Reseteaza cronometrul daca butonul a fost eliberat
    }

    // Codul tau pentru actualizarea ceasului si afisarea pe NeoPixeli ruleaza aici...
}
