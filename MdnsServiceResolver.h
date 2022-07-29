#ifndef MDNSSERVICERESOLVER_H
#define MDNSSERVICERESOLVER_H

#include <Arduino.h>
#include <ESP8266mDNS.h>

struct ResolvedServiceAddress
{
  IPAddress IpAddress;
  int Port;
};

// Resolves a Service-Protocol pair to an IP address and port. 
// Assumes that the device is already connected to a network and that only one device in the network is broadcasting the service.
class MdnsServiceResolver
{
  public:
    ResolvedServiceAddress ResolveServiceAddress(String service, String protocol);
};

ResolvedServiceAddress MdnsServiceResolver::ResolveServiceAddress(String service, String protocol)
{
  MDNS.begin("throwaway");

  Serial.printf("Sending mDNS query for %s.%s\n", service, protocol);
  int n = MDNS.queryService(service, protocol);
  Serial.printf("mDNS query got %d results.\n", n);

  if (n == 0)
  {
    return ResolvedServiceAddress();
  } 
  else 
  {
    if (n > 1)
    {
      Serial.println("More than one service was found. Using first one detected.");
    }

    ResolvedServiceAddress r;
    r.IpAddress = MDNS.IP(0);
    r.Port = MDNS.port(0);

    Serial.print("Resolved IP=");
    Serial.print(r.IpAddress);
    Serial.print(", Port=");
    Serial.println(r.Port);
    return r;
  }
}

#endif