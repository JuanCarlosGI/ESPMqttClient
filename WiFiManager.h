#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "ApWiFiCredentialsProvider.h"
#include "SpiffsWiFiCredentialsProvider.h"


class WiFiManager 
{
  public:
    bool ConnectToWiFi();

  private:
    enum State {
      Startup,
      GetCreds,
      Connect,
      SaveCreds,
      Shutdown
    };

    SpiffsWiFiCredentialsProvider _memoryProvider;
    ApWiFiCredentialsProvider _apProvider;
    WiFiCredentials _credentials;
    State _state;
    bool _fromMemory;

    void DoStartup();
    void DoGetCreds();
    void DoConnect();
    void DoSaveCreds();
    void DoShutdown();
};

bool WiFiManager::ConnectToWiFi()
{
  switch (_state)
  {
    case Startup:
      DoStartup();
      break;
    case GetCreds:
      DoGetCreds();
      break;
    case Connect:
      DoConnect();
      break;
    case SaveCreds:
      DoSaveCreds();
      break;
    case Shutdown:
      DoShutdown();
      return true;
      break;
  }

  return false;
}

void WiFiManager::DoStartup()
{
  _credentials = _memoryProvider.GetWiFiCredentials();
  if (_credentials.Ssid != "")
  {
    Serial.println("Got stored WiFi credentials from SPIFFS.");
    _fromMemory = true;
    _state = Connect;
    return;
  }

  _apProvider.Startup();
  _state = GetCreds;
}

void WiFiManager::DoGetCreds()
{
  _credentials = _apProvider.GetWiFiCredentials();
  
  if (_credentials.Ssid != "")
  {
    _state = Connect;
  }
}

void WiFiManager::DoConnect()
{
  Serial.println("Trying to connect to network '" + _credentials.Ssid + "'");
  
  WiFi.begin(_credentials.Ssid, _credentials.Password);
  uint8_t status = WiFi.waitForConnectResult();

  if (status == WL_CONNECTED)
  {
    Serial.print("Connection established! IP address: ");
    Serial.println(WiFi.localIP());

    _state = SaveCreds;
  }
  else
  {
    Serial.print("Connection failed with status ");
    Serial.println(status);

    if (_fromMemory)
    {
      Serial.println("Retrying in 10 seconds.");
      delay(10000);
    }
    else
    {
      Serial.println("Waiting for new WiFi credentials...");
      _apProvider.Reset();
      _state = GetCreds; 
    }
  }
}

void WiFiManager::DoSaveCreds()
{
  if (!_fromMemory)
  {
    Serial.println("Saving WiFi credentials to SPIFFS.");
    _memoryProvider.SaveWiFiCredentials(_credentials);
  }

  _state = Shutdown;
}

void WiFiManager::DoShutdown()
{
  if (!_fromMemory)
  {
    _apProvider.Shutdown();
  }
}

#endif