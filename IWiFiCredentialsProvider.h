#ifndef NetworkConfiguration_H
#define NetworkConfiguration_H

#include <Arduino.h>

struct WiFiCredentials {
  String Ssid;
  String Password;
};

class IWiFiCredentialsProvider {
  public:
    virtual WiFiCredentials GetWiFiCredentials() = 0;
};

#endif