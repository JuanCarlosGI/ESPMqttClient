#ifndef SPIFFS_PROVIDER_H
#define SPIFFS_PROVIDER_H

#include "IWiFiCredentialsProvider.h"
#include <FS.h>
#include <ArduinoJson.h>

class SpiffsWiFiCredentialsProvider : public IWiFiCredentialsProvider
{
  const String FilePath = "wificreds.json";

  public:
    WiFiCredentials GetWiFiCredentials();
    void SaveWiFiCredentials(WiFiCredentials credentials);
};

WiFiCredentials SpiffsWiFiCredentialsProvider::GetWiFiCredentials()
{
  SPIFFS.begin();

  if (!SPIFFS.exists(FilePath))
  {
    SPIFFS.end();
    return WiFiCredentials();
  }

  File testFile = SPIFFS.open(FilePath, "r");
  if(!testFile)
  {
    Serial.println("Failed to open WiFi credentials file from SPIFFS");
    SPIFFS.end();
    return WiFiCredentials();
  }

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, testFile);
  testFile.close();

  if (error) {
    Serial.print("Failed to deserialize WiFi credentials from SPIFFS: ");
    Serial.println(error.f_str());
    return WiFiCredentials();
  }

  WiFiCredentials creds;
  creds.Ssid = String(doc["ssid"]);
  creds.Password = String(doc["password"]);

  return creds;
}

void SpiffsWiFiCredentialsProvider::SaveWiFiCredentials(WiFiCredentials credentials)
{
  StaticJsonDocument<200> doc;

  doc["ssid"] = credentials.Ssid;
  doc["password"] = credentials.Password;

  SPIFFS.begin();
  File jsonFile = SPIFFS.open(FilePath, "w");
  if (serializeJson(doc, jsonFile) == 0) {
    Serial.println("Failed to write WiFi credentials to SPIFFS");
  }
  jsonFile.close();
  SPIFFS.end();
}

#endif