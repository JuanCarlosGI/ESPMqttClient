#ifndef AccessPointServerWiFiCredentialsProvider_H
#define AccessPointServerWiFiCredentialsProvider_H

#include "IWiFiCredentialsProvider.h"
#include <ESP8266WebServer.h>
#include <Arduino.h>

class ApWiFiCredentialsProvider: public IWiFiCredentialsProvider {
  public:
    WiFiCredentials GetWiFiCredentials();
    void Startup();
    void Shutdown();
    void Reset();

  private:
    ESP8266WebServer* _server = NULL;
    WiFiCredentials _wiFiCredentials;
    bool _isDone = false;

    void HandleRoot();
    void HandleConfigure();
};

void ApWiFiCredentialsProvider::Startup() {
  Serial.print("Starting AP... ");
  Serial.println(WiFi.softAP("ESP8266") ? "Ready" : "Failed!");

  Serial.print("AP IP address = ");
  Serial.println(WiFi.softAPIP());

  _server = new ESP8266WebServer(80);
  _server->on("/", HTTP_GET, [&](){ this->HandleRoot(); });
  _server->on("/configure", HTTP_POST, [&](){ this->HandleConfigure(); });
  _server->begin();

  Serial.println("HTTP server set up. Waiting for WiFi credentials...");
}

void ApWiFiCredentialsProvider::Shutdown() 
{
  Serial.println("Shutting down HTTP server...");
  _server->close();
  delete _server;

  Serial.println("Shutting down AP...");
  WiFi.softAPdisconnect(true);
}

void ApWiFiCredentialsProvider::Reset()
{
  _isDone = false;
}

void ApWiFiCredentialsProvider::HandleRoot()
{
  const String postForms = "<!DOCTYPE html>\
<html>\
<body>\
<h2>Connect to your router.</h2>\
<form action=\"/configure\" method=\"post\" enctype=\"multipart/form-data\" >\
  <table>\
      <tr>\
          <td><label for=\"ssid\">SSID:</label></td>\
          <td><input type=\"text\" id=\"ssid\" name=\"ssid\"></td>\
        </tr>\
      <tr>\
          <td><label for=\"password\">Password:</label></td>\
          <td><input type=\"password\" id=\"password\" name=\"password\"></td>\
        </tr>\
    </table>\
    <input type=\"submit\" value=\"Submit\">\
</form>\
</body>\
</html>";

  _server->send(200, "text/html", postForms);
}

void ApWiFiCredentialsProvider::HandleConfigure()
{
  _wiFiCredentials.Ssid = _server->arg("ssid");
  _wiFiCredentials.Password = _server->arg("password");
  _server->send(200, "text/plain", "Attempting to connect to network '"+_wiFiCredentials.Ssid+"'. If device does not connect, try again in a few seconds.");

  _isDone = true;
}

WiFiCredentials ApWiFiCredentialsProvider::GetWiFiCredentials()
{
  if (_isDone) {
    return _wiFiCredentials;
  }

  _server->handleClient();
  return WiFiCredentials();
}

#endif