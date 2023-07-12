#ifndef __NETWORK_H__
#define __NETWORK_H__
#include <WiFi.h>

class Network {
  public:
    Network(char* ssid, char* password, int retriesCount = 10);
    void Connect();
    boole IsWiFiConnected();
  private:
    char* _ssid;
    char* _password;
    int _retriesCount;
    bool _isConnected;
    String GetWiFiStatus(int status);
};
#endif