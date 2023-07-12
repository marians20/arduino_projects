#include "network.h"

Network::Network(char* ssid, char* password, int retriesCount)
  : _ssid(ssid),
    _password(password),
    _isConnected(false),
    _retriesCount(retriesCount) {
  WiFi.mode(WIFI_STA);
}

bool Network::IsWiFiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

void Network::Connect() {
  int retriesCount = _retriesCount;
  WiFi.begin(SSID, PWD);
  while (IsWiFiConnected() && retriesCount >= 0) {
    retriesCount--;
    status = WiFi.status();
    Serial.print("WiFi Status: ");
    Serial.print(status);
    Serial.print(" ");
    Serial.println(get_wifi_status(status));
    delay(500);
  }
}

String Network::GetWiFiStatus(int status) {
  switch (status) {
    case WL_IDLE_STATUS:
      return "WL_IDLE_STATUS";
    case WL_SCAN_COMPLETED:
      return "WL_SCAN_COMPLETED";
    case WL_NO_SSID_AVAIL:
      return "WL_NO_SSID_AVAIL";
    case WL_CONNECT_FAILED:
      return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST:
      return "WL_CONNECTION_LOST";
    case WL_CONNECTED:
      return "WL_CONNECTED";
    case WL_DISCONNECTED:
      return "WL_DISCONNECTED";
    default:
      return "NO_WIFI_SHIELD";
  }
}
