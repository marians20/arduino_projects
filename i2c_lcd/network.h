#ifndef __NETWORK_H__
#define __NETWORK_H__
#include <WiFi.h>

class Network {
  public:
    Network(char* ssid, char* password, int retriesCount = 10);
    void Connect(void (*)());
    bool IsWiFiConnected();
    IPAddress GetIpAddress();
    void PrintNetworkInfo();
  private:
    char* _ssid;
    char* _password;
    int _retriesCount;
    bool _isConnected;
    bool _wasConnected;
    String GetWiFiStatus(int status);
    static void WiFiEvent(WiFiEvent_t event);
    static void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
};
#endif