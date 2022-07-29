#include "WiFiManager.h"
#include "MdnsServiceResolver.h"

class MqttClient {
  public:
    void Run();

  private:
    enum State {
      ConnectToWiFi,
      ResolveMqttService,
      SomeOtherMqtt,
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
        _state = ResolveMqttService;
      }
      break;
    }
    case ResolveMqttService: 
    {
      MdnsServiceResolver resolver;
      ResolvedServiceAddress resolvedAddress = resolver.ResolveServiceAddress("mqtt", "tcp");

      if (resolvedAddress.IpAddress == IPAddress(0,0,0,0))
      {
        Serial.println("Failed to resolve MQTT service. Retrying in 10 seconds...");
        delay(10000);
        return;
      }

      _state = SomeOtherMqtt;
      break;
    }
    case SomeOtherMqtt: break;
  }
}