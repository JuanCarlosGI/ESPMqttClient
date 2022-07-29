#include "WiFiManager.h"

class MqttClient {
  public:
    void Run();

  private:
    enum State {
      ConnectToWiFi,
      FinalSetup
    };

    State _state = ConnectToWiFi;
    WiFiManager _wifiManager;
};

void MqttClient::Run()
{
  switch (_state)
  {
    case ConnectToWiFi:
    {
      if (_wifiManager.ConnectToWiFi())
      {
        _state = FinalSetup;
      }
      break;
    }
    case FinalSetup: 
      break;
  }
}